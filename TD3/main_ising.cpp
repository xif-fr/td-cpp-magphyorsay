#include "ising.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace std;

int main () {

	float T_sur_Tc;
	std::cout << "T/Tc ?" << std::endl;
	std::cin >> T_sur_Tc;
	//float β = log(1+sqrt(2))/2 / T_sur_Tc;  // réseau carré
	float β = log(sqrt(2+sqrt(3))) / T_sur_Tc;  // réseau hexagonal
	std::cout << "B ?" << std::endl;
	float B;
	std::cin >> B;

	std::ofstream fichier_r ("ising_magnétisation_énergie_txaccept.txt");

	Réseau S (160, 180);
	ising_init_alea(S);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	sf::Vector2f winsz = S.taille_affichage_SFML();
	sf::RenderWindow window (sf::VideoMode(winsz.x,winsz.y), "Ising", sf::Style::Close, settings);
	std::cout << std::fixed << std::setprecision(4);
	fichier_r << std::fixed << std::setprecision(4);

	int steps = 0;
	int steps_accept = 0;

	while (steps < 1000000000) {

		constexpr int recurr_stats = 20000;
		if (steps%recurr_stats == 0) {

			auto [m, e] = ising_magnétisation_énergie(S, B);
			fichier_r << steps << " " << m << " " << e << " " << steps_accept/(float)recurr_stats << std::endl;
			steps_accept = 0;

			if (steps%(recurr_stats*100) == 0) {

				//S.affiche_console();
				std::cout << "m = " << m << ", e = " << e << std::endl;

				window.clear(sf::Color::White);
				S.affiche_SFML(window, 0, 0);
				window.display();
			}
		}

		steps_accept += ising_metropolis_step(S, β, B);
		steps++;

		/* 
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					case sf::Keyboard::B:  B += 0.001; break;
					case sf::Keyboard::V:  B -= 0.001; break;
					case sf::Keyboard::A:  β *= 0.995; break;
					case sf::Keyboard::Z:  β *= 1.005; break;
					default: break;
				}
			}
		}
		*/

	}

	return 0;
}
