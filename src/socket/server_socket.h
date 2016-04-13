#ifndef Server_socketH
#define Server_socketH

#include <socket.h>
#include <map>
#include <vector>
#include <string>

///@b Server_socket
///The Server_socket class acts as serving socket connection
///It listens for incoming connections on a given port number
///
///@see Socket
class Server_socket : public Socket
{
public:
    ///@b Server_socket
    Server_socket(){};
    
    ///@b Server_socket
    ///Constructor for creating a listening socket
    ///
    ///@param port_number integer containing the port number that the socket should listen to
    ///@param num_connections integer containing the number of valid connections for this socket
    ///throws SocketException
    Server_socket(const int port_number,const int num_connections);
    ~Server_socket();
    
    ///@b operator<<
    ///Sends a data string over the socket connection
    ///
    ///@param msg std::string containg the message to send
    ///@return Server_socket
    ///throws Socket_exception
    const Server_socket& operator << ( const std::string& msg) const;
    const Server_socket& operator << (std::vector<std::string>& queue) const;
    
    ///@b operator<< 
    ///Receives a data string from a peer connection
    ///
    ///@param msg std::string containing the received data string
    ///@return Server_socket
    ///throws Socket_exception
    const Server_socket& operator >> ( std::string& msg) const;
    const Server_socket& operator >> (std::vector<std::string>& queue) const ;

    ///@b accept
    ///This function accepts incoming connections
    ///@param sock Server_socket to accept as incoming connection
    ///@return bool true if the server accepted the socket connection, false if not
    const bool accept(Server_socket *sock);

    ///@b get_client
    ///This function returns the socket associated with the client.
    /// The returned socket is a server socket representing the client's connection on the server side.
    /// @param handle integer containing the handle for the for the socket 
    /// @return Server_socket if the handle is ok this is a valid pointer, invalid handles causes the function to return NULL
    Server_socket *const get_client(const int handle);

 private:
    /// This member holds the connections and their associated handles. Handles are used as identifiers
    /// as these are ints instead of pointers
    std::map<int,Server_socket*> connections; 
};

#endif // Server_socketH
