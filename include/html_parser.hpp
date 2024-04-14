#ifndef _BOWSER_HTML_PARSER_HPP_
#define _BOWSER_HTML_PARSER_HPP_

#include <string>

class HtmlParser {
 public: 
  HtmlParser() = delete;
  HtmlParser(std::string code);

  void Show();
  bool IsValid();
 private:
  std::string code_;
  bool is_valid_ = true;
};

#endif // _BOWSER_HTML_PARSER_HPP_