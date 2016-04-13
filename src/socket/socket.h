#ifndef SocketH
#define SocketH

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <unistd.h>
#include <string>
#include <vector>

/// The socket class serves as a common foundation for both client and server connections.
/// see man socket for details.
class Socket
{
public:
    Socket();
    virtual ~Socket();

    // Server initialization start 
    
    ///@b create
    ///Creates the socket connection
    ///@return true if socket creation was a succes, false if not
    ///throws SocketException
    const bool create();
    
    ///@b bind
    ///This function binds the socket to the specified port
    ///@param port_no integer containing the port number to bind the socket to
    ///@return true if teht socket was bound to the requested port number, false if not
    ///throws SocketException
    const bool bind(const int port_no);
    
    ///@b listen
    ///This function sets the socket connection in listening mode
    ///@param port_no integer containing the port number to listen on
    ///@return true if the socket can start listening, false if not
    ///throws SocketException
    const bool listen(const int port_no) const;
    
    ///@b accept
    ///This function must be called when the server should accept a connection.
    ///@param sckt Socket pointer of the incoming socket
    ///@return true if the incoming connection was accepted, false if not
    const bool accept(Socket *sckt) const;

    // Server initialization ends ----

    // Client initialization start ---
    
    ///@b connect
    ///This function connects a client socket to a host
    ///@param host_name std::string containing the host name in ip format xxx.xxx.xxx.xxx
    ///@param port integer containing the host's port number
    ///@return bool true if the connection was established, false if not
    const bool connect(const std::string host_name, const int port);
    // Client initialization ends ---

    // Data Transimission start ---
    
    ///@b send
    ///This function sends a string of data over the socket
    ///@param msg std::string to send
    ///@return bool true if transmission was successful, false if not
    const bool send(const std::string msg) const;

    ///@b receive
    ///This function receives data from the socket.
    ///Notice passed string for data is overwritten by the receive call and, it is filled with incoming data
    ///@param msg std::string to put the received data in.
    ///@return int the number of bytes transmitted (this should be equal to the string length + NULL termination!)
    const int recv(std::string& msg) const;

    const int recv_buffer(std::vector<char> &rawdata) const;

    // Data Transimission end ---  

    // accessor functions
   
    ///@b set_non_blocking
    ///This function toggles the socket blooking connection bit
    ///@param block boolean true to set the socket in nonblocking mode, false to set the socket blocking
    void set_non_blocking(const bool block);

    const bool is_valid() const; ///< This funtion returns true if the socket is valid
    const char *get_addr() const; ///< This function returns the peer address for the socket connection
    const int get_port_no() const; ///< This function returnsthe peer post number for the connection
    const int get_handle() const; ///< This runftion returns the handle assigend to the socket
    const bool get_handle(sockaddr_in socket_addr); ///< This function returns the gandle from a socket_addrin

    void shutdown(); ///< This function shuts down the communication link on both ends of the socket
    void close(); ///< This function closes and changes the socket_handle to invalid

protected:
    // socket configuration options
    static const int MAXHOSTNAME;
    static const int MAXCONNECTIONS;
    static const int MAXRECV;

    // socket connection status definitions
    static const int socket_accept_fail;
    static const int socket_read_fail;
    static const int socket_write_fail;
    static const int socket_invalid_handle;
    static const int socket_created;
    static const int socket_bound;
    static const int socket_listening;
    static const int socket_connected;
    static const int socket_closed;
    int on;

private:
    int socket_handle; ///< This member holds the handle assigned to the socket 
    sockaddr_in socket_addr; ///< This member holds the sockaddr_in
};

#endif
