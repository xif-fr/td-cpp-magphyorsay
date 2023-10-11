#include <cmath>
#include <array>
#include <thread>
#include <iostream>
#include <random>
using namespace std;

#include <SFML/Graphics.hpp>
#include "Util.h"

#include "Scene.h"
#include "ObjetReflechissantLigne.h"
#include "ObjetReflechissantArc.h"

int main () {

	int iter = 0;

	Scene scene; 
	Rayon source = {
		.orig = { .x = 0.5, .y = 0.5 },
		.dir_angle = 0
	};

/*	for (int i = 0; i < 10; ++i) {
		objets.push_back(new ObjetReflechissantLigne( Point2{rand01(),rand01()}, 0.08, rand01()*2*M_PI ));
	}
	for (int i = 0; i < 10; ++i) {
		objets.push_back(new ObjetReflechissantArc( Point2{rand01(),rand01()}, 0.04, AngleIntervalle(+M_PI/2,-M_PI/2)+2*M_PI*rand01() ));
	}*/

/*	auto modifier_billard = [&] (real_t z) {
		scene.vider();
		constexpr int N = 3;
		std::array<Point2,N> sommets;
		for (int n = 0; n < N; n++) {
			sommets[n] = Point2{
				.x = 0.5 + cos(n*2*M_PI/N)/3,
				.y = 0.5 + sin(n*2*M_PI/N)/3
			}
			+ z * Vec2{ rand01(), rand01() };
		}
		for (int n = 0; n < N; n++) {
			scene.creer_objet<ObjetReflechissantLigne>( sommets[n], sommets[(n+1)%N] );
		}
	};
	real_t z = 0.01;
	modifier_billard(z);*/

	auto& L1 = scene.creer_objet<ObjetReflechissantLigne>( Point2{0.5f,0.2f}, Point2{0.5f,0.2f} );
	auto& L2 = scene.creer_objet<ObjetReflechissantLigne>( Point2{0.5f,0.8f}, Point2{0.5f,0.8f} );
	auto& A1 = scene.creer_objet<ObjetReflechissantArc>( Point2{0.5f,0.5f}, 0.3, AngleIntervalle(+M_PI/2,-M_PI/2) );
	auto& A2 = scene.creer_objet<ObjetReflechissantArc>( Point2{0.5f,0.5f}, 0.3, AngleIntervalle(-M_PI/2,+M_PI/2) );

	auto modifier_billard = [&] (real_t z) {
		L1.set_extremit_segment( Point2{0.5f-z,0.2f}, Point2{0.5f+z,0.2f} );
		L2.set_extremit_segment( Point2{0.5f-z,0.8f}, Point2{0.5f+z,0.8f} );
		A1.centre = Point2{0.5f+z,0.5f};
		A2.centre = Point2{0.5f-z,0.5f};
	};
	real_t z = 0;
	modifier_billard(z);

	// Création des fenêtres SFML
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow win_scene (sf::VideoMode(720,700), "Billard", sf::Style::Close, settings);
	win_scene.setPosition({0,0});
	
	sf::RenderWindow win_map (sf::VideoMode(820,800), "Map");
	sf::Image map;
	map.create(800, 800, sf::Color(0,0,0));
	sf::Texture texture_map;
	texture_map.create(800, 800);

	while (win_scene.isOpen()) {
		
		// Gestion des évènements clavier et souris
		sf::Event event;
		while (win_scene.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				win_scene.close();

			// Souris
			if (event.type == sf::Event::MouseMoved) {
				Point2 mouse = coord_fenetre_vers_01(sf::Vector2f(event.mouseMove.x,event.mouseMove.y));
				Vec2 v = mouse - source.orig;
				source.dir_angle = atan2f(v.y, v.x);
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				Point2 mouse = coord_fenetre_vers_01(sf::Vector2f(event.mouseButton.x,event.mouseButton.y));
				source.orig = mouse;
				iter = 0;
			}
			// Commandes clavier
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::A) {
				z += 0.001;
				modifier_billard(z);
				iter = 0;
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Z) {
				z -= 0.001;
				modifier_billard(z);
				iter = 0;
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Q) {
				source.dir_angle -= 0.0001;
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::S) {
				source.dir_angle += 0.0001;
			}
		}
		
		if (iter <= 800) {
			source.dir_angle = iter / 800. * M_PI/2;
			iter++;
		}

		// Dessin des objets de la scène avant propagation
		win_scene.clear(sf::Color::Black);

		scene.dessiner_scene(win_scene);

		std::array< std::vector<Rayon>, 11 > trajs;
		Rayon source_temp = source;
		for (int i = 0; i < (int)trajs.size(); i++) {
	//		modifier_billard(z);
			trajs[i] = scene.propagation_trajectoire(source_temp, 1000, win_scene, sf::Color(25*i,0,250-25*i,5));
			source_temp.dir_angle += 0.0001;
		}

/*		source.dir_angle += 0.00001 * rand01();
		scene.propagation_trajectoire(source, 10000, win_scene, sf::Color(0, 255, 0, 2));
		source.dir_angle += 0.00001 * rand01();
		scene.propagation_trajectoire(source, 10000, win_scene, sf::Color(0, 0, 255, 2));
		source.dir_angle -= 0.00002 * rand01();*/

		// Affichage SFML
		win_scene.display();

		int x = lround( angle_mod2pi_02(source.dir_angle)/2/M_PI * 800 * 4 );
		if (x < 800) {
			std::array<int,800> distrib;
			distrib.fill(0);
			for (auto& traj : trajs) {
				for (Rayon& ray : traj) {
					int y = lround( angle_mod2pi_02(ray.dir_angle)/2/M_PI * 800 ) % 800;
					distrib[y]++;
				}
			}
			for (unsigned int y = 0; y < 800; y++) {
				float l = (log((float)distrib[y]) - 1) * 40;
				sf::Uint8 c;
				if (l < 0) c = 0;
				else if (l >= 255) c = 255;
				else c = lround(l);
				map.setPixel(x, y, sf::Color(c,c,c));
			}

			texture_map.update(map);
			sf::Sprite sprite_map;
			sprite_map.setTexture(texture_map);
			win_map.draw(sprite_map);
			sf::VertexArray current_line (sf::Lines, 2);
			current_line[0] = sf::Vertex( sf::Vector2f(x,0), sf::Color::Blue );
			current_line[1] = sf::Vertex( sf::Vector2f(x,800), sf::Color::Blue );
			win_map.draw(current_line);
			win_map.display();
		}

	}

	return 0;
}