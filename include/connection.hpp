#ifndef _BOWSER_CONNECTION_HPP_
#define _BOWSER_CONNECTION_HPP_

#include <vector>
#include <cstdint>
#include <openssl/ssl.h>

#include <url.hpp>

class Connection {
 public:
  Connection(URL url);
  ~Connection();

  bool SendMessage(std::vector<uint8_t> message);
  std::vector<uint8_t> ReceiveMessage();

  void Close();

  inline bool IsValid() { return is_valid_; }

 private:
  void OpenRaw(URL url);
  void OpenTLS(URL url);

  std::vector<uint8_t> ReceiveRaw();
  std::vector<uint8_t> ReceiveTLS();

  bool SendRaw(std::vector<uint8_t>);
  bool SendTLS(std::vector<uint8_t>);

  bool is_valid_ = true;
  int sfd_ = -1;

  static const uint32_t BUFFER_SIZE = 2<<16;

  bool is_tls_ = false;
  SSL_CTX* ctx_ = NULL;
  BIO *web_ = NULL;
};

#endif // _BOWSER_CONNECTION_HPP_
