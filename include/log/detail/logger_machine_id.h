
#ifndef LOGGER_MACHINE_ID_HEADER
#define LOGGER_MACHINE_ID_HEADER

#include <log/logger_config.h>
#include <log/logger_pdetect.h>
#include <log/logger_pdefs.h>

#if defined(LOG_PLATFORM_WINDOWS)
  #if defined(WIN32_LEAN_AND_MEAN)
    #define  LOG_MACHINE_ID_WIN32_LEAN_AND_MEAN_DEFINED
  #endif /*WIN32_LEAN_AND_MEAN*/

  #define WIN32_LEAN_AND_MEAN        
  #include <windows.h>      

  // move definition 'WIN32_LEAN_AND_MEAN' to previous state
  #ifndef LOG_MACHINE_ID_WIN32_LEAN_AND_MEAN_DEFINED
    #undef WIN32_LEAN_AND_MEAN
  #else /*LOG_MACHINE_ID_WIN32_LEAN_AND_MEAN_DEFINED*/
    #undef LOG_MACHINE_ID_WIN32_LEAN_AND_MEAN_DEFINED
  #endif /*LOG_MACHINE_ID_WIN32_LEAN_AND_MEAN_DEFINED*/
#elif defined(LOG_PLATFORM_POSIX_BASED) || defined(LOG_PLATFORM_MAC)
  #include <stdio.h>
  #include <string.h>
  #include <unistd.h>          
  #include <errno.h>           
  #include <sys/types.h>       
  #include <sys/socket.h>      
  #include <sys/ioctl.h>  
  #include <sys/resource.h>    
  #include <sys/utsname.h>       
  #include <netdb.h>           
  #include <netinet/in.h>      
  #include <netinet/in_systm.h>                 
  #include <netinet/ip.h>      
  #include <netinet/ip_icmp.h> 
  #include <assert.h>

#ifdef DARWIN                    
  #include <net/if_dl.h>       
  #include <ifaddrs.h>         
  #include <net/if_types.h>    
#else //!DARWIN              
// #include <linux/if.h>        
// #include <linux/sockios.h>   
#endif //!DARWIN               

#endif

namespace logging {
namespace detail {

#ifdef LOG_PLATFORM_WINDOWS
namespace win {

uint32_t getVolumeHash()       
{        
   DWORD volume_serial_num = 0;  
   char buffer[1024];
   buffer[0] = 0;

   GetWindowsDirectoryA(buffer, sizeof(buffer));
   
   int i = 0;
   while (buffer[i] != 0 && buffer[i] != '\\') ++i;
   buffer[i + 1] = 0;
     
   // Determine if this volume uses an NTFS file system.      
   GetVolumeInformationA(buffer, NULL, 0, &volume_serial_num, NULL, NULL, NULL, 0 );    
   return volume_serial_num;
}        

void getMachineName(char* buffer, size_t buf_size) {           
  DWORD size = static_cast<DWORD>(buf_size);
  GetComputerNameA(buffer, &size );              
}
}//namespace win
#endif /*LOG_PLATFORM_WINDOWS*/


#if defined(LOG_PLATFORM_POSIX_BASED) || defined(LOG_PLATFORM_MAC) || defined(LOG_PLATFORM_ANDROID)
namespace posix {
void getMachineName(char* buffer, size_t buf_size) { 
  struct utsname u;  

  if ( uname( &u ) < 0 ) {       
    strncpy(buffer, "unknown", buf_size);
    return;
  }       

  strncpy(buffer, u.nodename, buf_size);
}   

unsigned short getVolumeHash()          
{ 
   // we don't have a 'volume serial number' like on windows. Lets hash the system name instead.    
  char buf[256];

  getMachineName(buf, sizeof(buf)-1);
   unsigned short hash = 0;             

   for ( unsigned int i = 0; buf[i]; i++ )
      hash += ( static_cast<unsigned char>(buf[i]) << (( i & 1 ) * 8 ));

   return hash;              
} 

} //namespace posix
#endif /*LOG_PLATFORM_POSIX_BASED || LOG_PLATFORM_MAC*/


uint32_t get_machine_id() {
#ifdef LOG_PLATFORM_WINDOWS
  using namespace logging::detail::win;
#elif defined(LOG_PLATFORM_POSIX_BASED) || defined(LOG_PLATFORM_MAC)
  using namespace logging::detail::posix;
#else
  return 0;
#endif /*LOG_PLATFORM_POSIX_BASED || LOG_PLATFORM_MAC*/

#if defined(LOG_PLATFORM_WINDOWS) || defined(LOG_PLATFORM_POSIX_BASED) || defined(LOG_PLATFORM_MAC)
  char buffer[1024];
  buffer[0] = 0;

  getMachineName(buffer, sizeof(buffer));

  uint32_t machine_name_hash = 0;
  for(int i=0; i<strlen(buffer); i++) {
    machine_name_hash += (buffer[i] << (i % 24));
  }

  return getVolumeHash() ^ machine_name_hash;
#endif /*LOG_PLATFORM_WINDOWS || LOG_PLATFORM_POSIX_BASED || LOG_PLATFORM_MAC*/
}

}//namespace detail
}//namespace logging


#endif /*LOGGER_MACHINE_ID_HEADER*/
