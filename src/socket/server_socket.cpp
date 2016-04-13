#include <server_socket.h>
#include <socket_exception.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

Server_socket::Server_socket(const int port,const int num_connections) 
{

    if(!Socket::create()) 
    {
	throw Socket_exception("Server_socket::Server_socket -> failed to create");
    }

    if (!Socket::bind(port)) 
    {
	throw Socket_exception("Server_socket::Server_socket -> failed to bind");
    }

    if(!Socket::listen(num_connections)) 
    {
	throw Socket_exception("Server_socket::Server_socket -> failed to listen");
    }
}

Server_socket::~Server_socket()
{
    connections.erase(connections.begin(),connections.end());
}

const Server_socket& Server_socket::operator << (const std::string& message) const 
{
    if (!Socket::send(message)) 
    {
	throw Socket_exception("Server_socket::operator << -> failed to send");
    }
    return *this;
}

const Server_socket& Server_socket::operator >> (std::string& message) const 
{
    int status = Socket::recv(message);
    if(-1 == status)
    {
	throw Socket_exception ("Server_socket::operator >> -> failed to read");
    }
    return *this;
}

const Server_socket& Server_socket::operator << (vector<string>& queue) const 
{
    for(vector<string>::iterator it = queue.begin();it != queue.end();it++) 
    {
	const string message(*(it));
	if (!Socket::send(message)) 
	{
	    ostringstream s;
	    s << distance(queue.begin(),it);
	    throw Socket_exception("Server_socket::operator << -> failed to send: " + s.str() + " aborting");
	}
    }
    return *this;
}

const Server_socket& Server_socket::operator >> (vector<std::string>& queue) const 
{
    vector<char> message_buffer;
    const int status = recv_buffer(message_buffer); // this function blocks until it has read the incoming msg stream!
    if(-1 == status)
    {
	throw Socket_exception ("Server_socket::operator >> -> failed to read");
    }
    else
    {
	if(!message_buffer.empty())
	{
	    // plug the message(s) into a queue of message(s)
	    // this decodes both a sequence of many messages received in order
	    // and a large message!
	    string message;
	    vector<char>::iterator it;
	    for(it=message_buffer.begin(); it != message_buffer.end();it++ )
	    {
		if('\0' == *it )
		{
		    queue.push_back(message);
		    message="";
		}
		else
		{
		    message += *it;
		}
	    }
	    message_buffer.clear();
	}
    }
    return *this;
}

const bool Server_socket::accept(Server_socket* sock) 
{
    if(sock) 
    {
	const int ret = Socket::accept(sock);
	if(ret) 
	{
	    connections[sock->get_handle()] = sock;
	    log::msg()->debug("Client@" + string(sock->get_addr()) + ":" + converter::int2string(sock->get_port_no()) + " assigning: " + converter::int2string(sock->get_handle()));
	    return true;
	}
	else 
	{
	    log::msg()->error("Server_socket::accept -> failed to accept connection from: " + string(sock->get_addr()));
	}
    }
    else 
    {
	log::msg()->error("Server_socket::accept -> failed to accept no socket!");
    }
    return false;
}

Server_socket *const Server_socket::get_client(const int handle) 
{
    map<int,Server_socket*>::iterator it = connections.find(handle);
    if(it != connections.end()) 
    {
	return (it)->second;
    }
    return NULL;
}
