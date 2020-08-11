#include <iostream>

#include <cstdlib>
#include <string>

#include "LogPrinter.h"

#include <cassert> 

using namespace std;

void test_newRequest() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	time_t currTime = time(NULL);
	string ip("255.255.255.255");
	string request("Hello world!");
	
	testLP.newRequest(id,currTime,ip,request);
	
}

void test_checkCache() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	time_t currTime = time(NULL);
	
	testLP.checkCache(id, NOT_IN);
	testLP.checkCache(id, EXPIRE, currTime);
	testLP.checkCache(id, NEED_VALID);
	testLP.checkCache(id, VALID);
	
}

void test_Request() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	string Req("test request");
	string SerName("Alibaba");

	testLP.Request(id, Req, SerName);
	
}

void test_Receive() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	string Res("test response");
	string SerName("Alibaba");

	testLP.Receive(id, Res, SerName);
	
}

void test_p200OK() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	time_t currTime = time(NULL);
	string res = "of love"; // reason
	
	testLP.p200OK(id, NOT_CACHEABLE, res);
	testLP.p200OK(id, NEED_VALID);
	testLP.p200OK(id, EXPIRE, currTime);
	
}

void test_Respond() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	string Res("test response");

	testLP.Respond(id, Res);
	
}

void test_tunnelClosed() {
	LogPrinter testLP; // Default constructor
	int id = 1234;

	testLP.tunnelClosed(id);
	
}
/*void test_writeInFile() {
	LogPrinter testLP; // Default constructor
	int id = 1234;
	time_t currTime = time(NULL);
	string ip("255.255.255.255");
	string request("Hello world!");
	
	string reqStr = testLP.newRequest(id,currTime,ip,request);

	testLP.writeInFile(reqStr, "test.txt");
	testLP.writeInFile(reqStr, "test.txt"); // Write twice
	
}*/


int main(){
	remove(LOG_FILE_NAME); // Remove the old log
	test_newRequest();
	test_checkCache();
	test_Request();
	test_Receive();
	test_p200OK();
	test_Respond();
	test_tunnelClosed();
	return 0;
}
