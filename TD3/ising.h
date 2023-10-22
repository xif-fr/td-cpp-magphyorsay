#ifdef RESEAU_HEXAGONAL
#include "reseau_hexa.h"
#else
#include "reseau_carre.h"
#endif

#include <utility>

#ifndef __ISING_H__
#define __ISING_H__

// Algorithme de Metropolis avec une inversion d'un spin aléatoire
bool ising_metropolis_step (Réseau& S, float β, float B);

// Calcul de la magnétisation et de l'énergie (intensive) d'une configuration
std::tuple<float,float> ising_magnétisation_énergie (const Réseau& S, float B);

// Configuration aléatoire
void ising_init_alea (Réseau& S);

#endif // __ISING_H__
