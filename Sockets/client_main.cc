#include <stdio.h>
#include <string.h>
#include "tcp_socket.h"
#include "protocol.h"

int main (int argc, char** argv)
{
	char*	host = "localhost";

	if (argc == 2)	host = argv [1];

	tcp_client_socket	sock (host, 3456);

	if (sock.error ()) {
		fprintf (stderr, "Erreur ouverture socket.\n");
		return 1;
	}
	int			n;
	char		buf [128];
	protocol	prot (sock.get_fd ());

	n = 1 + sprintf (buf, "Test de message envoye par: %d", ::getpid ());

	if (n != prot.send (buf, n))
		fprintf (stderr, "Erreur: taille message %d != %d\n", 1 + strlen (buf));

	if (prot.receive (buf))
		fprintf (stderr, "Reponse: %s\n", buf);

	return 0;
}
