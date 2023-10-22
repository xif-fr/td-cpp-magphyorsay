#include <array>
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>

#ifndef __RESEAU_HEXA_H__
#define __RESEAU_HEXA_H__

class Réseau {
public:

	// type opaque
	class Site {
	private:
		int _index;
		int _x;
		int _y;
		bool _a;

		Site (int i, int x, int y, bool a) : _index(i), _x(x), _y(y), _a(a) {}
		Site () = default;
		friend class Réseau;
	};

	const int nx;
	const int ny;

private:

	int8_t* data;
	const int ntot;

	Site site_index (int index) const;

public:

	Site site_xy (int x, int y, bool a) const;
	Site site_aleatoire () const;

	int8_t operator[] (Site) const;
	int8_t& operator[] (Site);

	void for_each (std::function<void(Site)>) const;
	void for_each (std::function<int8_t(Site)>);

	std::array<Site,3> voisins (Site s) const;

	Réseau (int nx, int ny);
	Réseau (int nx, int ny, int8_t valeur_def);
	Réseau (const Réseau&) = delete;
	~Réseau ();

	void affiche_console () const;
	void affiche_SFML (sf::RenderWindow& window, float pos_x, float pos_y) const;
	sf::Vector2f taille_affichage_SFML () const;
};

using Site = Réseau::Site;

#endif // __RESEAU_HEXA_H__
