#include <cmath>
#include <array>
#include <thread>
#include <iostream>
#include <random>
using namespace std;

#include <SFML/Graphics.hpp>
#include "Util.h"

#include "ObjetReflechissantLigne.h"
#include "ObjetReflechissantArc.h"

// Routine d'interception d'un rayon par le premier objet sur le chemin du rayon
std::pair<ObjetReflechissant*,Rayon> interception_réémission (const std::vector<ObjetReflechissant*>& objets, const Rayon& rayon_in) {
	float dist2_min = Inf;
	Rayon rayon_réémis;
	ObjetReflechissant* obj_intercept = nullptr;
	
	for (ObjetReflechissant* obj : objets) {
		ObjetReflechissant::Réémission r = obj->interception_réémission(rayon_in);
		if (r.distance_carrée < dist2_min) {
			dist2_min = r.distance_carrée;
			rayon_réémis = r.rayon_réémis;
			obj_intercept = obj;
		}
	}

	return { obj_intercept, rayon_réémis };
}

int main () {

	Rayon traj0 = {
		.orig = { .x = 0.5, .y = 0.5 },
		.dir_angle = 0
	};

	// Liste des objets de la scène
	std::vector<ObjetReflechissant*> objets;

	// Création du billard
	auto* L1 = new ObjetReflechissantLigne( Point2{0.5f,0.2f}, Point2{0.5f,0.2f} );
	objets.push_back(L1);
	auto* L2 = new ObjetReflechissantLigne( Point2{0.5f,0.8f}, Point2{0.5f,0.8f} );
	objets.push_back(L2);
	auto* A1 = new ObjetReflechissantArc( Point2{0.5f,0.5f}, 0.3, AngleIntervalle(+M_PI/2,-M_PI/2) );
	objets.push_back(A1);
	auto* A2 = new ObjetReflechissantArc( Point2{0.5f,0.5f}, 0.3, AngleIntervalle(-M_PI/2,+M_PI/2) );
	objets.push_back(A2);

	auto modifier_billard = [&] (real_t z) {
		L1->set_extremit_segment( Point2{0.5f-z,0.2f}, Point2{0.5f+z,0.2f} );
		L2->set_extremit_segment( Point2{0.5f-z,0.8f}, Point2{0.5f+z,0.8f} );
		A1->centre = Point2{0.5f+z,0.5f};
		A2->centre = Point2{0.5f-z,0.5f};
	};
	real_t z = 0;
	modifier_billard(z);

	// Création des fenêtres SFML
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow window (sf::VideoMode(720,700), "Billard", sf::Style::Close, settings);
	window.setPosition({0,0});

	while (window.isOpen()) {

		window.clear(sf::Color::Black);
		
		// Gestion des évènements clavier et souris
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
			// Commandes clavier (touches A et Z pour changer la géométrie du billard)
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::A) {
				z += 0.001;
				modifier_billard(z);
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Z) {
				z -= 0.001;
				modifier_billard(z);
			}
		}

		// Dessin des objets de la scène avant propagation
		for (auto obj : objets) 
			obj->dessiner(window);

		// Propagation de la trajectoire
		Rayon traj_courant = traj0;
		sf::Color couleur_traj = sf::Color(255,0,0,25);

		for (int k = 0; k < 1000; k++) {
			// Interception par le premier objet sur le chemin du rayon courant
			auto [obj,traj_next] = interception_réémission(objets, traj_courant);
			if (obj == nullptr) 
				break;

			// Dessin du segment de trajectoire
			sf::VertexArray line (sf::Lines, 2);
			line[0] = sf::Vertex( coord_01_vers_fenetre( traj_courant.orig ), couleur_traj );
			line[1] = sf::Vertex( coord_01_vers_fenetre( traj_next.orig ), couleur_traj );
			window.draw(line);

			traj_courant = traj_next;
		}

		// Dessin de la demi-droite finale
		sf::VertexArray line (sf::Lines, 2);
		line[0] = sf::Vertex( coord_01_vers_fenetre( traj_courant.orig ), couleur_traj );
		Point2 far_away = traj_courant.orig + 10 * Vec2::u_angle(traj_courant.dir_angle);
		line[1] = sf::Vertex( coord_01_vers_fenetre( far_away ), couleur_traj );
		window.draw(line);

		window.display();

		// Délai pour ne pas faire tourner le processeur à fond pour rien
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

	for (auto obj : objets) 
		delete obj;

	return 0;
}