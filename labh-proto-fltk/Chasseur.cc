#include "Chasseur.h"
#include <iostream>
#include <math.h>

#include "Gardien.h" // obtenir methodes de gardien 

using namespace std;

/*
 *	Tente un deplacement.
 */
bool Chasseur::move_aux (double dx, double dy)
{
	if (vivant) {
		int x = (int)((_x + dx) / Environnement::scale);
		int y = (int)((_y + dy) / Environnement::scale);

		if (_l -> _treasor._x == x && _l -> _treasor._y) {
			message("You are win.");
			game_over();
		}

		if (EMPTY == _l -> data (x,y)) {
			_x += dx;
			_y += dy;
			return true;
		}
	}
	return false;
}

/*
 *	Constructeur.
 */
Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// commence une partie
	game = true;

	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0) {
		_wall_hit = new Sound ("sons/hit_wall.wav");
	}
	
	// initialise les status
	HP = HP_MAX;
	HP_recouvre = 5; 	// Chasseur recouvre 5 HP par seconde
	vivant = true;

	// initialise les temps
	blesse_time = 0;
	increment_time = 2;
	dernier_recouvre_time = time(0);
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */
bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	// position de fire
  	int fb_x = (int)((_fb -> get_x() + dx) / Environnement::scale);
  	int fb_y = (int)((_fb -> get_y() + dy) / Environnement::scale);

	// marcher tous les gardiens et verifie si la boule touche un parmi eux
	for (int i=1; i<(_l->_nguards); i++) {
		int gardien_x = (int)((_l -> _guards[i] -> _x) / Environnement::scale);
		int gardien_y = (int)((_l -> _guards[i] -> _y) / Environnement::scale);

		if (gardien_x == fb_x && gardien_y == fb_y) {
			((Gardien*)(_l -> _guards[i])) -> tomber();
			((Gardien*)(_l -> _guards[i])) -> blesse();
			if (! (((Gardien*)(_l -> _guards[i])) -> vivant)) {
				((Gardien*)(_l -> _guards[i])) -> rester_au_sol();
			}
			return false;
		}
	}

	// on bouge que dans le vide!
	if (EMPTY == _l -> data (fb_x, fb_y))
	{
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	// message ("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */
void Chasseur::fire (int angle_vertical)
{
	// message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Frappé par une boule.
 */
void Chasseur::blesse ()
{
	_hunter_hit -> play ();
	blesse_time = time(0);
	dernier_recouvre_time = blesse_time + increment_time;
	HP -= 20;
	if (HP <= 0) {
		vivant = false;
	}
}

/*
 *	Recouvre le capital
 */
void Chasseur::recouvre()
{
	current_time = time(0);
	if (current_time - blesse_time >= increment_time) {
		int HP_ajout = (current_time - dernier_recouvre_time)*HP_recouvre;
		if (HP + HP_ajout >= HP_MAX) {
			HP = HP_MAX;
		} else {
			HP += HP_ajout;
		}
		dernier_recouvre_time = current_time;
	}
	message("HP : %d", (int) HP);
}

/*
 *	Termine la partie .
 */
void Chasseur::game_over()
{
	game = false;
	for (int i=1; i<(_l->_nguards); i++) {
		(((Gardien*)(_l -> _guards[i])) -> vivant) = false;
	}
}