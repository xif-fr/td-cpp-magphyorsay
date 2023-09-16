#ifndef _OBJBASE_H_
#define _OBJBASE_H_

#include "Util.h"
#include <SFML/Graphics/RenderWindow.hpp>

//-----------------------------------------------------------------------------------------------
// Classe de base des objets réfléchissants, où la surface de réflection est une ligne.
// Déclare l'interface commune utilisée lors de la propagation (interception puis ré-émission)
// des rayons, et l'affichage. Un ObjetReflechissant est capable de tester l'interception
// d'un rayon avec `interception_réémission` et, le cas échéant, de ré-émettre le rayon.
// Il est aussi capable de se dessiner dans une fenêtre SFML avec `dessiner`.

class ObjetReflechissant {
public:
	
	ObjetReflechissant () {}
	ObjetReflechissant& operator= (const ObjetReflechissant&) = delete;
	ObjetReflechissant (const ObjetReflechissant&) {}
	virtual ~ObjetReflechissant () {}

	// L'objet intercepte-t-il le rayon ? Si oui, donne la distance géométrique au carré
	//  parcourue par le rayon depuis son point d'émission ainsi qu'un rayon réfléchi.
	//  Sinon, renvoie Inf.
	struct Réémission {
		real_t distance_carrée;
		Rayon rayon_réémis;
	};
	Réémission interception_réémission (const Rayon& ray) const;
	
	// Méthode de test d'intersection, à surcharger dans les classes filles en fonction de la géométrie.
	struct DonnéesIntersection {
		bool intersection;
		Point2 point_intersect; // Point d'intersection
		real_t angle_incidence; // Angle d'incidence (angle du rayon à la normale)
		real_t angle_normale; // Angle absolu du vecteur normal à la courbe (par rapport à l'horizontale)
		//	bool sens_reg; // Sens d'incidence du rayon `sens_reg` (l'objet étant orienté; `true` si même sens que sur les figures du document)
	};
	virtual DonnéesIntersection test_intersection (const Rayon& ray) const = 0;

	// Rendu graphique de l'objet, à surcharger dans les classes filles.
	virtual void dessiner (sf::RenderWindow& window) const = 0;
};

#endif