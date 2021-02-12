#pragma once
#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>
#include <windows.h>

namespace SimpleServer
{
   using std::string;
   using std::vector;

   class Config
   {
   public:
      Config();

      Config( const Config& other )            = delete;
      Config( Config&& other )                 = delete;
      Config& operator=( const Config& other ) = delete;
      Config& operator=( Config&& other )      = delete;

      ~Config() {}

      string getValueByKey( const char* section, const char* key, const char* def );
      vector< string > getAllKeys( const char* section, const char* def );

   private:
      const static int m_bufferSize = 256;
      char m_configFilePath[ 256 ]{ 0 };
      char m_buffer[ m_bufferSize ]{ 0 };
   };
}

#endif // CONFIG_H_
