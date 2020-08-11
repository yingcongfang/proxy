#ifndef __REQUEST_H__
#define __REQUEST_H__
#include <unordered_map>
#include"Cacher.h"
#include "proxy.h"

class request{
 private:
  unordered_map<string, int> idPool;
  Cacher cache;
 public:
 request(int n):cache(Cacher(n)){};  
  void POST(string hostname,string port,proxy pxy,string command);
  void CONNECT(string hostname,string port,proxy pxy);

  void GET(string hostname,string port,proxy pxy,string command, int Uid);
  static void chooseMethod(int sfd,int csfd,int Uid,string command);

  int handler();

};


#endif
