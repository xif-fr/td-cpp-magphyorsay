#include <cmath>
#include <vector>
#include <thread>
using namespace std;
//#include <fmt/core.h>

#include <SFML/Graphics.hpp>
#include "Util.h"
#include "Scene.h"
#include "ObjetReflechissantLigne.h"
#include "ObjetReflechissantArc.h"

int main () {

	Rayon traj0 = {
		.orig = { .x = 0.5, .y = 0.51 },
		.dir_angle = 0
	};

	// Création des fenêtres SFML
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow win_scene (sf::VideoMode(720,700), "Billard", sf::Style::Close, settings);
	win_scene.setPosition({0,0});

	// Billard
	Scene scene;

	auto& L1 = scene.créer_objet<ObjetReflechissantLigne>( Point2{0.5,0.2}, Point2{0.5,0.2} );
	auto& L2 = scene.créer_objet<ObjetReflechissantLigne>( Point2{0.5,0.8}, Point2{0.5,0.8} );
	auto& A1 = scene.créer_objet<ObjetReflechissantArc>( Point2{0.5,0.5}, 0.3, AngleIntervalle(+M_PI/2,-M_PI/2) );
	auto& A2 = scene.créer_objet<ObjetReflechissantArc>( Point2{0.5,0.5}, 0.3, AngleIntervalle(-M_PI/2,+M_PI/2) );

	auto modifier_billard = [&] (double z) {
		L1.set_extremit_segment( Point2{0.5-z,0.2}, Point2{0.5+z,0.2} );
		L2.set_extremit_segment( Point2{0.5-z,0.8}, Point2{0.5+z,0.8} );
		A1.centre = Point2{0.5+z,0.5};
		A2.centre = Point2{0.5-z,0.5};
	};
	double z = +0.1;
	modifier_billard(z);

	while (win_scene.isOpen()) {
		
		// Gestion des évènements clavier et souris
		sf::Event event;
		while (win_scene.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				win_scene.close();

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

			// Commandes clavier
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::A) {
				z += 0.001;
				modifier_billard(z);
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Z) {
				z -= 0.001;
				modifier_billard(z);
			}
		}

		// Dessin des objets de la scène
		win_scene.clear(sf::Color::Black);
		scene.dessiner_scene(win_scene);

		// Dessin de la trajectoire
		scene.propagation_trajectoire(traj0, 2000, win_scene, sf::Color(255,0,0,10));

		win_scene.display();

		// Délai pour ne pas faire tourner le processeur à fond pour rien
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

	return 0;
}