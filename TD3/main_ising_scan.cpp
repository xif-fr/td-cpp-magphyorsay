#include "ising.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace std;

int main () {

	Réseau S (200, 200);
	ising_init_alea(S);

	sf::Vector2f winsz = S.taille_affichage_SFML();
	sf::RenderWindow window (sf::VideoMode(winsz.x,winsz.y), "Ising", sf::Style::Close);

	for (int k = 0; k <= 2; k++) {

		float T_sur_Tc = 1.2;
		float β = log(1+sqrt(2))/2 / T_sur_Tc;
		float B = -0.2 + 2*0.2 * k/40.;

		std::cout << "---------" << std::endl;
		std::cout << "T = " << T_sur_Tc << ", B = " << B << std::endl;

		std::string nom_fichier = std::string("scan_B_T1.2_up/ising_magnétisation_énergie.") + std::to_string(k) + ".txt";
		std::ofstream fichier_r (nom_fichier.c_str());

		std::cout << std::fixed << std::setprecision(4);
		fichier_r << std::fixed << std::setprecision(4);

		size_t steps = 0;

		while (steps < 500000000) {

			constexpr int recurr_stats = 100000;
			if (steps%recurr_stats == 0) {

				auto [m, e] = ising_magnétisation_énergie(S, B);
				fichier_r << steps << " " << m << " " << e << std::endl;

				if (steps%(recurr_stats*100) == 0) {

					//S.affiche_console();
					std::cout << "m = " << m << ", e = " << e << std::endl;

					window.clear(sf::Color::White);
					S.affiche_SFML(window, 0, 0);
					window.display();
				}
			}

			ising_metropolis_step(S, β, B);
			steps++;

		}

	}

	return 0;
}
