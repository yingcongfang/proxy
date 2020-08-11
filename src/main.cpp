#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <exception>
#include <unordered_map>
#include "request.h"
using namespace std;


int main(){
  request flow(100);
  flow.handler();
  return EXIT_SUCCESS;
}
