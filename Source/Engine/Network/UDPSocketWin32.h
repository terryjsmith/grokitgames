#ifdef WIN32

#ifndef win32udpsocket_h
#define win32udpsocket_h

#include <Network/UDPSocketBase.h>

class GIGA_API UDPSocketWin32 : public UDPSocketBase {
public:
    UDPSocketWin32();
    ~UDPSocketWin32();
    
public:
    /**
     * Connect to the server
     */
    bool Connect(std::string server, int port);
    
    /**
     * Send a message
     */
    int Write(unsigned char* buffer, int length);
    
    /**
     * Receive a message
     */
    int Read(void* buffer, int length);

    /**
     * Create a listening socket
     */
    void Listen(int port);
    
    /**
     * Close the connection to the server
     */
    void Close();
};

#endif

#endif