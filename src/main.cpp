#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <thread>

#include "config.h"
#include "server.h"

// const int g_maxClientBufferSize = 1024;

using std::endl;
using std::string;
using std::smatch;
using std::regex;
using std::ifstream;
using std::vector;
using std::cout;
using std::cerr;

using std::thread;

using SimpleServer::Config;
using SimpleServer::Server;

int main()
{
   Config config;

   string ipAddres( config.getValueByKey( "General", "Listen", "0.0.0.0:0" ) );

   auto index = ipAddres.find_first_of(':');

   string ip( ipAddres.substr( 0, index ) );
   string port( ipAddres.substr( index + 1 ) );

   Server server( ip, port );

   if ( server.init() )
   {
      server.runAsync().join();
   }
   system( "pause" );
   return 0;
}
