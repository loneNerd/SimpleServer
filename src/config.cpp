#include "config.h"

using namespace SimpleServer;

Config::Config()
{
   GetFullPathNameA( "..\\etc\\configs\\config.ini", 256, m_configFilePath, NULL );
}

string Config::getValueByKey( const char* section, const char* key, const char* def )
{
   memset( m_buffer, 0, m_bufferSize );
   GetPrivateProfileStringA( section, key, def, m_buffer, m_bufferSize, m_configFilePath );
   return m_buffer;
}

vector<string> Config::getAllKeys( const char* section, const char* def )
{
   vector< string > types;

   memset( m_buffer, 0, m_bufferSize );
   GetPrivateProfileStringA( section, NULL, def, m_buffer, m_bufferSize, m_configFilePath );

   string temp;

   for ( auto i = 0; i < m_bufferSize; ++i )
   {
      if ( m_buffer[ i ] == '\0' && !temp.empty() )
      {
         types.emplace_back( temp );
         temp.clear();
         temp.resize( 0 );
      }
      else if ( m_buffer[ i ] != '\0' )
         temp.push_back( m_buffer[ i ] );
   }

   return types;
}
