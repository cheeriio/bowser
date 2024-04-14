#include <http_response.hpp>
#include <log.hpp>

#include <sstream>
#include <vector>
#include <algorithm>

HttpResponse::HttpResponse(std::vector<uint8_t> data) {
  std::string response(data.begin(), data.end());
  std::stringstream sstr(response);
  std::string line;

  Log(LogLevel::kInfo, "Parsing http response:\n" + response);

  auto fail_func = [&]() {
    is_valid_ = false;
    Log(LogLevel::kError, "Failed to parse a Http response");
  };

  if(!std::getline(sstr, line)) {
    fail_func();
    return;
  }

  size_t pos = line.find(" ");
  if(pos == std::string::npos) {
    fail_func();
    return;
  }

  version_ = line.substr(0, pos);
  line.erase(0, pos + 1);

  pos = line.find(" ");
  if(pos == std::string::npos) {
    fail_func();
    return;
  }

  std::string code = line.substr(0, pos);
  description_ = line.substr(pos + 1, line.size());

  if(!std::all_of(code.begin(), code.end(), ::isdigit) || code.size() == 0) {
    fail_func();
    return;
  }

  status_ = std::stoi(code);

  while(std::getline(sstr, line)) {
    if(line == "\r")
      break;

    pos = line.find(": ");
    if(pos == std::string::npos) {
      fail_func();
      return;
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 2, line.size());

    headers_.insert({key, value});
  }

  content_ = sstr.str().substr(sstr.tellg());
}

const std::unordered_map<std::string, std::string> HttpResponse::Headers() {
  return headers_;
}

std::string HttpResponse::Content() {
  return content_;
}

int HttpResponse::Status() {
  return status_;
}

std::string HttpResponse::StatusDescription() {
  return description_;
}

bool HttpResponse::IsValid() {
  return is_valid_;
}

std::string HttpResponse::Version() {
  return version_;
}
