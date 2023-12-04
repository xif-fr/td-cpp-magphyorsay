#include "reseau_hexa.h"

/* Construction et destruction */

Réseau::Réseau (int nx, int ny) : nx(nx), ny(ny), ntot(nx*ny*2) {
	data = new int8_t[ntot];
}

Réseau::Réseau (int nx, int ny, int8_t valeur_def) : Réseau(nx,ny) {
	for (int i = 0; i < ntot; i++)
		data[i] = valeur_def;
}

Réseau::~Réseau () {
	delete[] data;
}

/* Indiçage des site */

// ▲▼▲▼▲▼▲▼▲▼    cell = ▲▼
//  ▲▼▲▼▲▼▲▼▲▼
//   ▲▼▲▼▲▼▲▼▲▼  ↘︎y x→

Réseau::Site Réseau::site_index (int index) const {
	// index replié -> (x,y,a)

	bool a = index % 2; // 1 if left triangle, 0 if right
	uint16_t i_cell = index / 2;

	return Site(
		index,
		i_cell % nx,
		i_cell / nx,
		a
	);
}

Réseau::Site Réseau::site_xy (int x, int y, bool a) const {
	// (x,y,a) quelconque -> (x,y,a) replié
	x = x % nx;
	if (x < 0) x += nx;
	y = y % ny;
	if (y < 0) y += ny;
	return site_xy_brut(x, y, a);
}

Réseau::Site Réseau::site_xy_brut (int x, int y, bool a) const {
	// (x,y,a) replié -> indice replié
	int index = 2*( x + nx * y ) + a;
	return Site(index, x, y, a);
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

/* Voisinage d'un site */

std::array<Réseau::Site,3> Réseau::voisins (Site s) const {
	int x = s._x;
	int y = s._y;

	if (s._a) { // left triangle ▼
		Site r = site_xy_brut( (x+1)%nx, y, false );
		Site l = site_xy_brut( x, y, false );
		if (y == 0) { y = ny-1; x = (x-ny/2+nx)%nx; }
		else y = y-1;
		Site u = site_xy_brut( (x+1)%nx, y, false );
		return {r,u,l};
	}
	else { // right triangle ▲
		Site l = site_xy_brut( (x-1+nx)%nx, y, true );
		Site r = site_xy_brut( x, y, true );
		if (y == ny-1) { y = 0; x = (x+ny/2)%nx; }
		else y = y+1;
		Site d = site_xy_brut( (x-1+nx)%nx, y, true );
		return {r,d,l};
	}
}

/* Itérateur sur les sites */

Réseau::sites_iterator_t::sites_iterator_t (const Réseau& r)
	: r(r), s(r.site_index(0))
{}

Réseau::sites_iterator_t Réseau::sites_iterator_t::operator++ () {
	s = r.site_index(s._index + 1);
	// pourrait être écrit de façon plus performant en incrémentant manuellement x, y, a
	return *this;
}

bool Réseau::sites_iterator_t::operator!= (Réseau::sites_iterator_end_t) const {
	return s._index != r.ntot;
}

/* Affichage du système */

#include <SFML/Graphics.hpp>

constexpr float trilatt_draw_spin_sz = 4;

void Réseau::affiche_SFML (sf::RenderWindow& window, float pos_x, float pos_y) const {

	sf::ConvexShape tri_up;
	tri_up.setPointCount(3);
	tri_up.setPoint(0, sf::Vector2f(0,0));
	tri_up.setPoint(1, sf::Vector2f(0.5,sqrt(3)/2));
	tri_up.setPoint(2, sf::Vector2f(1,0));
	tri_up.setScale(trilatt_draw_spin_sz, trilatt_draw_spin_sz);
	sf::ConvexShape tri_down;
	tri_down.setPointCount(3);
	tri_down.setPoint(0, sf::Vector2f(0.5,sqrt(3)/2));
	tri_down.setPoint(1, sf::Vector2f(1,0));
	tri_down.setPoint(2, sf::Vector2f(1.5,sqrt(3)/2));
	tri_down.setScale(trilatt_draw_spin_sz, trilatt_draw_spin_sz);

	for (int i = 0; i < ntot; i++) {
		Site s = site_index(i);
		int8_t v = data[i];
		float x = (2*s._x + s._y)%(2*nx) /2. * trilatt_draw_spin_sz;
		float y = (ny - s._y - 1.f) * (sqrt(3)/2) * trilatt_draw_spin_sz;
		sf::ConvexShape& tri = s._a ? tri_down : tri_up;
		tri.setPosition(pos_x+x, pos_y+y);
		if      (v > 0) tri.setFillColor(sf::Color::White);
		else if (v < 0) tri.setFillColor(sf::Color::Black);
		else            tri.setFillColor(sf::Color(127,127,127));
		window.draw(tri);
	}

}

sf::Vector2f Réseau::taille_affichage_SFML () const {
	return sf::Vector2f( trilatt_draw_spin_sz * (nx+1), trilatt_draw_spin_sz * ny * sqrt(3)/2 );
}

/* Décimation (aggrégation de plaquettes et détermination de la valeur majoritaire) */


