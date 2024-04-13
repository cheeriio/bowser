#ifndef _BOWSER_CONNECTION_HPP_
#define _BOWSER_CONNECTION_HPP_

#include <vector>
#include <cstdint>

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

  void OpenHttp(URL url);
  void OpenHttps(URL url);

  bool is_valid_ = true;
  int sfd_ = -1;

  static const uint32_t BUFFER_SIZE = 2<<16;
};

#endif // _BOWSER_CONNECTION_HPP_
