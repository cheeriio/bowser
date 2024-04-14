#include <html_parser.hpp>
#include <log.hpp>

#include <iostream>

HtmlParser::HtmlParser(std::string code) : code_(code) {}

// Magnificient
void HtmlParser::Show() {
  size_t pos_start = code_.find("<body>"),
         pos_end = code_.find("</body>");

  if(pos_start == std::string::npos || pos_end == std::string::npos) {
    Log(LogLevel::kInfo, "Could not parse the html code");
    return;
  }

  std::string body = code_.substr(pos_start + 6, pos_end);
  bool in_tag = false;

  for(int i = 0; i < body.size(); i++) {
    if(body.at(i) == '<')
      in_tag = true;
    else if(body.at(i) == '>')
      in_tag = false;
    else if(!in_tag)
      std::cout << body.at(i);
  }
}

bool HtmlParser::IsValid() {
  return is_valid_;
}
