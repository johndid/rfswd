#ifndef Socket_exception_class
#define Socket_exception_class

#include <string>
/// This class implements the socket_exception that can be thrown from the socket and decendant classes
class Socket_exception
{
 public:
  /// constructor:
  /// @param _exception_description std::string containing the exceptions description
  Socket_exception ( std::string _exception_description ) : exception_description(_exception_description) {};
  ~Socket_exception (){};

  /// This function returns the exceptions description
  /// @return std::string
  std::string description() { return exception_description; } 

 private:
  std::string exception_description; ///< This member holds the description on the exception
};

#endif
