#include <array>
#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>

#ifndef __RESEAU_CARRE_H__
#define __RESEAU_CARRE_H__

class Réseau {
public:

	// Type opaque stockant les coordonnées pré-calculées d'un site
	class Site {
	private:
		int _index;
		int _x;
		int _y;

		Site (int i, int x, int y) : _index(i), _x(x), _y(y) {}
		Site () = default;
		friend class Réseau;

	public:
		int x () const { return _x; }
		int y () const { return _y; }
	};

	// Taille de la matrice de site
	const int nx;
	const int ny;

private:

	// Zone mémoire stockant la matrice repliée
	int8_t* data;
	// Nombre total de sites (0 <= _index < ntot)
	const int ntot;

	// Revoie l'objet Site correspondant à l'indice `index` de la matrice repliée
	Site site_index (int index) const;
	// Revoie l'objet Site correspondant au site de coordonnées (x,y), avec 0 <= x,y < nx,ny, sans vérifications
	Site site_xy_brut (int x, int y) const;

public:

	// Revoie l'objet Site correspondant au site de coordonnées (x,y),
	// en tenant compte des conditions périodiques aux limites
	Site site_xy (int x, int y) const;
	// Revoie l'objet Site correspondant à un site choisi aléatoirement
	Site site_aleatoire () const;

	// Accès à la valeur d'un site
	int8_t operator[] (Site) const;
	int8_t& operator[] (Site);
	int8_t operator[] (int x, int y) const;
	int8_t& operator[] (int x, int y);

	// Lite des sites directement voisins d'un site `s`
	std::array<Site,4> voisins (Site s) const;

	// Construction et destruction
	Réseau (int nx, int ny);
	Réseau (int nx, int ny, int8_t valeur_def);
	Réseau (const Réseau&) = delete;
	~Réseau ();

	// Affichage textuel et graphique (SFML)
	void affiche_console () const;
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
	sites_iterator_t begin () const { return sites_iterator_t(*this); } // itérateur de début
	sites_iterator_end_t end () const { return sites_iterator_end_t(); } // itérateur de fin
};

// Pour éviter d'avoir à tapper `Réseau::Site` à chaque fois
using Site = Réseau::Site;

#endif // __RESEAU_CARRE_H__
