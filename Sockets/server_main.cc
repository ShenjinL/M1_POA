#include <stdio.h>
#include "tcp_socket.h"
#include "protocol.h"
#include "thread.h"

class Service : public thread
{
private:
	int					_fd;
	tcp_server_socket*	_sock;

	void run (void) {
		char		buf [128];
		protocol	prot (_fd);

		if (prot.receive (buf)) {
			char*	host = _sock -> where (_fd);
			fprintf (stderr, "Message de: %s : '%s'\n", host, buf);
			prot.send ("OK", 3);
		} else
			fprintf (stderr, "Erreur: taille message\n");

		::close (_fd);
	}

public:
	Service (int fd, tcp_server_socket* s) : _fd (fd), _sock (s) {}
};

int main (int argc, char** argv)
{
	tcp_server_socket	sock (3456);

	if (sock.error ()) {
		fprintf (stderr, "Erreur ouverture socket.\n");
		return 1;
	}
	for (;;) {
		int		fd = sock.accept ();

		if (fd < 0)
			continue;		// réveillé pour rien???

		(new Service (fd, &sock)) -> start ();
	}

	return 0;
}
