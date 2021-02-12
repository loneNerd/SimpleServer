#include "server.h"

using namespace SimpleServer;

Server::Server( string& ip, string& port ) : m_ip{ ip }, 
                                             m_port{ port },
                                             m_addr{ nullptr } { }

Server::~Server()
{
   cout << "Shutting server...\t";
   closesocket( m_listenSocket );
   freeaddrinfo( m_addr );
   WSACleanup();
   cout << "Done" << endl;
}

bool Server::init()
{
   cout << "Inititalizing server...\n" << endl;
   
   int result = WSAStartup( MAKEWORD( 2, 2 ), &m_wsaData );

   cout << "WSA starting up...\t";

   if ( result != 0 )
   {
      cerr << "WSAStartup error" << endl;
      return false;
   }

   cout << "Done" << endl;
   

   memset( &m_hints, 0, sizeof( m_hints ) );

   m_hints.ai_family   = AF_INET;
   m_hints.ai_socktype = SOCK_STREAM;
   m_hints.ai_protocol = IPPROTO_TCP;
   m_hints.ai_flags    = AI_PASSIVE;

   cout << "Getting address info...\t";

   result = getaddrinfo( m_ip.c_str(), m_port.c_str(), &m_hints, &m_addr );

   if ( result != 0 )
   {
      cerr << "getaddrinfo error" << endl;
      return false;
   }

   cout << "Done.\n\nAddress: " << m_ip << "\nPort:\t " << m_port << endl;
   cout << "\nCreating socket...\t";

   m_listenSocket = socket( m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol );

   if ( m_listenSocket == INVALID_SOCKET )
   {
      cerr << "Socket error" << endl;
      return false;
   }

   cout << "Done\nBinding socket...\t";

   result = bind( m_listenSocket, m_addr->ai_addr, static_cast< int >( m_addr->ai_addrlen ) );

   if ( result == SOCKET_ERROR )
   {
      cerr << "Binding Faild" << endl;
      return false;
   }

   cout << "Done\nStart listen socket..." << endl;

   if ( listen( m_listenSocket, SOMAXCONN ) == SOCKET_ERROR )
   {
      cerr << "Listen socket error" << endl;
      return false;
   }

   return true;
}

thread& Server::runAsync()
{
   m_serverProcess = thread( &Server::run, this );
   
   return m_serverProcess;
}

void Server::run()
{
   int result = 0;

   while ( true )
   {
      cout << "Accepting connection...\t";

      auto clientSocket = accept( m_listenSocket, NULL, NULL );

      if ( clientSocket == INVALID_SOCKET )
      {
         cerr << "Acception failed" << endl;
         closesocket( m_listenSocket );
         continue;
      }

      thread( &Server::acceptRequest, this, clientSocket ).detach();
   }
}

void Server::acceptRequest( SOCKET clientSocket )
{
   cout << " Done\nRecieving information... ";

   const int m_bufferSize = 1024;

   char m_buffer[ m_bufferSize ]{ 0 };

   memset( m_buffer, 0, sizeof( m_buffer ) );
   int result = recv( clientSocket, m_buffer, m_bufferSize, 0 );

   if ( result == SOCKET_ERROR )
   {
      cerr << "Recieving failed: " << result << endl;
      closesocket( clientSocket );
      return;
   }
   else if ( result == 0 )
   {
      cerr << "Recieving Failed. Connection closed" << endl;
      return;
   }
   else if ( result > 0 )
   {
      vector< string > types( m_config.getAllKeys( "AddType", "text/html" ) );

      regex typePattern{ R"(Accept: \w+/\w+)" };
      smatch matches;
      string bufferString( m_buffer );
      string typeResult;
      stringstream response;
      stringstream responseBody;

      if ( regex_search( bufferString, matches, typePattern ) )
      {
         typeResult.assign( matches.str().c_str() + 8 );
      }

      if ( strncmp( m_buffer, "GET", 3 ) )
      {
         cerr << "400 Forbidden" << endl;

         responseBody << "<title>Test C++ HTTP Server</title>\n"
                      << "<h1>400 Bad Request</h1>\n";

         response << "HTTP/1.1 400 Bad Request\r\n"
                  << "Version: HTTP/1.1\r\n"
                  << "Content-Type: text/html; charset=utf-8\r\n"
                  << "Content-Length: " << responseBody.str().length()
                  << "\r\n\r\n" << responseBody.str();
      }
      else if ( std::find( types.begin(), types.end(), typeResult ) == types.end() )
      {
         cerr << "403 Forbidden" << endl;

         responseBody << "<title>Test C++ HTTP Server</title>\n"
                      << "<h1>403 Forbidden</h1>\n";

         response << "HTTP/1.1 403 Forbidden\r\n"
                  << "Version: HTTP/1.1\r\n"
                  << "Content-Type: text/html; charset=utf-8\r\n"
                  << "Content-Length: " << responseBody.str().length()
                  << "\r\n\r\n" << responseBody.str();
      }
      else
      {
         regex pathPattern{ R"(GET [/\w+|\d+]{0,}\.\w+)" };
         regex emptyPattern{ R"(GET / )" };

         string path( m_config.getValueByKey( "General", "DocumentRoot", NULL ) );
         string sitePath;

         if ( regex_search( bufferString, matches, pathPattern ) )
         {
            sitePath.assign( matches.str().c_str() + 5 );
         }
         else if ( regex_search( bufferString, matches, emptyPattern ) )
         {
            sitePath.assign( "index.html" );
         }

         ifstream documentPath( path + sitePath );

         if ( documentPath.is_open() )
         {
            cout << "200 OK" << endl;
            std::copy( std::istreambuf_iterator< char >( documentPath ),
                       std::istreambuf_iterator< char >(),
                       std::ostreambuf_iterator< char >( responseBody ) );

            response << "HTTP/1.1 200 OK\r\n"
                     << "Version: HTTP/1.1\r\n"
                     << "Content-Type: text/html; charset=utf-8\r\n"
                     << "Content-Length: " << responseBody.str().length()
                     << "\r\n\r\n"
                     << responseBody.str();
         }
         else
         {
            cerr << "404 Not Found" << endl;

            responseBody << "<title>Test C++ HTTP Server</title>\n"
                         << "<h1>404 Not Found</h1>\n";

            response << "HTTP/1.1 404 Not Found\r\n"
                     << "Version: HTTP/1.1\r\n"
                     << "Content-Type: text/html; charset=utf-8\r\n"
                     << "Content-Length: " << responseBody.str().length()
                     << "\r\n\r\n" << responseBody.str();
         }
      }

      cout << "Sending response...\t";

      result = send( clientSocket, response.str().c_str(), static_cast< int >( response.str().length() ), 0 );

      if ( result == SOCKET_ERROR )
         cerr << "Sendig response failed: " << WSAGetLastError() << endl;
      else
         cout << "Done" << endl;

      cout << "Closing socket...\t";
      closesocket( clientSocket );
      cout << "Done" << endl;
   }
}
