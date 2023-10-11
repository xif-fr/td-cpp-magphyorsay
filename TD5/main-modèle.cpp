#include <cmath>
using namespace std;
#include <SFML/Graphics.hpp>
#include "Util.h"
#include "ObjetReflechissantLigne.h"

int main () {

	// Le segment
	ObjetReflechissantLigne obj = ObjetReflechissantLigne( Point2{0.7,0.2}, Point2{0.5,0.8} );

	// Le rayon
	Rayon traj0 = {
		.orig = { .x = 0.5, .y = 0.5 },
		.dir_angle = 0
	};

	// Création de la fenêtre SFML
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow window (sf::VideoMode(720,700), "Billard", sf::Style::Close, settings);

	while (window.isOpen()) {

		window.clear(sf::Color::Black);
		
		// Gestion des évènements
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			// L'orientation du rayon suit la souris
			if (event.type == sf::Event::MouseMoved) {
				Point2 mouse = coord_fenetre_vers_01(sf::Vector2f(event.mouseMove.x,event.mouseMove.y));
				Vec2 v = mouse - traj0.orig;
				traj0.dir_angle = atan2(v.y, v.x);
			}
			// L'origine du rayon change lorsque l'on clique
			if (event.type == sf::Event::MouseButtonReleased) {
				Point2 mouse = coord_fenetre_vers_01(sf::Vector2f(event.mouseButton.x,event.mouseButton.y));
				traj0.orig = mouse;
			}
		}

		// Cœur du progamme et dessin		
		// ...

		// Affichage SFML
		window.display();
	}

	return 0;
}