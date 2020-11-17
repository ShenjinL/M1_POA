#include <unistd.h>
#include "protocol.h"

/*
 *	Encode un entier sur 4 octets.
 */

inline void protocol::encode (int n, char* len)
{
	len [0] = n & 0xff;
	len [1] = (n >> 8) & 0xff;
	len [2] = (n >> 16) & 0xff;
	len [3] = (n >> 24) & 0xff;
}

/*
 *	Décode un entier de 4 octets.
 */

inline int protocol::decode (char* len)
{
	return	((len [3] & 0xff) << 24) |
			((len [2] & 0xff) << 16) |
			((len [1] & 0xff) << 8) |
			(len [0] & 0xff);
}

int protocol::send (char* buf, int length)
{
	char	len [4];

	encode (length, len);
	if (4 != ::write (_fd, len, 4))
		return -1;
	return ::write (_fd, buf, length);
}

bool protocol::receive (char* buf)
{
	char	len [4];

	if (4 != ::read (_fd, len, 4))
		return -1;
	int	n = decode (len);
	return n == ::read (_fd, buf, n);
}
