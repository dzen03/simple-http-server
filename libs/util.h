// start of platform dependant functions
#if defined __has_include
  #if __has_include(<winsock2.h>)
    #define WINDOWS
  #elif __has_include(<unistd.h>)
    #define POSIX
  #endif
#else
  #if defined (_WIN64)
    #define WINDOWS
  #elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    #define POSIX
  #endif
#endif
