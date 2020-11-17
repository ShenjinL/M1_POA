#ifndef THREAD_H
#define THREAD_H

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

/*
 *	Les threads
 */

class thread {
private:
	static void* srun (void*);		// 'run' des pthread.

	pthread_t	_thread;
protected:
	thread () { _thread = 0; }
	virtual ~thread () {
		fprintf (stderr, "thread détruit\n");
		if (_thread != 0) ::pthread_exit (0);
	}
	// doit être redéfini quand on dérive de thread.
	virtual void run (void) =0;
public:
	// indique si le processus est lancé.
	bool alive () { return _thread != 0; };
	// démarre la thread.
	bool start ();
	// attend la fin de la thread.
	bool join ();
	// endors la thread courante pendant 'milli' millisecondes.
	static void sleep (int milli) { usleep (1000*milli); }
};
#endif
