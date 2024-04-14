#ifndef _BOWSER_HTTP_REQUEST_HPP_
#define _BOWSER_HTTP_REQUEST_HPP_

#include <unordered_map>

#include <url.hpp>

class HttpRequest {
 public:
  HttpRequest(URL);

  // Should be later changed to enum
  HttpRequest& Method(std::string);
  HttpRequest& Version(std::string);
  HttpRequest& AddHeader(std::string, std::string);
  HttpRequest& Content(std::string);

  std::string Method();
  std::string Version();
  const std::unordered_map<std::string, std::string> Headers();
  std::string Content();

  /* String representation which of request. Ready to be sent. */
  std::string Str();
 private:
  std::string method_;
  URL url_;
  std::string version_;
  std::unordered_map<std::string, std::string> headers_;
  std::string content_;
};

#endif // _BOWSER_HTTP_REQUEST_HPP_