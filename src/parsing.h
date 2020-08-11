#ifndef __PARSING_H__
#define __PARSING_H__

#include <stdlib.h>

#include <sstream>
#include <string>

class parsing {
 private:
  std::string command;
  std::string hostname;
  std::string port;
  std::string method;

 public:
  parsing(){};
  parsing(char * input) {
    std::stringstream ss;
    ss.str(input);
    command = ss.str();
  };
  parsing(std::string s){
    command=s;
  };
  std::string findhost();
  std::string findPort();
  std::string findMethod();
  std::string all();
  std::string findHTML();
  int findContentL();
  std::string findHeader();
  std::string findChunk();
  std::string findFirstLine();
  int findCache();
  int findAge();
  bool find200OK();
};

#endif
