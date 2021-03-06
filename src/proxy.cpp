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
#include <unordered_map>
#include "proxy.h"
#include "parsing.h"
#include "Cacher.h"
#include "LogPrinter.h"
using namespace std;


void POST(string hostname,string port,proxy pxy,string command,LogPrinter LP,int id){
  cout<<"begin POST"<<endl;
  int ccfd=pxy.client_fd();
  if(hostname.size()==0||port.size()==0){
    cout<<"can't find host"<<endl;  
  }

  if(pxy.connectServer(hostname,port)==-1){
    cerr<<"connect server error"<<endl;
  }
  int sfd=pxy.server_fd();
  parsing pp(command);
  int len=pp.findContentL();
  int res_size;
  int index=0;
  vector<char> buffer(50000,0);
  //get client post
  if((int)command.size()<len){
#ifdef WIN32
    while((res_size=recv(ccfd, &buffer.data()[index],50000,0))>0){
      index += res_size;
      if((index+(int)command.size())>len){
	break;
      }
       
      buffer.resize(buffer.size()+res_size);
    }
#else
    while((res_size=read(ccfd, &buffer.data()[index],50000))>0){
      index += res_size;
      if((index+(int)command.size())>len){
	break;
      }
      buffer.resize(buffer.size()+res_size);      
    }
#endif

  }
  string next(buffer.begin(),buffer.end());
  command.append(next);
#ifdef WIN32
  send(sfd,command.c_str(),command.size(),0);
#else
  write(sfd,command.c_str(),command.size());
#endif
  vector<char> resS(50000,0);
  int indexS=0;
  int resS_size;
#ifdef WIN32
  resS_size=recv(sfd, &resS.data()[indexS],50000,0);
#else
  resS_size=read(sfd, &resS.data()[indexS],50000);
#endif
  indexS+=resS_size;
  //parsing
  string re(resS.begin(),resS.end());
  parsing pap(re);
  int reL=pap.findContentL();
  string firstLine=pap.findFirstLine();
  LP.Receive(id,firstLine , hostname);
  int count=0;
  //receive response from server
#ifdef WIN32
    while((resS_size=recv(sfd, &resS.data()[indexS],50000,0))>0){
      count+=resS_size;
      indexS += resS_size;
      if(count>=reL){
	break;
      }
             
      resS.resize(resS.size()+resS_size);
    }
#else
    while((resS_size=read(sfd, &resS.data()[indexS],50000))>0){
      count+=resS_size;
      indexS += resS_size;
      if(count>=reL){
	break;
      }
      
      resS.resize(resS.size()+resS_size);
      
    }
#endif
  
#ifdef WIN32
  int n=send(ccfd,resS.data(),indexS,0);
#else
  int n=write(ccfd,resS.data(),indexS);
#endif
  cout<<"send back: "<<n<<endl;
  pxy.freeServer();
}


void CONNECT(string hostname,string port,proxy pxy,LogPrinter LP,int id){
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

    LP.Respond(id, "HTTP/1.1 200 OK");
  
    send(ccfd, ack, strlen(ack), 0);
    
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
	cout<<"error\n";
	exit(-1);
	break;
      case 0:
	cout<<"didn't get anything\n";
	break;
      default:
	//listen from client
	if(FD_ISSET(ccfd,&fds)){
	  char buffer[50000];
	  int recvn=0;
	  memset(buffer, 0, 50000);
#ifdef WIN32
	  recvn=recv(ccfd, buffer, 50000,0);
#else
	  recvn=read(ccfd, buffer, 50000);
#endif
	  
#ifdef WIN32
	  send(sfd, buffer, recvn,0);
#else
	  write(sfd, buffer, recvn);
#endif
	  
	  if(strlen(buffer)==0){
	    cout<<"receive empty from client"<<endl;
	    return;
	  }
	}
	//listen from server
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
	  
#ifdef WIN32
	  send(ccfd, buffer, recvn,0);
#else
	  write(ccfd, buffer, recvn);
#endif
	  
	}
	break;
      }
    }
  }
  pxy.freeServer();  
}



void GET(string hostname,string port,proxy pxy,string command,Cacher *cache,int Uid,LogPrinter LP,int id){
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
  //header
  string headS(buffer.begin(),buffer.end());
  
  parsing pas(headS);
  int len=pas.findContentL();
  int caornot=pas.findCache();
  int age=pas.findAge();
  time_t currTime = time(NULL);
  string firstLine=pas.findFirstLine();
  LP.Receive(id, firstLine, hostname);
  if(pas.find200OK()){
    if(caornot==3){
      LP.p200OK(id, 3);
    }
    if(caornot==0){
      LP.p200OK(id, 5, "Can't find cache control");
    }
    if(caornot==2){
      LP.p200OK(id, 5, "Cache Control: no-store");
    }
    if(caornot==4){
      LP.p200OK(id, 4, currTime);
    }
  }
  buffer.resize(buffer.size()+recvn);
  //normal
  if(len>=0){
    int res_size;
    index+=recvn;
    int count=0;
#ifdef WIN32
    while((res_size=recv(sfd, &buffer.data()[index],50000,0))>0){
      index += res_size;
      count+=res_size;
      if(recvn>len||count==len||index>=len){
	break;
      }
       
      buffer.resize(buffer.size()+res_size);

    }
#else
    while((res_size=read(sfd, &buffer.data()[index],50000))>0){
      index += res_size;
      count+=res_size;
      if(recvn>len||count==len||index>=len){
	break;
      }
      buffer.resize(buffer.size()+res_size);
       
    }
#endif

    //cache here
    if(caornot!=0){
      cout<<Uid<<"cache is storing..."<<endl;
      string saveinCache(buffer.begin(),buffer.end());
      pthread_mutex_t lock;
      pthread_mutex_lock(&lock);
      cache->store(Uid,buffer,caornot,age);
      pthread_mutex_unlock(&lock);
    }
#ifdef WIN32
    send(ccfd,buffer.data(),index,0);
#else
    write(ccfd,buffer.data(),index);
#endif
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
  pxy.freeServer();
}




void chooseMethod(int sfd,int csfd,int Uid,string command,Cacher* cache,LogPrinter LP,int id){
  cout<<"start to choose!"<<endl;
  proxy pxy;
  pxy.setSFD(sfd,csfd);
   
  parsing pp(command);
  string hostname=pp.findhost();
  string method=pp.findMethod();
  LP.Request(id, method, hostname);
  
  if(method.compare("CONNECT")==0){
    CONNECT(hostname,"443",pxy,LP,id);
    LP.tunnelClosed(id);
  }
  if(method.compare("GET")==0){
    GET(hostname,"80",pxy,pp.all(),cache,Uid,LP,id);
  }
  if(method.compare("POST")==0){
    POST(hostname,"80",pxy,pp.all(),LP,id);
  }
  
}


int main(int argc, char *argv[])
{  
  int id=100;
  int status;
  int cc_fd;
  struct addrinfo client_info;
  struct addrinfo *client_info_list;
  
  const char *hostname = NULL;
  const char *port     = "12345";
  Cacher *cache=new Cacher(100);
  unordered_map<string, int> idPool;
  LogPrinter LP;
  time_t currTime = time(NULL);
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
    
    string command(buffer);
    if(command.size()==0){
      continue;
    }
    //parsing
    parsing pp(buffer);
    string req=pp.findFirstLine();
    string met=pp.findMethod();
    //get unique cache id for commands
    if(idPool.find(req)==idPool.end()){
      idPool[req] = idPool.size();
    }
    int UniId=idPool[req];
    cout <<id <<"received from client: " << buffer << endl;

    LP.newRequest(id,currTime,"client",met);
    //check if file is in cache or not
    if(cache->check(UniId)){
      LP.checkCache(id, 1);
      cout<<"get from cache..."<<endl;
      vector<char> input=cache->retrieve(UniId);
#ifdef WIN32
      send(cc_fd,input.data(),input.size(),0);
#else
      write(cc_fd,input.data(),input.size());
#endif
      
    }
    else{
      if(cache->getStatus(UniId)==4){
	LP.checkCache(id, 4,cache->getBirthTime(UniId)+cache->getMaxAge(UniId));
      }
      else{
	LP.checkCache(id,0);
      }
      thread trd(chooseMethod,cc_fd,cs_fd,UniId,command,cache,LP,id);
      trd.detach();
    }
    id++;
  }
  freeaddrinfo(client_info_list);
  close(cs_fd);
  close(cc_fd);
}
