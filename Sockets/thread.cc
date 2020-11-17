#include "thread.h"

/*************************************************************
 *
 *	Les threads (à linker avec -lpthread)
 *	
 *************************************************************/

/*
 *	Lance la méthode 'run' de la thread.
 */

void* thread::srun (void* th)
{
	((thread*) th) -> run ();
	((thread*) th) -> _thread = 0;
	return 0;
}

/*
 *	Crée et démarre une nouvelle thread.
 */

bool thread::start ()
{
	return !(_thread != 0 || 0 != ::pthread_create (&_thread, 0, srun, (void*) this));
}

/*
 *	Attends la fin de la thread courante.
 */

bool thread::join ()
{
	return !(_thread == 0 || 0 != ::pthread_join (_thread, 0));
}
