#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "Util.h"
#include "ObjetReflechissantBase.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Scene {
private:
	// Liste des objets de la scène (liste de pointeurs car les objets sont traités polymorphiquement)
	std::vector<ObjetReflechissant*> objets;

public:
	Scene () = default;
	Scene (const Scene&) = delete;
	Scene& operator= (const Scene&) = delete;

	// Raccourci pour création d'objet de type `ObjT` : simple transfert des argument au constructeur
	template <class ObjT, typename... Args>
	ObjT& créer_objet (Args&& ...args) {
		//auto obj = std::make_shared<ObjT>(std::forward<Args>(args)...);
		ObjT* obj = new ObjT(std::forward<Args>(args)...);
		this->objets.push_back(obj);
		return *obj;
	}

	// Vide la scène de tous les objets
	void vider () {
		for (ObjetReflechissant* objet : objets)
			delete objet;
		objets.clear();
	}

	~Scene() {
		vider();
	}
	
		///--------- Routines de propagation des rayons ---------///

	// Test d'interception du rayon contre toutes les objets de la scène puis ré-émission; la première
	//  interception sur le trajet du rayon depuis son origine est choisie. Le premier élément de la paire
	//  est un pointeur sur l'objet interceptant le rayon (nullptr si il n’y a aucune interception),
	//  et le deuxième élément sera la rayon éventuellement ré-émis.
	std::pair<ObjetReflechissant*,Rayon> interception_réémission (const Rayon& rayon_in) const;
	
	// Caclul d'une trajectoire sur la scène : récursion de l'interception/ré-émission
	// Renvoie la trajectoire sous forme d'un tableau de rayons
	std::vector<Rayon> propagation_trajectoire (Rayon traj0, size_t limite_taille) const;

	// Caclul d'une trajectoire sur la scène et affichage de la trajectoire
	//  dans la fenêtre window avec la couleur couleur_traj
	std::vector<Rayon> propagation_trajectoire (Rayon traj0, size_t limite_taille, sf::RenderWindow& window, sf::Color couleur_traj) const;
	
		///--------- Affichage et interface utilisateur ---------///
	
	// Dessin de tous les objets et sources de la scène.
	void dessiner_scene (sf::RenderWindow& window) const;

};

#endif
