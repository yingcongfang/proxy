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
#include "proxy.h"
#include "parsing.h"
#include "Cacher.h"
#include "request.h"
void request::POST(string hostname,string port,proxy pxy,string command){
  cout<<"begin POST"<<endl;
  int ccfd=pxy.client_fd();
  if(hostname.size()==0||port.size()==0){
    cout<<"can't find host"<<endl;  
  }

  if(pxy.connectServer(hostname,port)==-1){
    cerr<<"connect server error"<<endl;
  }
  int sfd=pxy.server_fd();
  
#ifdef WIN32
  send(sfd,command.c_str(),command.size(),0);
#else
  write(sfd,command.c_str(),command.size());
#endif
  cout<<"start to recv server"<<endl;
  vector<char> resS(50000,0);
  int indexS=0;
#ifdef WIN32
  int nnn=recv(sfd, &resS.data()[indexS],50000,0);
   
#else
  int nnn=read(sfd, &resS.data()[indexS],50000);
#endif
  indexS+=nnn;
  //string sss(resS.begin(),resS.end());
  //cout<<"POST receive from server: "<<sss<<endl;
#ifdef WIN32
  int n=send(ccfd,resS.data(),indexS,0);
#else
  int n=write(ccfd,resS.data(),indexS);
#endif
  cout<<"send back: "<<n<<endl;
}

void request::CONNECT(string hostname,string port,proxy pxy){
  int ccfd=pxy.client_fd();
  while(1) {
    if(hostname.size()==0||port.size()==0){
      cout<<"can't find host"<<endl;
      break;
    }
    //build server socket
    if(pxy.connectServer(hostname,port)==-1){
      cerr<<"connect server error"<<endl;
    }
    int sfd=pxy.server_fd();     
    //send 200ok  
    const char* ack="HTTP/1.1 200 OK\r\n\r\n";
    send(ccfd, ack, strlen(ack), 0);
    cout<<"200ok!"<<endl;
    //cout<<"send size:"<<size<<endl;
    while(1){
      int result;
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(sfd,&fds);
      FD_SET(ccfd,&fds);
      
      int maxfdp=std::max(sfd+1,ccfd+1);
      result = select(maxfdp,&fds,NULL,NULL,NULL);
      switch(result){
      case -1:
	cout<<"connect -1\n";
	return;
      case 0:
	cout<<"didn't get anything\n";
	break;
      default:
	if(FD_ISSET(ccfd,&fds)){
	  char buffer[50000];
	  int recvn=0;
	  memset(buffer, 0, 50000);
#ifdef WIN32
	  recvn=recv(ccfd, buffer, 50000,0);
#else
	  recvn=read(ccfd, buffer, 50000);
#endif
	  //cout << "select received from client: " << buffer << endl;
#ifdef WIN32
	  send(sfd, buffer, recvn,0);
#else
	  write(sfd, buffer, recvn);
#endif
	  //cout<<"client send size:"<<n<<endl;
	  
	  if(strlen(buffer)==0){
	    cout<<"receive empty from client"<<endl;
	    return;
	  }
	}
	if(FD_ISSET(sfd,&fds)){
	  char buffer[50000];
	  memset(buffer, 0, 50000);
	  int recvn=0;
#ifdef WIN32
	  recvn=recv(sfd, buffer+0, 50000,0);
#else
	  recvn=read(sfd, buffer+0, 50000);
#endif
	  if(strlen(buffer)==0){
	    cout<<"receive empty from server"<<endl;
	    return;
	  }
	  //cout << "select received from server: " << buffer << endl;
#ifdef WIN32
	  send(ccfd, buffer, recvn,0);
#else
	  write(ccfd, buffer, recvn);
#endif
	  //cout<<"server send size:"<<n<<endl;
	}
	break;
      }
    }
  }
}

void request::GET(string hostname,string port,proxy pxy,string command,int id){
  int ccfd=pxy.client_fd();
  if(hostname.size()==0||port.size()==0){
    cout<<"can't find host"<<endl;  
  }

  if(pxy.connectServer(hostname,port)==-1){
    cerr<<"connect server error"<<endl;
  }
  int sfd=pxy.server_fd();
  //send GET command to server
#ifdef WIN32
  send(sfd, command.c_str(),command.size(),0);
#else
  write(sfd,command.c_str(),command.size());
#endif
  //receive header from server
  vector<char> buffer(50000,0);
  int index=0;
#ifdef WIN32
  int recvn=recv(sfd, &buffer.data()[index], 50000,0);
#else
  int recvn=read(sfd, &buffer.data()[index], 50000);
#endif
  string headS(buffer.begin(),buffer.end());
  parsing pas(headS);
  int len=pas.findContentL();
  cout<<"the length is: "<<len<<endl;
  cout<<"now recvn: "<<recvn<<endl;
  //cout<<"header is : "<<headS<<endl;
  buffer.resize(buffer.size()+recvn);
  //normal
  if(len>=0){
    int res_size;
    index+=recvn;
    int count=0;
#ifdef WIN32
    while((res_size=recv(sfd, &buffer.data()[index],50000,0))>0){
      cout<<"GET res size: "<<res_size<<endl;
      index += res_size;
      count+=res_size;
      if(count==len||index>=len){
	cout<<"count break!"<<endl;
	break;
      }
      cout<<"GET index: "<<index<<endl;
       
      buffer.resize(buffer.size()+res_size);

      cout<<"GET size: "<<buffer.size()<<endl;
    }
#else
    while((res_size=read(sfd, &buffer.data()[index],50000))>0){
      cout<<"GET res size: "<<res_size<<endl;
      index += res_size;
      cout<<"GET index: "<<index<<endl;
      count+=res_size;
      if(count==len||index>=len){
	cout<<"count break!"<<endl;
	break;
      }
      buffer.resize(buffer.size()+res_size);
       
      cout<<"GET size: "<<buffer.size()<<endl;
    }
#endif
    cout<<"final recv size: "<<count<<endl;
    
#ifdef WIN32
    send(ccfd,buffer.data(),index,0);
#else
    write(ccfd,buffer.data(),index);
#endif
    //string final(buffer.begin(),buffer.end());
    //cout<<"send to POST client: "<<final<<endl;
  }
  
  //chunk
  else if(len==-1){
    char buff[50000];
    int sss;
#ifdef WIN32
    send(ccfd,buffer.data(),recvn,0);
    while((sss=recv(sfd, buff, 50000,0))>0){
      send(ccfd,buff,sss,0);
      memset(buff, '\0', 50000);
    }
#else
    write(ccfd,buffer.data(),recvn);
    while((sss=read(sfd, buff, 50000))>0){
      write(ccfd,buff,sss);
      memset(buff, '\0', 50000);
    }
#endif
  }
  else{
    cout<<"can't get any thing!"<<endl;
  }
}

static void request::chooseMethod(int sfd,int csfd,int Uid,string command){
  proxy pxy;
  pxy.setSFD(sfd,csfd);
  parsing pp(command);
  string hostname=pp.findhost();
  string method=pp.findMethod();
    
  
  if(method.compare("CONNECT")==0){
    CONNECT(hostname,"443",pxy,command);
  }
  if(method.compare("GET")==0){
    GET(hostname,"80",pxy,pp.all(),command,Uid);
  }
  if(method.compare("POST")==0){
    POST(hostname,"80",pxy,pp.all());
  }
  pxy.freeServer();
}

int request::handler(){
  int status;
  int cc_fd;
  struct addrinfo client_info;
  struct addrinfo *client_info_list;
  
  const char *hostname = NULL;
  const char *port     = "12345";
  int id=100;
  
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
  
  const int cs_fd = socket(client_info_list->ai_family, 
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
  
  status = listen(cs_fd, 500);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
    } //if
  while(1){
    cout << "Waiting for connection on port " << port << endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    cc_fd = accept(cs_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (cc_fd == -1) {
      perror("accept error:");
      cerr << "Error: cannot accept connection on socket" << endl;
      return -1;
    } //if
    
    cout<<"Client connect success! Start to make thread "<<id<<endl;
    //receive command from client
    char buffer[12048];
    memset(buffer, '\0', 12048);
    recv(cc_fd, buffer, 12048,0);
    cout << "received from client: " << buffer << endl;
    string command(buffer);
    //parsing
    parsing pp(buffer);
    string req=pp.findFirstLine();
    string met=pp.findMethod();

    //get unique cache id for commands
    if(idPool.find(req)==idPool.end()){
      idPool[req] = idPool.size();
    }
    int UniId=idPool[req];
    if(cache.check(UniId)&&met.compare("GET")==0){
      string resFromCache=cache.retrieve(UniId);
      
    }
    try{
      thread trd(chooseMethod,cc_fd,cs_fd,UniId,command);
      id++;
      trd.detach();
    }
    catch(exception &e){
      cout << e.what() << '\n';
    }
  }
  freeaddrinfo(client_info_list);
  close(cs_fd);
  close(cc_fd);
}

