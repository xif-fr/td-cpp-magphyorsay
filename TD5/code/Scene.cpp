#include "Scene.h"
#include <cmath>
#include <SFML/Graphics.hpp>

// Test d'interception du rayon contre toutes les objets de la scène puis ré-émission; la première
//  interception sur le trajet du rayon depuis son origine est choisie. Le premier élément de la paire
//  est un pointeur sur l'objet interceptant le rayon (nullptr si il n’y a aucune interception),
//  et le deuxième élément sera la rayon éventuellement ré-émis.
//
std::pair<ObjetReflechissant*,Rayon> Scene::interception_réémission (const Rayon& rayon_in) const {
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

// Caclul d'une trajectoire sur la scène : récursion de l'interception/ré-émission
// Renvoie la trajectoire sous forme d'un tableau de rayons
//
std::vector<Rayon> Scene::propagation_trajectoire (Rayon rayon, size_t limite_taille) const {
	std::vector<Rayon> traj;
	traj.reserve(limite_taille+1);

	while (true) {
		traj.push_back(rayon);
		if (traj.size() > limite_taille) 
			break;
		auto p = this->interception_réémission(rayon);
		if (p.first == nullptr)
			break;
		rayon = p.second;
	}

	return traj;
}

// Caclul d'une trajectoire sur la scène et affichage de la trajectoire
//  dans la fenêtre window avec la couleur couleur_traj
//
std::vector<Rayon> Scene::propagation_trajectoire (Rayon traj0, size_t limite_taille, sf::RenderWindow& window, sf::Color couleur) const {

	// Calcul de la trajectoire
	std::vector<Rayon> traj = propagation_trajectoire(traj0, limite_taille);

	// Dessin des rayons
	sf::VertexArray line (sf::Lines, 2);
	for (size_t i = 0; i < traj.size()-1; ++i) {
		line[0] = sf::Vertex( coord_01_vers_fenetre( traj[i].orig ), couleur );
		line[1] = sf::Vertex( coord_01_vers_fenetre( traj[i+1].orig ), couleur );
		window.draw(line);
	}
	// Dessin du dernier rayon, non intercepté, allant à l'infini (ici = 10)
	const Rayon& last_rayon = traj.back();
	Point2 far_away = last_rayon.orig + 10 * Vec2::u_angle(last_rayon.dir_angle);
	line[0] = sf::Vertex( coord_01_vers_fenetre( last_rayon.orig ), couleur );
	line[1] = sf::Vertex( coord_01_vers_fenetre( far_away ), couleur );
	window.draw(line);

	return traj;
}

// Dessin de tous les objets et sources de la scène.
//
void Scene::dessiner_scene (sf::RenderWindow& window) const {
	for (ObjetReflechissant* objet : objets)
		objet->dessiner(window);
}
