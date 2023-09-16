#include <cmath>
#include <vector>
using namespace std;
#include <fmt/core.h>

#include <SFML/Graphics.hpp>
#include "Util.h"
#include "Scene.h"
#include "ObjetReflechissantLigne.h"
#include "ObjetReflechissantArc.h"
#include "PhaseSpaceDistrib.h"

int main () {

	Rayon traj0 = {
		.orig = { .x = 0.5, .y = 0.51 },
		.dir_angle = -0.1
	};

	// Création des fenêtres SFML
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow win_scene (sf::VideoMode(720,700), "Billard", sf::Style::Close, settings);
	win_scene.setPosition({0,0});
	
	constexpr int PSNpts = 500;
	sf::RenderWindow win_phasespace (sf::VideoMode(PSNpts,PSNpts), "Espace des phases", sf::Style::Titlebar);
	sf::Image phasespace_image;
	phasespace_image.create(PSNpts, PSNpts, sf::Color(0,0,0));
	sf::Texture texture_phasespace;
	texture_phasespace.create(PSNpts, PSNpts);

	// Billard
	Scene scene;

	auto modifier_billard = [&scene,&win_phasespace] (double z) {
		scene.vider();
		constexpr int N = 3;
		std::array<Point2,N> sommets;
		for (int n = 0; n < N; n++) {
			sommets[n] = Point2{
				.x = 0.5 + cos(n*2*M_PI/N)/3,
				.y = 0.5 + sin(n*2*M_PI/N)/3
			};
		}
		for (int n = 0; n < N; n++) {
			scene.créer_objet<ObjetReflechissantArc>( sommets[n], sommets[(n+1)%N], 1/z );
		}
		win_phasespace.setTitle(fmt::format("Espace des phases @ z^2={:.3f}",z*z));
	};
	double z = sqrt(3); // triangle de Reuleaux
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
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::A) {
					z += 0.001;
					modifier_billard(z);
				}
				if (event.key.code == sf::Keyboard::Z) {
					z -= 0.001;
					modifier_billard(z);
				}
				if (event.key.code == sf::Keyboard::Q) 
					traj0.dir_angle -= 0.0001;
				if (event.key.code == sf::Keyboard::S) 
					traj0.dir_angle += 0.0001;
				if (event.key.code == sf::Keyboard::Up) 
					traj0.orig.y += 0.0001;
				if (event.key.code == sf::Keyboard::Down) 
					traj0.orig.y -= 0.0001;
				if (event.key.code == sf::Keyboard::Left) 
					traj0.orig.x -= 0.0001;
				if (event.key.code == sf::Keyboard::Right) 
					traj0.orig.x += 0.0001;		
			}
		}

		// Dessin des objets de la scène
		win_scene.clear(sf::Color::Black);
		scene.dessiner_scene(win_scene);

		// Scanne l'espace des phases en lançant des trajectoires d'origines `traj_orig`
		//  avec `n_angles` angles répartis uniformément dans [-pi,+pi], dans le billard.
		//  Les trajectoires sont limitées en taille à `len_traj`. Leurs points sont
		//  ajoutés à la distribution dans l'espace des phases.
		PhaseSpaceDistrib<PSNpts> ps_expl;

		auto scan_phase_space = [&ps_expl,&scene] (Point2 traj_orig, size_t n_angles, size_t n_origs) {
			Rayon traj0;
			traj0.orig.x = 0.5;

			for (size_t l = 0; l <= n_angles; l++) {
				traj0.dir_angle = l*M_PI/n_angles * 2;

				for (size_t k = 0; k < n_origs; k++) {
					traj0.orig.y = 0.5 + k*0.5/n_origs;

					std::vector<Rayon> traj = scene.propagation_trajectoire(traj0, 1000);
					ps_expl.ajout_traj_dans_distrib(traj, 3, 2);
				}
			}
		};
		scan_phase_space({0.5,0.51}, 15, 15);


		// Trajectoire courante (contrôlée par la souris)
		// Affichage et distribution dans l'espace des phases
		PhaseSpaceDistrib<PSNpts> ps_traj_courante;
		std::vector<Rayon> traj = scene.propagation_trajectoire(traj0, 10000, win_scene, sf::Color(255,0,0,4));
		ps_traj_courante.ajout_traj_dans_distrib(traj, 3, 2);

		// Rayon traj0_dev = traj0;
		// traj0_dev.dir_angle += 0.0001;
		// std::vector<Rayon> traj2 = scene.propagation_trajectoire(traj0_dev, 10000, win_scene, sf::Color(0,0,255,1));

		// traj0_dev.dir_angle += 0.0001;
		// std::vector<Rayon> traj3 = scene.propagation_trajectoire(traj0_dev, 10000, win_scene, sf::Color(0,0,255,1));

		// std::vector<double> dists (traj.size());
		// for (size_t t = 0; t < traj.size(); t++) {
		// 	dists[t] = ( !(traj[t].orig - traj2[t].orig) + !(traj2[t].orig - traj3[t].orig) )/2;
		// }

		// sf::VertexArray lines (sf::LineStrip, 2000);
		// for (size_t t = 0; t < 2000; t++) {
		// 	lines[t] = sf::Vertex(
		// 		coord_01_vers_fenetre({t/2000., 0.8+0.05*log(dists[t])}),
		// 		sf::Color::White
		// 	); 
		// }
		// win_scene.draw(lines);

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

/*		phasespace_image.saveToFile(fmt::format("{:05}.png",iter));
		iter++;
		z += 0.0002;
		modifier_billard(z);*/

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