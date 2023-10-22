#include <iostream>
#include "reseau.h"

int main () {

	Réseau S (20, 20, 0);

	for (int x = -5; x < 8; x++) {
		S[x,3] = -1;
	}
	for (int x = -5; x < 8; x++) {
		S[x,-6] = 1;
	}
	for (int y = 18; y <= 23; y++) {
		S[8,y] = 1;
	}
	
	S.affiche_console();
	std::cout << std::endl;

	std::cout << "Voisins de (0,0) :" << std::endl;
	Site i = S.site_xy(0,0);
	for (Site j : S.voisins(i))
		std::cout << "(" << j.x() << "," << j.y() << ")" << std::endl;

	return 0;
}
