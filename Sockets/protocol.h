#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>

class protocol {
private:
	int	_fd;
	static void encode (int, char*);
	static int decode (char* len);

public:
	protocol (int fd) : _fd (fd) {};
	int send (char*, int);
	bool receive (char*);
};


#endif
