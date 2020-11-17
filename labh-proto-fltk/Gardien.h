#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>

#define	HP_MAX 100

class Labyrinthe;

class Gardien : public Mover {
private:

	//  *** STATU ***
	char mode;				// Le mode de gardien (defense/attaque/tue)
	float potentiel;		// Le potentiel de protection
	float seuil;			// Le seuil
	int HP;					// Le capital initial

	// *** Carte ***
	// char (*data)[200][200];	// labyrinthe data

	// *** POSITION & MOUVEMENT ***
	float scale;			// Environnement scale
	float ori_x, ori_y;		// La position de gardien où il est né
	bool getOri;
	int g_angle;			// l'angle de movement du gardien
	float vitesse;			// La vitesse de gardiens
	int tresor_x, tresor_y;	// La position de tresor

	// *** ATTAQUE ***
	float dis_fire;			// La distance de fire()
	time_t current_time;	// Le temps du systeme
	time_t time_wait;		// Le temps entre deux fire()
	time_t last_fire_time;	// Le temps de dernier fire()

	// *** INCREMENTER CAPITAL ***
	int HP_recouvre; 				// Le capital recouvré chaque fois
	time_t blesse_time;				// Le temps de dernier blesse
	time_t increment_time;			// La duree entre blesse et recouvrer
	time_t dernier_recouvre_time; 	// Le temps de dernier recouvre 

public:

	// *** les sons ***
 	static Sound*	_guard_fire;	// bruit de l'arme du gardien
	static Sound*	_guard_hit;		// cri du gardien touché
	static Sound* 	_guard_die;		// cri du gardien tué
	static Sound* 	_wall_hit;		// on a tapé un mur

	bool vivant;			// Le gardien est vivant

	Gardien (Labyrinthe* l, const char* modele);
	// mon gardien pense tr�s mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);

	// Si le chasseur est dans la vue de gardien 
	bool regarde(float dx, float dy, float radian);

	// Calculer le potentiel de protection
	float potentiel_protection();
	
	// perdre le capital
	void blesse ();
	
	// incrementer le capital
	void recouvre();



	// calculer la distance
	float distance(float x1, float y1, float x2, float y2) 
	{
		return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
	}

	// transmettre angle à radian
	float angle_to_radian(int angle)
	{
		return (angle/180.0)*M_PI;
	}

	// transmettre radian à angle
	int radian_to_angle(float radian)
	{
		return int(radian/M_PI*180.0);
	}

	// changer la direction de gardien
	void change_direction()
	{
		srand(time(NULL));
		_angle = _angle + 90 + rand() % 45;
	}
};

#endif
