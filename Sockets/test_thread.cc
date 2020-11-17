#include <stdio.h>
#include "thread.h"

class Test : public thread
{
private:
	int		_n;
	char	_id;

	void run (void)
	{
		for (int n = 10; --n >= 0;) {
			fprintf (stderr, "%c", _id);
			sleep (_n);
		}
	}

public:
	Test (char id, int n) : _n (n), _id (id) {}
};

int main (int argc, char** argv)
{
	Test t1 ('A', 300);
	Test t2 ('B', 900);

	t1.start ();
	t2.start ();
	t1.join ();
	t2.join ();
	fprintf (stderr, "\nTerminé.\n");
	return 0;
}
