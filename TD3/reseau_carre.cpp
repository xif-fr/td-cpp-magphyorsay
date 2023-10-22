#include "reseau_carre.h"

/* Construction et destruction */

Réseau::Réseau (int nx, int ny) : nx(nx), ny(ny), ntot(nx*ny) {
	data = new int8_t[nx*ny];
}

Réseau::Réseau (int nx, int ny, int8_t valeur_def) : Réseau(nx,ny) {
	for (int i = 0; i < ntot; i++)
		data[i] = valeur_def;
}

Réseau::~Réseau () {
	delete[] data;
}

/* Indiçage des site */

Réseau::Site Réseau::site_index (int index) const {
	// index replié -> (x,y)
	int x = index % nx;
	int y = index / nx;
	return Site(index, x, y);
}

Réseau::Site Réseau::site_xy (int x, int y) const {
	// (x,y) quelconque -> (x,y) replié -> indice replié
	x = x % nx;
	if (x < 0) x += nx;
	y = y % ny;
	if (y < 0) y += ny;
	int index = x + nx * y;
	return Site(index, x, y);
}

#include <random>
std::mt19937 rng_reseau;

Réseau::Site Réseau::site_aleatoire () const {
	return site_index( rng_reseau() % ntot );
}

/* Accès à la valeur d'un site */

int8_t Réseau::operator[] (Site s) const {
	return data[s._index];
}

int8_t& Réseau::operator[] (Site s) {
	return data[s._index];
}

int8_t Réseau::operator[] (int x, int y) const {
	return data[site_xy(x,y)._index];
}

int8_t& Réseau::operator[] (int x, int y) {
	return data[site_xy(x,y)._index];
}

void Réseau::for_each (std::function<void(Site)> func) const {
	for (int i = 0; i < ntot; i++) 
		func( site_index(i) );
}

void Réseau::for_each (std::function<int8_t(Site)> func) {
	for (int i = 0; i < ntot; i++) 
		data[i] = func( site_index(i) );
}

/* Voisinage d'un site */

/*std::array<Réseau::Site,4> Réseau::voisins (Site s) const {
	return {
		site_xy_brut( (s._x+1)%nx,    s._y          ),
		site_xy_brut(  s._x,         (s._y-1+ny)%ny ),
		site_xy_brut( (s._x-1+ny)%nx, s._y          ),
		site_xy_brut(  s._x,         (s._y+1)%ny    ),
	};
}*/

std::array<Réseau::Site,4> Réseau::voisins (Site s) const {
	return {
		site_xy( s._x+1, s._y   ),
		site_xy( s._x,   s._y-1 ),
		site_xy( s._x-1, s._y   ),
		site_xy( s._x,   s._y+1 ),
	};
}

/* Affichage du système */

#include <iostream>

void Réseau::affiche_console () const {
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			int8_t v = data[ site_xy(x,y)._index ];
			if (v < 0)
				std::cout << "■";
			else if (v > 0)
				std::cout << "□";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}
}

#include <SFML/Graphics.hpp>

void Réseau::affiche_SFML (sf::RenderWindow& window, float pos_x, float pos_y) const {

	// rectangle de 2x2 pixels
	sf::RectangleShape sq;
	sq.setSize(sf::Vector2f(2,2));

	for (int x = 0; x < nx; x++) {
		for (int y = 0; y < ny; y++) {
			int8_t v = data[ site_xy(x,y)._index ];
			sq.setPosition(sf::Vector2f(
				pos_x + 2 * x,
				pos_y + 2 * (ny - y - 1)
			));
			sq.setFillColor( (v>0) ? sf::Color::White : sf::Color::Black);
			window.draw(sq);
		}
	}
}

sf::Vector2f Réseau::taille_affichage_SFML () const {
	return sf::Vector2f( 2*nx, 2*ny );
}
