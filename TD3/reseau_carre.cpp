#include "reseau_carre.h"

/* Construction et destruction */

Réseau::Réseau (int nx, int ny)
	: nx(nx), ny(ny), ntot(nx*ny) 
{
	data = new val_t[nx*ny];
}

Réseau::Réseau (int nx, int ny, val_t valeur_def)
	: Réseau(nx,ny)
{
	for (int i = 0; i < ntot; i++)
		data[i] = valeur_def;
}

Réseau::Réseau (const Réseau& o)
	: Réseau(o.nx, o.ny)
{
	for (int i = 0; i < ntot; i++)
		data[i] = o.data[i];
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
	// (x,y) quelconque -> (x,y) replié
	x = x % nx;
	if (x < 0) x += nx;
	y = y % ny;
	if (y < 0) y += ny;
	return site_xy_brut(x, y);
}

Réseau::Site Réseau::site_xy_brut (int x, int y) const {
	// (x,y) replié -> indice replié
	int index = x + nx * y;
	return Site(index, x, y);
}

#include <random>
std::mt19937 rng_reseau;

Réseau::Site Réseau::site_aleatoire () const {
	return site_index( rng_reseau() % ntot );
}

/* Accès à la valeur d'un site */

Réseau::val_t Réseau::operator[] (Site s) const {
	return data[s._index];
}

Réseau::val_t& Réseau::operator[] (Site s) {
	return data[s._index];
}

Réseau::val_t Réseau::operator[] (int x, int y) const {
	return data[site_xy(x,y)._index];
}

Réseau::val_t& Réseau::operator[] (int x, int y) {
	return data[site_xy(x,y)._index];
}

/* Itérateur sur les sites */

Réseau::sites_iterator_t::sites_iterator_t (const Réseau& r)
	: r(r), s(0,0,0)
{}

Réseau::sites_iterator_t Réseau::sites_iterator_t::operator++ () {
	s._index++;
	s._x++;
	if (s._x == r.nx) {
		s._x = 0;
		s._y++;
	}
	return *this;
}

bool Réseau::sites_iterator_t::operator!= (Réseau::sites_iterator_end_t) const {
	return s._index != r.ntot;
}

/* Voisinage d'un site */

std::array<Réseau::Site,4> Réseau::voisins (Site s) const {
	return {
		site_xy( s._x+1, s._y   ),
		site_xy( s._x,   s._y-1 ),
		site_xy( s._x-1, s._y   ),
		site_xy( s._x,   s._y+1 ),
	};
}

/*std::array<Réseau::Site,4> Réseau::voisins (Site s) const {
	// version ~15% plus performante que celle ci-dessus
	return {
		site_xy_brut( (s._x+1)%nx,    s._y          ),
		site_xy_brut(  s._x,         (s._y-1+ny)%ny ),
		site_xy_brut( (s._x-1+nx)%nx, s._y          ),
		site_xy_brut(  s._x,         (s._y+1)%ny    ),
	};
}*/

/* Affichage du système */

#include <iostream>

void Réseau::affiche_console () const {
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			val_t v = data[ site_xy(x,y)._index ];
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
	sq.setSize(sf::Vector2f(taille_site_affichage_SFML,taille_site_affichage_SFML));

	for (int x = 0; x < nx; x++) {
		for (int y = 0; y < ny; y++) {
			val_t v = (*this)[x,y];
			sq.setPosition(sf::Vector2f(
				pos_x + taille_site_affichage_SFML * x,
				pos_y + taille_site_affichage_SFML * (ny - y - 1)
			));
			if      (v > 0) sq.setFillColor(sf::Color::White);
			else if (v < 0) sq.setFillColor(sf::Color::Black);
			else            sq.setFillColor(sf::Color(127,127,127));
			window.draw(sq);
		}
	}
}

sf::Vector2f Réseau::taille_affichage_SFML () const {
	return sf::Vector2f( taille_site_affichage_SFML*nx, taille_site_affichage_SFML*ny );
}

/* Décimation (aggrégation de plaquettes et détermination de la valeur majoritaire) */

#include <map>
#include <algorithm>

Réseau Réseau::decimate () const {
	Réseau S_decim (nx / 3, ny / 3);
	for (int x_decim = 0; x_decim < S_decim.nx; x_decim++) {
		for (int y_decim = 0; y_decim < S_decim.ny; y_decim++) {

			val_t sum = 0;
			for (int dx = -1; dx <= +1; dx++) {
				for (int dy = -1; dy <= +1; dy++) {
					int x = 3 * x_decim + dx + 1;
					int y = 3 * y_decim + dy + 1;
					val_t v = (*this)[site_xy_brut(x,y)];
					sum += v;
				}
			}
			val_t maj_val = (sum > 0) ? 1 : -1;
			S_decim[x_decim,y_decim] = maj_val;
		}
	}
	return S_decim;
}

/*Réseau Réseau::decimate () const {
	Réseau S_decim (nx / 2, ny / 2);
	for (int x_decim = 0; x_decim < S_decim.nx; x_decim++) {
		for (int y_decim = 0; y_decim < S_decim.ny; y_decim++) {

			std::map< val_t, uint8_t > counters;
			//val_t sum = 0;
			for (int dx = 0; dx <= +1; dx++) {
				for (int dy = 0; dy <= +1; dy++) {
					int x = 2 * x_decim + dx;
					int y = 2 * y_decim + dy;
					val_t v = (*this)[site_xy_brut(x,y)];
					if (v != 0) {
						counters.insert({v, 0});
						counters[v]++;
					}
					//sum += v;
				}
			}
			val_t maj_val = 0;
			uint8_t maj_cnt = 0, min_cnt = -1;
			for (auto [val,cnt] : counters) {
				if (cnt > maj_cnt) {
					maj_val = val;
					maj_cnt = cnt;
				}
				if (cnt < min_cnt) 
					min_cnt = cnt;
			}
			if (counters.size() > 1 and maj_cnt == min_cnt) {
				// pas de majoritaire
				maj_val = 0;
			}
			//val_t domin_value = (sum > 0) ? 1 : -1;
			S_decim[x_decim,y_decim] = maj_val;
		}
	}
	return S_decim;
}*/

