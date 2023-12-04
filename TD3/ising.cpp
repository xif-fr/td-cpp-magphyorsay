#include "ising.h"
#include <random>

std::random_device rd;
std::mt19937 rng (rd());
std::uniform_real_distribution<float> distrib_u01 (0,1);

/* Algorithme de Metropolis avec une inversion d'un spin aléatoire ("algorithme local") */

bool ising_metropolis_step (Réseau& S, float β, float B) {
	// choix d'un site aléatoire
	Site i = S.site_aleatoire();

	// calcul du champ local
	int8_t b = 0;
	for (Site j : S.voisins(i))
		b += S[j];

	// différence d'énergie locale sous inversion du spin
	float ΔE = 2 * S[i] * (B + b);

	// inversion du spin avec probabilité d'acceptation
	float r_flip = exp( -β * ΔE );

	if (distrib_u01(rng) < r_flip) {
		S[i] = -S[i];
		return true; // spin inversé (transition acceptée)
	}
	else
		return false;
}

/* Algorithme de Metropolis-Hastings avec une nouvelle valeur de spin choisie pour ne pas avoir de rejet ("heat bath algorithm") */

bool ising_heat_bath_step (Réseau& S, float β, float B) {
	Site i = S.site_aleatoire();

	// calcul du champ local
	int8_t b = 0;
	for (Site j : S.voisins(i))
		b += S[j];

	// proba up
	float p_up = 1 / ( 1 + exp(-2 * β * (B + b)) );

	// set spin up or down
	if (distrib_u01(rng) < p_up) 
		S[i] = +1;
	else
		S[i] = -1;

	return true;
}

/* Calcul de la magnétisation et de l'énergie (intensive) d'une configuration */

std::tuple<float,float> ising_magnétisation_énergie (const Réseau& S, float B) {
	int M = 0;
	float E = 0;

	// boucle for range-based
	// sous le capot, utilise l'itérateur sites_iterator_t et Réseau::begin(), Réseau::end()
	for (Site i : S) {
		// magnétisation :
		M += S[i];
		// énergie :
		for (Site j : S.voisins(i)) 
			E -= S[i] * S[j] / 2.f;
		E -= S[i] * B;
	}

	int ntot = S.nx * S.ny;
	return { M /(float) ntot, E / ntot };
}

/* Configuration aléatoire */

void ising_init_alea (Réseau& S) {
	for (Site i : S) {
		S[i] = 2 * (rng()%2) - 1;
	}
}
