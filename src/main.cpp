#include <iostream>
#include <sstream>

#include <url.hpp>
#include <connection.hpp>

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " <url>\n";
    return 1;
  }

  URL url(argv[1]);
  Connection connection(url);

  if(!connection.IsValid()) {
    std::cout << "Could not establish a connection with " + url.Host() << "\n";
    return 0;
  }

  std::stringstream message_sstr;
  message_sstr << "GET " << url.Path() << " HTTP/1.0\r\n"
               << "Host: " << url.Host() << "\r\n"
               << "\r\n";
  std::string message_str = message_sstr.str();
  std::vector<uint8_t> message(message_str.begin(), message_str.end());

  connection.SendMessage(message);

  std::vector<uint8_t> response = connection.ReceiveMessage();

  std::string response_str(response.begin(), response.end());

  std::cout << "Received response:\n";
  std::cout << response_str;

  return 0;
}