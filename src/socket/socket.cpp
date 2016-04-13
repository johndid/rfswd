#include <socket/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sstream> 
#include <iostream>
#include <iomanip>
#include <errno.h>
#include <stdio.h>
#include <string.h>

using namespace std;

const int Socket::MAXHOSTNAME = 200;
const int Socket::MAXCONNECTIONS = 5;
const int Socket::MAXRECV = 8192;

const int Socket::socket_accept_fail = -1;
const int Socket::socket_read_fail = -1;
const int Socket::socket_write_fail = -1;
const int Socket::socket_invalid_handle = -1;
const int Socket::socket_created = 0;
const int Socket::socket_bound = 0;
const int Socket::socket_listening = 0;
const int Socket::socket_connected = 0;
const int Socket::socket_closed = 0;

Socket::Socket() : socket_handle(socket_invalid_handle)
{
    memset(&socket_addr, 0, sizeof(socket_addr));
    on=true;
}

Socket::~Socket()
{
    if(is_valid()) 
    {
	::shutdown(socket_handle,SHUT_RDWR);
	::close(socket_handle);
    }
}

const bool Socket::create()
{
    socket_handle = ::socket( AF_INET, SOCK_STREAM, 0 );
    if (is_valid()) 
    {
	const int status = ::setsockopt(socket_handle, SOL_SOCKET, SO_REUSEADDR|SO_KEEPALIVE, (const char*) &on, sizeof(on));
	if (socket_created == status) 
	{
	    return true;
	}
	//	log::msg()->error("Socket::create() failed");
    }
    return false;
}

const bool Socket::bind(const int port)
{
    if(is_valid()) 
    {
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(port);

	const int bind_return = ::bind( socket_handle, (struct sockaddr*) &socket_addr, sizeof(socket_addr));
	if(socket_bound == bind_return) 
	{
	    return true;
	}
	perror("Socket::bind failed");
    }
    return false;
}

const bool Socket::listen(const int num_connections) const
{
    if(is_valid()) 
    {
	const int listen_return = ::listen( socket_handle, num_connections );
	if( 0 == listen_return) 
	{
	    return true;
	}
	perror("Socket::listen failed");
    }
    return false;
}

const bool Socket::accept(Socket *new_socket) const
{
    if(is_valid()) 
    {
	const int addr_length = sizeof(socket_addr);
	new_socket->socket_handle = ::accept(socket_handle,(sockaddr*)&socket_addr,(socklen_t*)&addr_length);

	if(socket_accept_fail < new_socket->socket_handle) 
	{
	    memcpy((void*)&new_socket->socket_addr,(void*)&socket_addr,sizeof(socket_addr));
	    return true;
	}
	//	log::msg()->error("Socket::accept failed");
	::close(socket_handle);
    }
    return false;
}

const bool Socket::send(const std::string s) const
{
    if(is_valid()) 
    {
	// because the AdobeFlash expects a '\0' as message termination we use the size of the string +1
	// to ensure that the transmission of the tailing zero in the string is actually send
	if(-1 != ::send(socket_handle, s.c_str() , s.size()+1, 0))
	{
	    return true;
	}
	//	log::msg()->error("Socket::send failed");
    }
    return false;
}

const int Socket::recv_buffer(vector<char> &rawdata) const
{
    int bytes_read = 0;
    if(is_valid())
    {
	char buf[MAXRECV+1];
	memset(buf,0,MAXRECV+1);
	
	while(bytes_read = ::recv(socket_handle,buf,MAXRECV, MSG_DONTWAIT ), bytes_read > 0)
	{
//	    cout << buf << endl;
	    rawdata.insert(rawdata.end(),buf, buf + bytes_read);
	    memset(buf,0,MAXRECV+1);
 	}

	// nothing to read or the socket woild be blocking, don't react!
	if(0 == bytes_read || errno == EWOULDBLOCK || errno == EAGAIN)
	{
	    bytes_read = 0;
	}
	else
	{
	    // some error occured. Dump it!
	    cout << strerror(errno) << endl;
	}
    }
    return (0 < bytes_read) ? rawdata.size() : bytes_read;
}

// insert rav data into this mutha fucka! usinf vector<char> ;)
const int Socket::recv(std::string& s) const 
{
    int bytes_read=0;
    if(is_valid()) 
    {
	char buf[MAXRECV+1];
	memset(buf,0,MAXRECV+1);
	
	while(bytes_read = ::recv(socket_handle,buf,MAXRECV, MSG_DONTWAIT ), bytes_read > 0)
	{
//	    cout << buf << endl;
	    s.append(buf,static_cast<size_t>(bytes_read));
	    memset(buf,0,MAXRECV+1);
 	}

	if(0 == bytes_read || errno == EWOULDBLOCK || errno == EAGAIN)
	{
		bytes_read = 0;
	}
	else
	{
	    cout << strerror(errno) << endl;
	}
    }
    return (0 < bytes_read) ? s.size() : bytes_read;
}

const bool Socket::connect(const std::string host, const int port)
{
    if(is_valid()) 
    {
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);

	int status = inet_pton(AF_INET, host.c_str(), &socket_addr.sin_addr);
	if(status) 
	{
	    status = ::connect(socket_handle,(sockaddr*) &socket_addr, sizeof (socket_addr));
	    if(socket_connected == status) 
	    {
		return true;
	    }
	}
	perror("Socket::connection failed");
    }
    return false;
}

void Socket::set_non_blocking(const bool b) //TODO: the parameter should be removed
{
    if(is_valid()) 
    {
	int flags;
	if (-1 == (flags = fcntl(socket_handle, F_GETFL, 0)))
	{
	    flags = 0;
	}

	if(b) 
	{
	    flags |= O_NONBLOCK;
	}

	fcntl(socket_handle, F_SETFL, flags );
    }
}

const bool Socket::is_valid(void) const
{
    return (socket_handle != socket_invalid_handle);
}

const int Socket::get_handle() const {
    return socket_handle;
}

const char *Socket::get_addr(void) const
{
    return inet_ntoa(socket_addr.sin_addr);
}

const int Socket::get_port_no(void) const
{
    return socket_addr.sin_port;
}

const bool Socket::get_handle(sockaddr_in _socket_addr) 
{
    socket_addr = _socket_addr;
    return true;
}

void Socket::close()
{
    if(is_valid()) 
    {
	::close(socket_handle);
	socket_handle = socket_invalid_handle;
    }
}

void Socket::shutdown()
{
    if(is_valid()) 
    {
	int ret = ::shutdown(socket_handle,2);
	if(0 != ret) 
	{
	  stringstream s ;
	  s << setw(3) << setfill('0') << ret;
	  cout << "Socket still waiting?!?!: " << s;
	}
    }
}
