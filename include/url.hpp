#ifndef _BOWSER_URL_HPP_
#define _BOWSER_URL_HPP_

#include <string>

class URL {
 public:
  enum class WebProtocol {
    kHttp,
    kHttps,
    kUnknown
  };

  URL(std::string url);

  WebProtocol Protocol();
  std::string Host();
  std::string Path();
  uint16_t Port();

  bool IsValid();

 private:
  WebProtocol ProtocolFromString(std::string str);

  uint16_t DefaultPort(WebProtocol p);

  WebProtocol protocol_;
  std::string host_, path_;
  uint16_t port_;
  bool port_set_ = false;
  bool is_valid_ = true;
};

std::string to_string(URL::WebProtocol p);

std::ostream& operator<<(std::ostream& os, URL::WebProtocol p);

#endif // _BOWSER_URL_HPP_
