#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class Connection {
 public:
  Connection(URL url) {
    switch(url.Protocol()) {
    case URL::Protocol::kHttp:
      OpenHttp(url);
      return;
    case URL::Protocol::kHttps:
      OpenHttps(url);
      return;
    default:
      Log(LogLevel::kError, to_string(url.protocol));
    }
  }

  ~Connection() {}

  bool SendMessage(std::string message) {

  }

  std::string ReceiveMessage()

 private:

  void OpenHttp(URL url) {
    int sfd, s;
    char buf[BUF_SIZE];
    size_t len;
    ssize_t nread;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    s = getaddrinfo(url.Host(), url.Port(), &hints, &result);
    if(s != 0) {
      Log(LogLevel::kError, "Could not get addrinfo from " + url.Host() + ":" + url.Port());
      is_valid_ = false;
      return;
    }

    for(rp = result; rp != NULL; rp = rp->ai_next) {
      sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

      if(sfd == -1)
        continue;

      if(connect(sfd, rp->ai_addr, rp->ai_addrelen) != -1)
        break;

      close(sfd);
    }

    freeaddrinfo(result);

    if(rp == NULL) {
      Log(LogLevel::kError, "Could not connect to " + url.Host() + ":" + url.Port());
      is_valid_ = false;
      return;
    }

    std::string message = "GET /index.html HTTP/1.0\r\n"
                          "Host: example.org\n";

    if(write(sfd, message.str(), message.length()) != message.length()) {

    }
  }

  void OpenHttps(URL url) {
    return;
  }

  bool is_valid_ = true;
}