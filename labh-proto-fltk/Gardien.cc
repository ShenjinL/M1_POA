#include "Gardien.h"
#include <iostream>
#include <math.h>

#include "Labyrinthe.h" // obtenir _data[LAB_WIDTH][LAB_HEIGHT]
#include "Chasseur.h" // obtenir methodes de chasseur 

using namespace std;

/*
 *	Constructeur.
 */
Gardien::Gardien (Labyrinthe* l, const char* module) : Mover (120, 80, l, module)
{
  // initialise les sons.
  if (_guard_fire == 0) {
	  _guard_fire = new Sound ("sons/guard_fire.wav");
  }
  if (_guard_hit == 0) {
	  _guard_hit = new Sound ("sons/guard_hit.wav");
  }
  if (_guard_die == 0) {  
    _guard_die = new Sound ("sons/guard_die.wav");
  }
  if (_wall_hit == 0) {
		_wall_hit = new Sound ("sons/hit_wall.wav");
  }
  
  // recuperer la carte
  // data = &(l -> _data);


  // initialise les positions
  getOri = false;
  scale  = Environnement::scale;
  vitesse = 0.3; 
  dis_fire = rand()%15+10;
  time_wait = 2;
  last_fire_time = 0;

	// initialise les status
  mode = 'd';
  potentiel = 0;
  seuil = 45 + rand()%10;
	HP = HP_MAX;
	HP_recouvre = 1; 	// Gardien recouvre 1 HP par seconde
	vivant = true;

	// initialise les temps pour recouvrer
	blesse_time = 0;
	increment_time = 2;
	dernier_recouvre_time = time(0);
}

/*
 *	Mise à jour.
 */
void Gardien::update()
{
  // Le chasseur recouvre son capital
  if (((Chasseur*)(_l -> _guards[0])) -> vivant) {
    if (((Chasseur*)(_l -> _guards[0])) -> game) {
      ((Chasseur*)(_l -> _guards[0])) -> recouvre();
    }
  } else {
    _angle += 90; // le gardien dance si chasseur est mort
  }

  // Obtenir la position initiale de gardien
  if (!getOri) {
    ori_x = _x; ori_y = _y;
    getOri = true;
  }

  // Le gardien est vivant
  if (vivant) {
    // Le gardien recouvre son capital
    recouvre();
    // Comparer le potentiel de protection et le seuil
    potentiel = potentiel_protection();
    if (potentiel < seuil) {
      if (mode == 'a') {
        ori_x = _x; ori_y = _y;
        mode = 'd';
      }
    } else {
      mode = 'a';
    }

    //en mode defense
    if(mode=='d'){
      float dx,dy;
      g_angle = _angle + 90;
      dx = 0.25*vitesse*scale*cos(angle_to_radian(g_angle));
      dy = 0.25*vitesse*scale*sin(angle_to_radian(g_angle));
      if(move(dx,dy) && move(dx, 0.0) && move(0.0,dy) ){
        // move
      } else {
        change_direction();
      }
      if (_x>ori_x+80 || _x<ori_x-80 || _y>ori_y+80 || _y<ori_y-80) {
        change_direction();
      }
    } // fin en mode defense

    // en mode attaque
    if (mode=='a') {
      float dx, dy, radian;

      float chasseur_x = _l -> _guards[0] -> _x;
      float chasseur_y = _l -> _guards[0] -> _y;

      radian = atan2(chasseur_y-_y, chasseur_x-_x);
      g_angle = radian_to_angle(radian);
      _angle = g_angle - 90;
    
      dx = 0.25*vitesse*scale*cos(radian);
      dy = 0.25*vitesse*scale*sin(radian);

      float dis_cg = distance(chasseur_x/scale, chasseur_y/scale, _x/scale, _y/scale);
      // comparer la distance entre chasseur et gardien avec la distance de tir.
      if (dis_cg <= dis_fire) {
        current_time = time(0);
        if (current_time - last_fire_time >= time_wait && regarde(chasseur_x,chasseur_y,radian)) {
          fire(0);
          last_fire_time = current_time;
        }
      } else {
        if (move(dx, dy)) {
          // move
        } else {
          _angle += 90;
          g_angle += 90;
          radian = angle_to_radian(g_angle);
          dx = 0.25*vitesse*scale*cos(radian);
          dy = 0.25*vitesse*scale*sin(radian);
          move(dx, dy);
        }
      }
    } // fin en mode attaque
  } else {
    rester_au_sol();
  }
}

/*
 *	Tente un deplacement.
 */
bool Gardien::move(double dx, double dy)
{

  if (EMPTY == _l -> data ((int)((_x + dx) / scale),
							             (int)((_y + dy) / scale)))
	{
	  _x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Calcule le potentiel de protection.
 */
float Gardien::potentiel_protection() {
  // positon de chasseur
  float chasseur_x = _l -> _guards[0] -> _x;
  float chasseur_y = _l -> _guards[0] -> _y;
  // position de tresor
  tresor_x = _l -> _treasor._x;
  tresor_y = _l -> _treasor._y;

  // distance entre gardien et chasseur
  float dis_gc = distance(chasseur_x/scale, chasseur_y/scale, _x/scale, _y/scale);
  // distance entre gardien et tresor
  float dis_gt = distance(_x/scale, _y/scale, tresor_x, tresor_y);
  // distance entre chasseur et tresor
  float dis_ct = distance(chasseur_x/scale, chasseur_y/scale, tresor_x, tresor_y);

  if (dis_gc<30) {
     potentiel += (30-dis_gc);
  } else {
    potentiel = 0;
  }
  if (dis_gt>dis_ct && dis_gc<(dis_ct/2)) {
    potentiel += 50;
  }
  if (regarde(chasseur_x, chasseur_y, atan2(chasseur_y-_y, chasseur_x-_x))) {
    potentiel += 50;
  }
  return potentiel;
}

/*
 *	Tire sur le chasseur.
 */
void Gardien::fire(int angle_vertical)
{
  int angle_fire = 90-g_angle;
  srand(time(NULL));
  angle_fire +=  (int)(100/HP)*(rand()%10-5);
  _guard_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, angle_fire); 
}

/*
 *	Fait bouger la boule de feu
 */
bool Gardien::process_fireball(float dx, float dy)
{
  // calculer la distance entre le gardien et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / scale;
	float	y = (_y - _fb -> get_y ()) / scale;
	float	dist2 = x*x + y*y;

  // position de chasseur
  int chasseur_x = (int)(_l -> _guards[0] -> _x / scale);
  int chasseur_y = (int)(_l -> _guards[0] -> _y / scale);

  // position de fire
  int fb_x = (int)((_fb -> get_x() + dx)/scale);
  int fb_y = (int)((_fb -> get_y() + dy)/scale);

  if (chasseur_x == fb_x && chasseur_y == fb_y) {
    ((Chasseur*)(_l -> _guards[0])) -> blesse();
    if (! (((Chasseur*)(_l -> _guards[0])) -> vivant)) {
      message(" You are die. Good Game ");
      ((Chasseur*)(_l -> _guards[0])) -> game_over();
    }
    return false;
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
 *	vérifie si le gardien regarde le chasseur
 */
bool Gardien::regarde (float dx, float dy, float radian)
{
  float x,y;
  x = _x; y = _y;
  while (!(int(x/scale)==int(dx/scale) && int(y/scale)==int(dy/scale))) {

    for (int i=0; i<_l->_nwall; i++) {
      if ((_l->_walls[i]._x1 == int(x/scale) && _l->_walls[i]._y1 == int(y/scale)) ||  
          (_l->_walls[i]._x2 == int(x/scale) && _l->_walls[i]._y2 == int(y/scale))) {
            return false;
      }
    }
    x += 0.25*vitesse*scale*cos(radian);
    y += 0.25*vitesse*scale*sin(radian);
  }
  return true;
}

/*
 *	Frappé par une boule
 */
void Gardien::blesse ()
{
	_guard_hit -> play ();
	blesse_time = time(0);
	dernier_recouvre_time = blesse_time + increment_time;
	HP -= 20;
	if (HP <= 0) {
		vivant = false;
    _guard_die -> play();
	}
}


/*
 *	Recouvre le capital
 */
void Gardien::recouvre()
{
  time(&current_time);
  // current_time = time(0);
  if (current_time - blesse_time >= increment_time) {
    int HP_ajout = (current_time - dernier_recouvre_time) * HP_recouvre;
    if (HP + HP_ajout >= HP_MAX) {
      HP = HP_MAX;
    } else {
      HP += HP_ajout;
    }
    dernier_recouvre_time = current_time;
  }

}
