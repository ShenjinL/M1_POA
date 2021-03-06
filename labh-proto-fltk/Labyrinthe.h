#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

#define	LAB_WIDTH	200
#define	LAB_HEIGHT	200

class Labyrinthe : public Environnement {
public:
	char	_data [LAB_WIDTH][LAB_HEIGHT];

public:
	Labyrinthe (char*);
	int width () { return LAB_WIDTH;}	// retourne la largeur du labyrinthe.
	int height () { return LAB_HEIGHT;}	// retourne la longueur du labyrinthe.

	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
};

#endif
