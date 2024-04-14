#include <connection.hpp>
#include <log.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <array>
#include <cstring>

Connection::Connection(URL url) {
  switch(url.Protocol()) {
  case URL::WebProtocol::kHttp:
    OpenHttp(url);
    return;
  case URL::WebProtocol::kHttps:
    OpenHttps(url);
    return;
  default:
    Log(LogLevel::kError, to_string(url.Protocol()));
  }
}

Connection::~Connection() {
  if(sfd_ != -1)
    close(sfd_);
}

bool Connection::SendMessage(std::vector<uint8_t> message) {
  Log(LogLevel::kInfo, "Sending " + std::to_string(message.size()) + " bytes");
  return write(sfd_, message.data(), message.size()) == message.size();
}

std::vector<uint8_t> Connection::ReceiveMessage() {
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

void Connection::Close() {
  if(sfd_ != -1) {
    close(sfd_);
    sfd_ = -1;
  }
}

void Connection::OpenHttp(URL url) {
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

void Connection::OpenHttps(URL url) {
  Log(LogLevel::kError, "HTTPS not YET implemented");
  is_valid_ = false;
  return;
}
