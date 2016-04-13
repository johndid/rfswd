#ifndef Client_socketH
#define Client_socketH

#include <socket.h>
#include <vector>

/// @b This client socket class is an implementation of a client
class Client_socket : private Socket
{
 public:
  /// @b create client socket by assigning teh address to connect to and it's port number
  /// @param host std::string sontaining the hostname as an ip address <xxx.xxx.xxx.xxx>
  /// @param port int containing the port number that is used
  /// throws Socket_exception
  Client_socket(const std::string host, const int port);
  ~Client_socket(void);

  /// @b This function sends data to the server
  /// @param message std::string containing the message to send to the server
  /// throws Socket_exception
  const Client_socket& operator << (const std::string& message) const;
  const Client_socket& operator << (std::vector<std::string>& queue) const;

  /// @b This function receives data to the server
  /// @return std::string containing the message that is received from the server
  /// throws Socket_exception
  const Client_socket& operator >> (std::string&) const;
  const Client_socket& operator >> (std::vector<std::string>& queue) const;
};
#endif
