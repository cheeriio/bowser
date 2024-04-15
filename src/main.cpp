#include <iostream>
#include <sstream>

#include <url.hpp>
#include <connection.hpp>
#include <http_request.hpp>
#include <http_response.hpp>
#include <html_parser.hpp>

#include <gzip/decompress.hpp>

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " <url>\n";
    return 1;
  }

  URL url(argv[1]);

  if(!url.IsValid()) {
    std::cout << "Invalid url\n";
    return 0;
  }

  Connection connection(url);

  if(!connection.IsValid()) {
    std::cout << "Could not establish a connection with " + url.Host() << "\n";
    return 0;
  }

  HttpRequest request(url);
  request.Method("GET")
         .Version("HTTP/1.1")
         .AddHeader("Connection", "close")
         .AddHeader("User-Agent", "Chewbacca")
         .AddHeader("Accept-Encoding", "gzip");

  std::string message_str = request.Str();
  std::vector<uint8_t> message(message_str.begin(), message_str.end());
  connection.SendMessage(message);
  
  std::vector<uint8_t> response = connection.ReceiveMessage();
  HttpResponse response_obj(response);

  if(!response_obj.IsValid()) {
    std::cout << "Received a response but failed to parsed it\n";
    return 0;
  }

  std::cout << response_obj.Status() << " " << response_obj.StatusDescription() << "\n";

  if(!response_obj.Headers().count("Content-Type")) {
    std::cout << "Response has no 'Content-Type' header\n";
    return 0;
  }

  if(response_obj.Headers().count("Transfer-Encoding")) {
    std::cout << "'Transfer-Encoding' header not supported\n";
    return 0;
  }

  std::string content_type = response_obj.Headers().at("Content-Type");

  if(content_type.rfind("text/html", 0) != 0) {
    std::cout << "Content is not html code\n";
    return 0;
  }

  std::string content = response_obj.Content();
  if(response_obj.Headers().count("Content-Encoding") &&
     response_obj.Headers().at("Content-Encoding") == std::string("gzip")) {
    content = gzip::decompress(content.data(), content.size());
    std::cout << "\n\nDecoded response:" << content << "\n\n";
  }

  HtmlParser html_parser(content);
  html_parser.Show();

  connection.Close();

  return 0;
}