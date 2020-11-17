#include <stdio.h>
#include "tcp_socket.h"

// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// gethostname
#include <unistd.h>
#include <errno.h>

// strings
#include <string.h>


tcp_socket::tcp_socket ()
{
	_sd = ::socket (AF_INET, SOCK_STREAM, 0);
	_error = (_sd == -1);
}

tcp_client_socket::tcp_client_socket (const char* hostname, int port)
	: tcp_socket()
{
	int			res = 0;
	const char*	host;

	if (hostname [0] == 0)
		host = "localhost";
	else
		host = hostname;

	hostent*	theHostent = ::gethostbyname(host);
	if (theHostent == 0) {
		_error = true;
		return;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)theHostent->h_addr;
	memset (addr.sin_zero, 0, sizeof (addr.sin_zero));
  
	res = ::connect (_sd, (sockaddr*) &addr, sizeof(addr));
	_error |= (res != 0);
}

tcp_server_socket::tcp_server_socket(int port, int maxconn) : tcp_socket()
{
	int					res;
	int					optval = 1;
	struct sockaddr_in	sock;

	if (_error)
		return;		// faillu !

	res = setsockopt (_sd, SOL_SOCKET, SO_REUSEADDR, (char*) &optval, sizeof (int));
	memset (&sock, 0, sizeof (sock));
	sock.sin_port = htons ((unsigned short) (port & 0xFFFF));
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	if (bind (_sd, (struct sockaddr*) (&sock), sizeof (struct sockaddr)) != 0 ||
		listen (_sd, maxconn) != 0)
	{
		::close (_sd);
		_error = true;
		_sd = -1;
	}
}

char* tcp_server_socket::where (int fd, unsigned char** v)
{
	static char					result [256];
	static struct sockaddr_in	addr;	// doit résister au retour de la fonction.
	unsigned char*				ipaddr;
	struct hostent*				h;
	socklen_t					lenaddr = sizeof (addr);

	if (getpeername (fd,(struct sockaddr *)&addr, &lenaddr) == -1)
		return 0 ;

	ipaddr = (unsigned char *)&addr.sin_addr.s_addr ;
	if ((h = gethostbyaddr ((char *)&addr.sin_addr, sizeof (addr.sin_addr), AF_INET)))
		sprintf (result, "%s", h -> h_name);
	else
		sprintf (result, "%d.%d.%d.%d", ipaddr [0], ipaddr [1], ipaddr [2], ipaddr [3]);
	// retourner aussi l'adresse ip.
	if (v != 0)
		*v = ipaddr;
	return result;
}

int tcp_server_socket::accept (void)
{
	struct sockaddr_in	addr;
	socklen_t			lenaddr = sizeof(addr);

	return ::accept (_sd, (struct sockaddr*) (&addr), &lenaddr);
}
