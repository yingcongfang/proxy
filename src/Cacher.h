#ifndef __CACHER_H__
#define __CACHER_H__

#include <iostream>

#include <cstdlib>
#include <ctime>

#include <string>
#include <unordered_map>

#include "LRUCache.h"

// TODO: deal with private and public cache
#define CACHEABLE 1
#define NO_STORE 2 //No caching
#define NO_CACHE 3 //Cache but revalidate
#define EXPIRABLE 4

using namespace std;

class Cacher {
 private:
  unsigned int capacity;
  LRUCache  cache; // Use unique id as key
  unordered_map<int, int> status; // cachable status, can be update.
  unordered_map<int, time_t> birth_time; // id -> birthtime
  unordered_map<int, double> max_age; // Max age, in seconds
  
  double  compute_age(int id);
  bool  checkExpire(int id);
  void 	  set_birthtime(int id, time_t birthtime);
 public:
 	Cacher(unsigned int cap): cache(LRUCache(cap)) {
    	this->capacity = cap;
  	};
  
  unsigned getCapacity();
  int getStatus(int id);
  time_t getBirthTime(int id);
  double getMaxAge(int id);
  void   store(int id, vector<char> data, int CacheStat = CACHEABLE, double m_age = 0);
  bool check(int id);
  vector<char> retrieve(int id);

};

#endif