#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"
#include <ctime>

#define	HP_MAX 100

class Labyrinthe;

class Chasseur : public Mover {
private:
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);

	// *** STATUS ***
	int HP;			// Le capital initial

	// *** INCREMENTER CAPITAL ***
	int HP_recouvre; 				// Le capital recouvré chaque fois
	time_t current_time;			// Le temps du systeme
	time_t blesse_time;				// Le temps de dernier blesse
	time_t increment_time;			// La duree entre blesse et recouvrer
	time_t dernier_recouvre_time; 	// Le temps de dernier recouvre 

public:
	// *** les sons ***
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touché.
	static Sound*	_wall_hit;		// on a tapé un mur.
	
	bool vivant; 	// Le chasseur est vivant
	bool game;		// La partie est commence

	// constructeur.
	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) {};
	// fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);

	// perdre le capital
	void blesse ();

	// incrementer le capital
	void recouvre();

	// terminer le jeu
	void game_over();
};

#endif
