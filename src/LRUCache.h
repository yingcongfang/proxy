#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <iostream>

#include <cstring>
#include <cstdlib>

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

using namespace std;

class LRUCache {
private:
    unsigned int capacity;
    list<int> recent;
    unordered_map<int, vector<char>> cache;
    unordered_map<int, list<int>::iterator> pos;
    void use(int key) {
        if (pos.find(key) != pos.end()) {				// If in recent list, delete the old position and add to front at line 16
        												// Avoid repetition
            recent.erase(pos[key]);
        } else if (recent.size() >= capacity) {			// If cache is full
            int old = recent.back();
            recent.pop_back();
            cache.erase(old);
            pos.erase(old);
        }
        recent.push_front(key);
        pos[key] = recent.begin();
    }
public:
    LRUCache(unsigned int capacity): capacity(capacity) {}
    
    // If not in cache, return false
    bool check(int key) {
        if(cache.find(key) == cache.end()) {
            return false;
        }
        return true;
    }
    
    vector<char> get(int key) {
        use(key);
        return cache[key];						   								
    }

    void set(int key, vector<char> value) {
        use(key);
        cache[key] = value;
    }
};

#endif