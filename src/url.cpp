#include <url.hpp>
#include <log.hpp>
#include <const.hpp>

#include <unordered_map>
#include <algorithm>

namespace {
  using WebProtocol = URL::WebProtocol;
}

URL::URL(std::string url) {
  size_t pos = url.find("://");
  
  if(pos == std::string::npos) {
    is_valid_ = false;
    Log(LogLevel::kError, "Protocol not found in URL");
    return;
  }
  
  std::string protocol_str = url.substr(0, pos);
  protocol_ = ProtocolFromString(protocol_str);
  
  if(protocol_ == WebProtocol::kUnknown) {
    is_valid_ = false;
    Log(LogLevel::kError, "'" + protocol_str + "' protocol not recognized");
    return;
  }
  
  url = url.erase(0, pos + 3);

  pos = url.find(":");

  if(pos != std::string::npos) {
    port_set_ = true;
    host_ = url.substr(0, pos);
    url.erase(0, pos + 1);
  } else {
    pos = url.find("/");
    if(pos == std::string::npos) {
      host_ = url;
      path_ = "/";
      return;
    } else {
      host_ = url.substr(0, pos);
      url.erase(0, pos);
      path_ = url;
      return;
    }
  }

  pos = url.find("/");

  std::string port;

  if(pos == std::string::npos) {
    port = url;
    url = "/";
  } else {
    port = url.substr(0, pos);
    url.erase(0, pos);
  }

  if(!std::all_of(port.begin(), port.end(), ::isdigit) || port.size() == 0) {
    Log(LogLevel::kError, "'" + port + "' is not a valid port");
    is_valid_ = false;
    return;
  }

  port_ = std::stoi(port);
  path_ = url;

  Log(LogLevel::kInfo, "Host: " + host_ + ", Port: " + std::to_string(port_) + ", Path: " + path_);
}

WebProtocol URL::Protocol() { return protocol_; }

std::string URL::Host() { return host_; }

std::string URL::Path() { return path_; }

uint16_t URL::Port() { 
  if(port_set_)
    return port_;
  return DefaultPort(protocol_);
};

bool URL::IsValid() { return is_valid_; }

WebProtocol URL::ProtocolFromString(std::string str) {
  static std::unordered_map<std::string, WebProtocol> protocol_map = {
    {"http", WebProtocol::kHttp},
    {"https", WebProtocol::kHttps},
  };

  if(protocol_map.count(str))
    return protocol_map.at(str);
  else
    return WebProtocol::kUnknown;
}

uint16_t URL::DefaultPort(WebProtocol p) {
  switch(p) {
    case WebProtocol::kHttp:
      return HTTP_PORT;
    case WebProtocol::kHttps:
      return HTTPS_PORT;
    default:
      return 0;
  }
}

std::string to_string(URL::WebProtocol p) {
  switch(p) {
    case WebProtocol::kHttp:
      return "http";
    case WebProtocol::kHttps:
      return "https";
    default:
      return "unknown";
  }
}

std::ostream& operator<<(std::ostream& os, URL::WebProtocol p) {
  os << to_string(p);
  return os;
}
