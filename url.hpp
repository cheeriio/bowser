

class URL {
 public:
  enum class Protocol {
    kHttp,
    kHttps,
    kUnknown
  };

 
  URL(std::string url) {
    size_t pos = url.find("://");
    
    if(pos == std::string::npos) {
      is_valid_ = false;
      Log(LogLevel::kError, "Protocol not found in URL");
      return;
    }
    
    std::string protocol_str = url.substr(0, pos);
    protocol_ = ProtocolFromString(protocol_str);
    
    if(protocol_ == Protocol::kUnknown) {
      is_valid_ = false;
      Log(LogLevel::kError, "'" + protocol_str + "' protocol not recognized");
      return;
    }
    
    url = url.erase(0, pos + 1);
    
    pos = url.find("/");
    
    if(pos != std::string::npos) {
      host_ = url;
      return;
    }
    
    host_ = url.substr(0, pos);
    path_ = url.substr(pos, url.length() - pos);
  }

  Protocol Protocol() { return protocol_; }

  std::string Host() { return host_; }

  std::string Path() { return path_; }

  uint16_t Port() { 
    if(port_set_)
      return port_;
    return DefaultPort(protocol_);
  };

  bool IsValid() { return is_valid_; }

 private:
  Protocol ProtocolFromString(std::string str) {
    switch(str) {
      case "http":
        return Protocol::kHttp;
      case "https":
        return Protocol::kHttps;
      default:
        return Protocol::kUnknown;
    }
  }

  uint16_t DefaultPort(Protocol p) {
    switch(p) {
      case Protocol::kHttp:
        return PORT_HTTP;
      case Protocol::kHttps:
        return PORT_HTTPS;
      default:
        return 0;
    }
  }

  Protocol protocol_;
  std::string host_, path_;
  uint16_t port_;
  bool port_set_ = false;
  bool is_valid_ = true;
}

std::string to_string(URL::Protocol p) {
  using URL::Protocol;

  switch(p) {
    case Protocol::kHttp:
      return "http";
    case Protocol::kHttps:
      return "https";
    default:
      return "unknown";
  }
}

std::ostream& operator<<(std::ostream& os, URL::Protocol p) {
  os << to_string(p);
  return os;
}


