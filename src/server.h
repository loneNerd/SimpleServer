#pragma once
#ifndef SERVER_H_
#define SERVER_H_

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "config.h"

namespace SimpleServer
{
   using std::cerr;
   using std::cout;
   using std::endl;
   using std::ifstream;
   using std::regex;
   using std::smatch;
   using std::string;
   using std::stringstream;
   using std::thread;

   class Server
   {
   public:
      Server( string& ip, string& port );

      Server( const Server& other )           = delete;
      Server( Server&& other )                = delete;
      Server operator=( const Server& other ) = delete;
      Server operator=( Server&& other )      = delete;

      ~Server();

      bool init();
      thread& runAsync();
      void run();
      void acceptRequest( SOCKET clientSocket );

   private:
      struct addrinfo* m_addr;
      struct addrinfo  m_hints;
      
      WSADATA m_wsaData;
      SOCKET  m_listenSocket;
      string  m_ip;
      string  m_port;
      thread  m_serverProcess;
      Config  m_config;
   };
}

#endif // SERVER_H_