#include <http_request.hpp>

#include <sstream>

HttpRequest::HttpRequest(URL url) : url_(url) {}

HttpRequest& HttpRequest::Method(std::string method) {
  method_ = method;
  return *this;
}

HttpRequest& HttpRequest::Version(std::string version) {
  version_ = version;
  return *this;
}

HttpRequest& HttpRequest::AddHeader(std::string key, std::string value) {
  headers_.insert({key, value});
  return *this;
}

HttpRequest& HttpRequest::Content(std::string content) {
  content_ = content;
  return *this;
}

std::string HttpRequest::Method() {
  return method_;
}

std::string HttpRequest::Version() {
  return version_;
}

const std::unordered_map<std::string, std::string> HttpRequest::Headers() {
  return headers_;
}

std::string HttpRequest::Content() {
  return content_;
}

std::string HttpRequest::Str() {
  std::stringstream result;
  result << method_ << " " << url_.Path() << " " << version_ << "\r\n";

  if(headers_.count("Host") == 0)
    result << "Host: " << url_.Host() << "\r\n";

  for(auto p : headers_) {
    result << p.first << ": " << p.second << "\r\n";
  }

  result << "\r\n";
  result << content_;

  return result.str();
}