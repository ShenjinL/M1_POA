#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>


using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

Sound*	Gardien::_guard_fire;	// bruit de l'arme du gardien.
Sound*	Gardien::_guard_hit;	// cri du gardien touché.
Sound*	Gardien::_guard_die;	// cri du gardien mort.
Sound*	Gardien::_wall_hit;		// on a tapé un mur.


Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

// fonction pour lire le fichier et remplir _data
void getData(char *filename, char _data[LAB_WIDTH][LAB_HEIGHT])
{
    ifstream ReadFile;
    string tmp;
		int ligne = 0;
    ReadFile.open(filename,ios::in);
    if(ReadFile.fail())
    {
        cout << "error read file" << endl;
    }
    else
    {
        while(getline(ReadFile,tmp,'\n'))
        {
            if (tmp=="" || tmp[0]=='#' || tmp.find('#')!=string::npos){
                continue; // ne pas voir une ligne (vide ou commentaire)
            }
            for (int j=0; j<LAB_HEIGHT; j++) {
				if (j<tmp.size()) {
                	_data[ligne][j] = tmp[j];
				} else {
					_data[ligne][j] = EMPTY;
				}
            }
			ligne++;
        }
		for (int i=ligne; i<LAB_WIDTH; i++) {
			for (int j=0; j<LAB_HEIGHT; j++) {
				_data[i][j] = EMPTY;
			}
		}
        ReadFile.close();
    }
}

// fonction pour créer un mur '-'
void createWallHorizontal(int x, int y, Wall walls[], int *nwall)
{
	Wall w;
	w._x1 = x; w._y1 = y; w._x2 = x; w._y2 = y+1; w._ntex = 0;
	walls[*nwall] = w;
	*nwall += 1;
}

// fonction pour créer un mur '|'
void createWallVertical(int x, int y, Wall walls[], int *nwall)
{
	Wall w;
	w._x1 = x; w._y1 = y; w._x2 = x+1; w._y2 = y; w._ntex = 0;
	walls[*nwall] = w;
	*nwall += 1;
}

// fonction pour créer un mur '+'
void createWallCroix(char _data[LAB_WIDTH][LAB_HEIGHT], int x, int y,Wall walls[], int *nwall)
{
	if (x+1<LAB_WIDTH && (_data[x+1][y]=='+' || _data[x+1][y]=='|'))
		createWallVertical(x, y, walls, nwall);
	if (y+1<LAB_HEIGHT && (_data[x][y+1]=='+' || _data[x][y+1]=='-'))
		createWallHorizontal(x, y, walls, nwall);
}

// fonction pour créer un gardien 'G'
void createGuard(int x, int y, Mover *guards[], int *nguards, Labyrinthe *l, string m[])
{
	guards[*nguards] = new Gardien(l, m[rand()%8].data());
	guards[*nguards] -> _x = float(x)*Environnement::scale;
	guards[*nguards] -> _y = float(y)*Environnement::scale;
	guards[*nguards] -> _angle = 0;
	*nguards += 1;
}

// fonction pour créer un chasseur 'C'
void createChasseur(int x, int y, Mover *guards[], Labyrinthe *l)
{
	guards[0] = new Chasseur(l);
	guards[0] -> _x = float(x)*Environnement::scale;
	guards[0] -> _y = float(y)*Environnement::scale;
}

// fonction pour créer une caisse 'x'
void createBox(int x, int y, Box boxes[], int *nboxes)
{
	Box b;
	b._x = x; b._y = y; b._ntex = 0;
	boxes[*nboxes] = b;
	*nboxes += 1;
}

// fonction pour créer une affiche 'a' ou 'b'
void createAffiche(char _data[LAB_WIDTH][LAB_HEIGHT], int x, int y,
					 Wall picts[], int *npicts, int l_tmp[], int t)
{
	Wall p;
	p._x1 = x; p._y1 = y;
	//  les affiches doivent faire 2 de long
	if (x+1<LAB_WIDTH && (_data[x+1][y]=='+' || _data[x+1][y]=='|')) {
		p._x2 = x+2; p._y2 = y;
	} else if (y+1<LAB_HEIGHT && (_data[x][y+1]=='+' || _data[x][y+1]=='-')) {
		p._x2 = x; p._y2 = y+2;
	}
	picts[*npicts] = p;
	l_tmp[*npicts] = t;
	*npicts += 1;
}


Labyrinthe::Labyrinthe (char* filename)
{
	// lire le fichier et ecrire dans _data
    getData(filename, _data);

	// initialiser Murs et nombre
	int nb_case = LAB_WIDTH*LAB_HEIGHT;
	Wall *walls = new Wall[nb_case];
	int nwall = 0;
	// initialiser Affiche, nombre et une liste de type affiche
	Wall *picts = new Wall[nb_case];
	int npicts = 0;
	int *l_tmp = new int[nb_case];
	// initialiser Caisse et nombre
	Box *boxes = new Box[nb_case];
	int nboxes = 0;
	// initialiser Trésor
	Box treasor;
	// initialiser Gardien, Chasseur et nombre (toujour commence par 1 chasseur)
	Mover **guards = new Mover* [nb_case];
	int nguards = 1;
	string modeles[8] = {"Blade","drfreak","garde","Lezard",
						"Marvin","Potator","Samourai","Serpent"};
	srand(time(NULL));

	// lire _data et créer labyrinthe
	for (int i=0; i<LAB_WIDTH; i++) {
		for (int j=0; j<LAB_HEIGHT; j++) {
			switch (_data[i][j])
			{
				case '-':
					createWallHorizontal(i, j, walls, &nwall);
					break;
				case '|':
					createWallVertical(i, j, walls, &nwall);
					break;
				case '+':
					createWallCroix(_data, i, j, walls, &nwall);
					break;
				case 'G':
					createGuard(i, j, guards, &nguards, this, modeles);
					_data[i][j] = EMPTY;
					break;
				case 'C':
					createChasseur(i, j, guards, this);
					_data[i][j] = EMPTY;
					break;
				case 'T':
					treasor._x = i; treasor._y = j;
					break;
				case 'x':
					createBox(i, j, boxes, &nboxes);
					break;
				case 'a':
					createAffiche(_data, i, j, picts, &npicts, l_tmp, 1);
					break;
				case 'b':
					createAffiche(_data, i, j, picts, &npicts, l_tmp, 2);
					break;
				default:
					_data[i][j] = EMPTY;
					break;
			}
		}
	}

	// ** les murs. **
	_nwall = nwall;
	_walls = walls;
	// ** les affiches. **
	_npicts = npicts;
	_picts = picts;
	// prendre les images
	for (int i=0; i<_npicts; i++) {
		char tmp[128];
		if (l_tmp[i]==1)
			sprintf (tmp, "%s/%s", texture_dir, "affiche.jpg");
		if (l_tmp[i]==2)
			sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		_picts[i]._ntex = wall_texture(tmp);
	}
	// ** les caisses. **
	_nboxes = nboxes;
	_boxes = boxes;
	// indiquer qu'on ne marche pas sur une caisse.
	for (int i=0; i<_nboxes; i++)
		_data[_boxes[i]._x][_boxes[i]._y] = 1;
	// ** le trésor. **
	_treasor = treasor;
	_data [_treasor._x][_treasor._y] = 1;	// indiquer qu'on ne marche pas sur le trésor.
	// ** le chasseur et les gardiens. **
	_nguards = nguards;
	_guards = guards;
	// indiquer qu'on ne marche pas sur les gardiens.
	// for (int i=1; i<_nguards; i++)
	// 	_data [(int)(_guards[i] -> _x / scale)][(int)(_guards[i] -> _y / scale)] = 1;

}
