#ifndef __PROXY_H__
#define __PROXY_H__

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "Cacher.h"
using namespace std;

class proxy{
 private:
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  int cs_fd;
  int cc_fd;
  struct addrinfo client_info;
  struct addrinfo *client_info_list;
  
 public:
  proxy(){
  };
  int client_fd(){
    return cc_fd;
  };
  int server_fd(){
    return socket_fd;
  };
  void setSFD(int sfd,int cssfd){
    cc_fd=sfd;
    cs_fd=cssfd;
  };
  int getCS_fd(){
    return cs_fd;
  };
  
  int connectServer(string hn,string pt){
  int status;
  const char *hostname = hn.c_str();
  const char *port     = pt.c_str();
  
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  return 0;
  };


  void freeServer(){
    freeaddrinfo(host_info_list);
    close(socket_fd);
  };

  int connectClient(){
  int status;
  const char *hostname = NULL;
  const char *port     = "12345";

  memset(&client_info, 0, sizeof(client_info));

  client_info.ai_family   = AF_UNSPEC;
  client_info.ai_socktype = SOCK_STREAM;
  client_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &client_info, &client_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  cs_fd = socket(client_info_list->ai_family, 
		     client_info_list->ai_socktype, 
		     client_info_list->ai_protocol);
  if (cs_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  int yes = 1;
  status = setsockopt(cs_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(cs_fd, client_info_list->ai_addr, client_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  status = listen(cs_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  cout << "Waiting for connection on port " << port << endl;
  //struct sockaddr_storage socket_addr;
  //socklen_t socket_addr_len = sizeof(socket_addr);
  
  //cc_fd = accept(cs_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  // while(cc_fd==-1){
  //  cc_fd = accept(cs_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  //}
  //if (cc_fd == -1) {
  //cerr << "Error: cannot accept connection on socket" << endl;
  //return -1;
  //} //if
  
  
  return 0;
};

  void freeClient(){
    freeaddrinfo(client_info_list);
    close(cs_fd);
    close(cc_fd);
  };

};


#endif
