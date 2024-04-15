#ifndef _BOWSER_HTTP_RESPONSE_HPP_
#define _BOWSER_HTTP_RESPONSE_HPP_

#include <unordered_map>
#include <string>
#include <vector>

class HttpResponse {
 public:
  HttpResponse(std::vector<uint8_t>);

  const std::unordered_map<std::string, std::string>& Headers();
  std::string Content();
  int Status();
  std::string StatusDescription();
  bool IsValid();
  std::string Version();

 private:
  std::unordered_map<std::string, std::string> headers_;
  std::string content_;
  int status_;
  std::string description_;
  std::string version_;

  bool is_valid_ = true;
};

#endif // _BOWSER_HTTP_RESPONSE_HPP_