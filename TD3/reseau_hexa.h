#include <array>
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>

#ifndef __RESEAU_HEXA_H__
#define __RESEAU_HEXA_H__

class Réseau {
public:

	// Type opaque stockant les coordonnées pré-calculées d'un site
	class Site {
	private:
		int _index;
		int _x; // coord x dans le réseau de Bravais (0 <= x < nx)
		int _y; // coord y dans le réseau de Bravais (0 <= y < nx)
		bool _a; // quel réseau de Bravais (a ou b)

		Site (int i, int x, int y, bool a) : _index(i), _x(x), _y(y), _a(a) {}
		Site () = default;
		friend class Réseau;
	};

	// Taille de la matrice de site
	// Note : il y a en réalité 2 ⨉ nx sites horizontalement car deux sites par point du réseau de Bravais
	const int nx;
	const int ny;

private:

	// Zone mémoire stockant la matrice repliée
	int8_t* data;
	// Nombre total de sites (0 <= _index < ntot), ici 2*nx*ny
	const int ntot;

	// Revoie l'objet Site correspondant à l'indice `index` de la matrice repliée
	Site site_index (int index) const;
	// Revoie l'objet Site correspondant au site de coordonnées (x,y), avec 0 <= x,y < nx,ny, sans vérifications
	Site site_xy_brut (int x, int y, bool a) const;

public:

	// Revoie l'objet Site correspondant au site de coordonnées (x,y) et sur le sous réseau indiqué par `a`,
	// en tenant compte des conditions périodiques aux limites
	Site site_xy (int x, int y, bool a) const;
	// Revoie l'objet Site correspondant à un site choisi aléatoirement
	Site site_aleatoire () const;

	// Accès à la valeur d'un site
	int8_t operator[] (Site) const;
	int8_t& operator[] (Site);

	// Lite des sites directement voisins d'un site `s`
	std::array<Site,3> voisins (Site s) const;

	// Construction et destruction
	Réseau (int nx, int ny);
	Réseau (int nx, int ny, int8_t valeur_def);
	Réseau (const Réseau&) = delete;
	~Réseau ();

	// Affichage graphique (SFML)
	void affiche_SFML (sf::RenderWindow& window, float pos_x, float pos_y) const;
	sf::Vector2f taille_affichage_SFML () const;

	// Itérateur de sites pour les range-based loops :
public:
	class sites_iterator_end_t {};
	class sites_iterator_t {
		private:
			const Réseau& r;
			Site s; // site courant
		public:
			sites_iterator_t (const Réseau&); // itérateur de début
			sites_iterator_t operator++ (); // passage au site suivant
			bool operator!= (sites_iterator_end_t) const; // vérification de fin d'itération
			const Site& operator* () const { return s; } // récupération du site
	};
	sites_iterator_t begin () const { return sites_iterator_t(*this); }
	sites_iterator_end_t end () const { return sites_iterator_end_t(); }
};

using Site = Réseau::Site;

#endif // __RESEAU_HEXA_H__
