#include <connection.hpp>
#include <log.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <array>
#include <cstring>

#if (SSLEAY_VERSION_NUMBER >= 0x0907000L)
# include <openssl/conf.h>
#endif

namespace {

  // Perhaps this is not the greatest place for initialization
  void init_openssl_library(void) {
    static bool setup = false;
    if(setup)
      return;
    setup = true;

    (void)SSL_library_init();
    SSL_load_error_strings();
    OPENSSL_config(NULL);
      
  #if defined (OPENSSL_THREADS)
    fprintf(stdout, "Warning: thread locking is not implemented\n");
  #endif
  }

}

Connection::Connection(URL url) {
  switch(url.Protocol()) {
  case URL::WebProtocol::kHttps:
    OpenTLS(url);
    return;
  case URL::WebProtocol::kHttp:
    OpenRaw(url);
    return;
  default:
    Log(LogLevel::kError, to_string(url.Protocol()));
  }
}

Connection::~Connection() {
  Close();
}

bool Connection::SendMessage(std::vector<uint8_t> message) {
  if(is_tls_)
    return SendTLS(message);
  else
    return SendRaw(message);
}

std::vector<uint8_t> Connection::ReceiveMessage() {
  if(is_tls_)
    return ReceiveTLS();
  else
    return ReceiveRaw();
}

void Connection::Close() {
  if(sfd_ != -1 && !is_tls_) {
    close(sfd_);
    sfd_ = -1;
  }

  if(is_tls_) {
    if(web_ != NULL)
      BIO_free_all(web_);
    if(NULL != ctx_)
      SSL_CTX_free(ctx_);
  }
}

void Connection::OpenRaw(URL url) {
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = 0;

  s = getaddrinfo(url.Host().data(), std::to_string(url.Port()).data(), &hints, &result);
  if(s != 0) {
    Log(LogLevel::kError, "Could not get addrinfo from " + url.Host() + " : " + std::to_string(url.Port()));
    is_valid_ = false;
    return;
  }

  for(rp = result; rp != NULL; rp = rp->ai_next) {
    sfd_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if(sfd_ == -1)
      continue;

    if(connect(sfd_, rp->ai_addr, rp->ai_addrlen) != -1)
      break;

    close(sfd_);
  }

  freeaddrinfo(result);

  if(rp == NULL) {
    Log(LogLevel::kError, "Could not connect to " + url.Host() + ":" + std::to_string(url.Port()));
    is_valid_ = false;
    return;
  }

  Log(LogLevel::kInfo, "Established a connection with " + url.Host() + ":" + std::to_string(url.Port()));
}

void Connection::OpenTLS(URL url) {
  is_tls_ = true;
  long res = 1;
  SSL *ssl = NULL;

  init_openssl_library();

  const SSL_METHOD* method = SSLv23_method();
  if(!(NULL != method)) {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not create SSL method");
    return;
  }

  ctx_ = SSL_CTX_new(method);
  if(!(ctx_ != NULL)) {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not create a SSL context");
    return;
  }

  SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER, [](auto pre, auto){ return pre; });
  SSL_CTX_set_verify_depth(ctx_, 4);

  const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
  SSL_CTX_set_options(ctx_, flags);

  res = SSL_CTX_load_verify_locations(ctx_, NULL, "/etc/ssl/certs");
  if(!res) {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not verify locations");
    return;
  }

  web_ = BIO_new_ssl_connect(ctx_);
  if(!web_) {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not create a BIO object");
    return;
  }

  std::string complete_url = url.Host() + ":" + std::to_string(url.Port());
  res = BIO_set_conn_hostname(web_, complete_url.data());
  if(!res) {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not connect to " + url.Host() + " : " + std::to_string(url.Port()));
    return;
  }

  auto fail_func = [&]() {
    is_valid_ = false;
    Log(LogLevel::kError, "Could not do something");
    // Need to read a bit about ssl library
  };

  BIO_get_ssl(web_, &ssl);
  if(!ssl) {
    fail_func();
    return;
  }

  const char PREFERRED_CIPHERS[] = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
  res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
  if(!res) {
    fail_func();
    return;
  }

  res = SSL_set_tlsext_host_name(ssl, url.Host().data());
  if(!res) {
    fail_func();
    return;
  }

  res = BIO_do_connect(web_);
  if(!res) {
    fail_func();
    return;
  }

  res = BIO_do_handshake(web_);
  if(!res) {
    fail_func();
    return;
  }

  X509* cert = SSL_get_peer_certificate(ssl);
  if(cert)
    X509_free(cert);

  if(cert == NULL) {
    fail_func();
    return;
  }

  res = SSL_get_verify_result(ssl);
  if(X509_V_OK != res) {
    fail_func();
    return;
  }
}

std::vector<uint8_t> Connection::ReceiveRaw() {
  Log(LogLevel::kInfo, "Awaiting a message");
  std::array<uint8_t, BUFFER_SIZE> buffer;
  std::vector<uint8_t> result;
  ssize_t read_bytes;

  while((read_bytes = read(sfd_, buffer.data(), BUFFER_SIZE)) > 0) {
    Log(LogLevel::kInfo, "Read " + std::to_string(read_bytes) + " bytes");
    result.insert(result.end(), buffer.begin(), buffer.begin() + read_bytes);
  }

  if(read_bytes == -1)
    Log(LogLevel::kError, "Error reading from socket");

  return result;
}

std::vector<uint8_t> Connection::ReceiveTLS() {
  Log(LogLevel::kInfo, "Awaiting a message");
  std::array<uint8_t, BUFFER_SIZE> buffer;
  std::vector<uint8_t> result;
  ssize_t read_bytes;

  do {
    read_bytes = BIO_read(web_, buffer.data(), BUFFER_SIZE);
    result.insert(result.end(), buffer.begin(), buffer.begin() + read_bytes);
    if(read_bytes > 0)
      Log(LogLevel::kInfo, "Read " + std::to_string(read_bytes) + " bytes");
  } while(read_bytes > 0 || BIO_should_retry(web_));

  return result;
}

bool Connection::SendRaw(std::vector<uint8_t> message) {
  Log(LogLevel::kInfo, "Sending " + std::to_string(message.size()) + " bytes");
  return write(sfd_, message.data(), message.size()) == message.size();
}

bool Connection::SendTLS(std::vector<uint8_t> message) {
  Log(LogLevel::kInfo, "Sending " + std::to_string(message.size()) + " bytes");
  bool ret = BIO_puts(web_, (const char*)message.data());
  return ret;
}
