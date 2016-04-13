#include <client_socket.h>
#include <socket_exception.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace std;

Client_socket::Client_socket(const std::string host, const int port) 
{

    if (!Socket::create()) 
    {
	throw Socket_exception("Client_socket::Client_socket -> failed to create socket");
    }

    if (!Socket::connect(host,port)) 
    {
	ostringstream port_buf;
	port_buf << port;

	string err("Client_socket::Client_socket -> failed to connect to: " + host + ":" + port_buf.str());
	throw Socket_exception(err.c_str());
    }
}

Client_socket::~Client_socket()
{

}

const Client_socket& Client_socket::operator << (const std::string& s) const 
{
    if (!Socket::send(s)) 
    {
	throw Socket_exception("Client_socket::operator << -> failed to send");
    }
    return *this;
}

const Client_socket& Client_socket::operator << (vector<string>& queue) const 
{
    for(vector<string>::iterator it = queue.begin();it != queue.end();it++) 
    {
	const string message(*(it));
	if (!Socket::send(message)) 
	{
	    ostringstream s;
	    s << distance(queue.begin(),it);
	    throw Socket_exception("Server_socket::operator << -> failed to send:" + s.str());
	}
    }
    return *this;
}

const Client_socket& Client_socket::operator >> (vector<std::string>& queue) const 
{
    vector<char> message_buffer;
    // The operator ignores all errors and and wait for either connection closed or a buffer full of bytes
    // this is considered polling on the buffer interface and is is caused by the sockets being NON_BLOCKING
    // and they are non blocking because the threads that use them needs to be shut down politely!
    // if you'll need to just send and receive in a "non polling mode" use the send and receive functions instead!
    int status = Socket::recv_buffer(message_buffer);
    if(-1 == status)
    {
	    throw Socket_exception ("Client_socket::operator >> -> failed to read");
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
	    for(it = message_buffer.begin();it != message_buffer.end();it++)
	    {
		if(*it == '\0') // flash XMl sockets sends '\0' as end of xmlstream, but there can be many of these in a single socket flush()
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

const Client_socket& Client_socket::operator >> (std::string& s) const 
{
    // The operator ignores all errors and and wait for either connection closed or a buffer full of bytes
    // this is considered polling on the buffer interface and is is caused by the sockets being NON_BLOCKING
    // and they are non blocking because the threads that use them needs to be shut down politely!
    // if you'll need to just send and receive in a "non polling mode" use the send and receive functions instead!
    int status = Socket::recv(s);
    if(-1 == status)
    {
	    throw Socket_exception ("Client_socket::operator >> -> failed to read");
    }
    return *this;
}
