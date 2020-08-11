#ifndef __LOGPRINTER_H__
#define __LOGPRINTER__

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <string>
#include <unordered_map>


#define LOG_FILE_NAME "/var/log/erss/proxy.log"

#define NOT_IN 0
#define EXPIRE 4
#define NEED_VALID 3
#define VALID 1

#define NOT_CACHEABLE 5




using namespace std;

class LogPrinter {
 private:
  void writeInFile(string s, string file_name);
  string rawTimeCooker(time_t rawtime);
  string idHeader(int id);
 public:
 	// Default constructed

 	/*Cacher(unsigned int cap): cache(LRUCache(cap)) {
    	this->capacity = cap;
  	};*/
  
  void newRequest(int id, time_t time, string ipStr, string requestStr);
  void checkCache(int id, int ctl_word, time_t ExpireTime = time(NULL));
  void Request(int id, string ReqLine, string ServerName);
  void Receive(int id, string ResLine, string ServerName);

  // @Overload
  void p200OK(int id, int ctl_word, time_t ExpireTime = time(NULL));
  void p200OK(int id, int ctl_word, string reason);

  void Respond(int id, string response);
  void tunnelClosed(int id);
};

#endif
