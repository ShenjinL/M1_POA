#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <unistd.h>

class tcp_socket
{
protected:
	int		_sd;
	bool	_error;

public:
	tcp_socket (int s) : _sd(s), _error (false) {}
	tcp_socket();

	virtual ~tcp_socket () { if (_sd != -1) ::close (_sd); }
	int get_fd () { return _sd; }
	bool error () const { return _error; }
};


class tcp_client_socket : public tcp_socket
{
public:
	tcp_client_socket (const char* hostname, int port =  0);
	tcp_client_socket (int s) : tcp_socket (s) {}

	virtual ~tcp_client_socket () {}
};


class tcp_server_socket : public tcp_socket
{
public:
	tcp_server_socket (int port, int maxconn = 10);
	virtual ~tcp_server_socket () {}
	char*	where (int fd, unsigned char** ipaddr =0);
	int accept (void);
};

#endif
