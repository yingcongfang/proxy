#include "parsing.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <stdlib.h>


bool parsing::find200OK(){
  if(command.find("200 OK")!=std::string::npos){
    return true;
  }
  return false;
}

int parsing::findCache(){
  int n=command.find("Cache-Control");
  if(command.find("Cache-Control")==std::string::npos){
    return 2;
  }
  else{
    int nn=command.substr(n).find("\r\n");
    std::string cache=command.substr(n,nn);
    if(cache.find("no-store")!=std::string::npos){
      return 2;
    }
    if(cache.find("no-cache")!=std::string::npos){
      return 3;
    }
    if(cache.find("public")!=std::string::npos){
      return 1;
    }
    if(cache.find("max-age")!=std::string::npos||cache.find("maxage")!=std::string::npos){
      return 4;
    }
  }
  return 0;
}

int parsing::findAge(){
  if(findCache()==4){
    int n=command.find("Cache-Control");
    int nn=command.substr(n).find("\r\n");
    std::string cache=command.substr(n,nn);
    int numN=cache.find('=');
    int numNN=cache.find('\r');
    std::string number=cache.substr(numN+1,numNN-numN);
    int final=std::stoi(number);
    return final;
  }
  return 0;
}


std::string parsing::findFirstLine(){
  int n=command.find("\r\n");
  return command.substr(0,n);
}

std::string parsing::findHeader(){
  int bre=command.find("\r\n\r\n");
  return command.substr(0,bre+4);
}
std::string parsing::findChunk(){
  int bre=command.find("\r\n\r\n");
  return command.substr(bre+4);
}

int parsing::findContentL(){
  if(command.find("Content-Length")!=std::string::npos){
    int start=command.find("Content-Length");
    std::string sub=command.substr(start);
    int space=sub.find(' ');
    int end=sub.find('\r');
    std::string contsize=sub.substr(space+1,end-space);
    int ans=std::stoi(contsize);
    return ans;
  }
  if(command.find("Transfer-Encoding: chunked")!=std::string::npos){
    return -1;
  }
  return -2;
}



std::string parsing::findhost() {
  if(command.size()==0||command.find("Host:")==std::string::npos){
    return "";
  }
  int firstSpace = command.find("Host:");
  //std::cout<<command.substr(firstSpace)<<std::endl;
  int firstColon = command.substr(firstSpace).find(' ');
  //std::cout<<command.substr(firstSpace+firstColon)<<std::endl;
  int enter=command.substr(firstColon+firstSpace+1).find('\r');
  int colon=command.substr(firstColon+firstSpace+1).find(':');
  hostname = command.substr(firstSpace + firstColon+1, std::min(enter,colon));
  return hostname;
}

std::string parsing::findPort() {
  if(command.size()==0){
    return "";
  }
  int firstColon = command.find(':');
  int portSpace = command.substr(firstColon).find(' ');
  port = command.substr(firstColon + 1, portSpace - 1);
  return port;
}

std::string parsing::findMethod(){
  int firstSpace = command.find(' ');
  method = command.substr(0,firstSpace);
  return method;
}

std::string parsing::all(){
  return command;
}

std::string parsing::findHTML(){
  if(command.find("<HTML>")!=std::string::npos){
    int n=command.find("<HTML>");
    return command.substr(n);
  }
  else{
    int n=command.find("\r\n\r\n");
    return command.substr(n+4);
  }
}

