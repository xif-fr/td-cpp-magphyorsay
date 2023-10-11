#include <cmath>
#include <vector>
using namespace std;
//#include <fmt/core.h>

#include <SFML/Graphics.hpp>
#include "Util.h"
#include "Scene.h"
#include "ObjetReflechissantLigne.h"
#include "ObjetReflechissantArc.h"
#include "PhaseSpaceDistrib.h"

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
	
	constexpr int PSNpts = 600;
	sf::RenderWindow win_phasespace (sf::VideoMode(PSNpts,PSNpts), "Espace des phases", sf::Style::Titlebar);
	sf::Image phasespace_image;
	phasespace_image.create(PSNpts, PSNpts, sf::Color(0,0,0));
	sf::Texture texture_phasespace;
	texture_phasespace.create(PSNpts, PSNpts);

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
		// win_phasespace.setTitle(fmt::format("Espace des phases @ z={:.3f}",z));
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
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Q) {
				traj0.dir_angle -= 0.0001;
			}
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::S) {
				traj0.dir_angle += 0.0001;
			}
		}

		// Dessin des objets de la scène
		win_scene.clear(sf::Color::Black);
		scene.dessiner_scene(win_scene);

		// Distribution dans l'espace des phases
		PhaseSpaceDistrib<PSNpts> ps_expl;

		// Scanne l'espace des phases en lançant des trajectoires d'origines `traj_orig`
		//  avec `n_angles` angles répartis uniformément dans [-pi,+pi], dans le billard.
		//  Les trajectoires sont limitées en taille à `len_traj`. Leurs points sont
		//  ajoutés à la distribution dans l'espace des phases.
		{
			const size_t n_angles = 15;
			const size_t n_origs = 15;
			const size_t len_traj = 1000;
			Rayon traj0_scan;
			traj0.orig.x = 0.5;

			for (size_t l = 0; l < n_angles; l++) {
				traj0_scan.dir_angle = l*M_PI/2/n_angles;

				for (size_t k = 0; k < n_origs; k++) {
					traj0_scan.orig.y = 0.5 + k*0.5/n_origs;

					std::vector<Rayon> traj = scene.propagation_trajectoire(traj0_scan, len_traj);
					ps_expl.ajout_traj_dans_distrib(traj);
				}
			}
		}

		// Trajectoire courante (contrôlée par la souris)
		// Affichage et distribution dans l'espace des phases
		PhaseSpaceDistrib<PSNpts> ps_traj_courante;
		std::vector<Rayon> traj = scene.propagation_trajectoire(traj0, 10000, win_scene, sf::Color(255,0,0,2));
		ps_traj_courante.ajout_traj_dans_distrib(traj);

		// Bornage de la valeur de la composante d'une couleur
		auto pixel_color_value_cap = [] (float value) -> sf::Uint8 {
			if (value < 0) return 0;
			else if (value >= 255) return 255;
			else return lround(value);
		};
		
		// Affichage des distributions dans la fenêtre auxiliaire via une image

		for (int i = 0; i < PSNpts; i++) {
			for (int j = 0; j < PSNpts; j++) {

				// L'espace des phases scanné
				sf::Uint8 niveau_gris = pixel_color_value_cap(
					ps_expl.log_phase_space_pixel(i,j) * 50
				);

				// La trajectoire courante en rouge par dessus
				sf::Uint8 rouge = niveau_gris;
				if (ps_traj_courante.phase_space_accumul[i][j] != 0) {
					rouge = pixel_color_value_cap(
						ps_traj_courante.log_phase_space_pixel(i,j) * 30
					);
				}

				phasespace_image.setPixel(i, j, sf::Color(rouge,niveau_gris,niveau_gris));
			}
		}

		texture_phasespace.update(phasespace_image);
		sf::Sprite sprite_phasespace;
		sprite_phasespace.setTexture(texture_phasespace);
		win_phasespace.draw(sprite_phasespace);

		// Affichage des fenêtres
		win_phasespace.display();
		win_scene.display();
	}

	return 0;
}