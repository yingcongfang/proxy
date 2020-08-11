#include "Cacher.h"

// Getters
unsigned Cacher::getCapacity(){
	return this->capacity;
}
int Cacher::getStatus(int id){
	return this->status[id];
}
time_t Cacher::getBirthTime(int id){
	return this->birth_time[id];
}
double Cacher::getMaxAge(int id){
 	return this->max_age[id];
}

// Compute the age (in seconds)
double Cacher::compute_age(int id) {
	time_t now = time(NULL); // current time
	return difftime(now, this->birth_time[id]);
}

// Expirable cache, check if older than max age
// Return true if expires
// Return false if fresh
bool Cacher::checkExpire(int id) {
	// Expirable cache, check if older than max age
	if(compute_age(id) > this->max_age[id]) {// Too old
	    return true;
	}
	else {
	    return false;
	}	  
}

void Cacher::set_birthtime(int id, time_t birthtime) {
	this->birth_time[id] = birthtime;
}

void Cacher::store(int id, vector<char> data, int CacheStat, double m_age) {	
  	switch (CacheStat) { 
    case CACHEABLE:	// Cacheable
    	this->status[id] = CACHEABLE; // Update Cache Status
    	this->cache.set(id, data);	// Store in cache
        break;
    case EXPIRABLE: // Expirable
        this->status[id] = EXPIRABLE;			// Update Cache Status
        set_birthtime(id, time(NULL));  // Set current time as birthtime
        this->max_age[id]= m_age;            	// Store max age
        this->cache.set(id, data);		    // Store in cache
        break;
     // No store by default
	}
}

// Check before retrieve
// If don't need refetch: return true
// If need refetch: return false 
bool Cacher::check(int id){
	// Cache is empty: return null
	if(!this->cache.check(id)) {
		return false;
	}
	if(this->status[id] == EXPIRABLE && checkExpire(id)) { // In cache but expired
		return false;
	}
	return true;
}

vector<char> Cacher::retrieve(int id) {
	return this->cache.get(id);	            
}

