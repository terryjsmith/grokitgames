
#ifndef udpsocket_h
#define udpsocket_h

#ifdef WIN32
#include <Network/UDPSocketWin32.h>
typedef UDPSocketWin32 UDPSocket;
#elif __APPLE__
#include <Network/UDPSocketPosix.h>
typedef UDPSocketPosix UDPSocket;
#elif __linux__
#include <Network/UDPSocketPosix.h>
typedef UDPSocketPosix UDPSocket;
#endif

#endif
