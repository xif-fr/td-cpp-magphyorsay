#ifndef _OBJLIGNE_H_
#define _OBJLIGNE_H_

#include "ObjetReflechissantBase.h"

//-----------------------------------------------------------------------------------------------
// Objet réfléchissant ayant la forme d'un segment.

class ObjetReflechissantLigne : virtual public ObjetReflechissant {
protected:
	Point2 extremit_b;
	Vec2 vecteur_ba;
	real_t angle_segment;  // angle (horizontale,a,b) gardé en cache pour optimisation

public:
	 // Constructeurs et destructeur
	ObjetReflechissantLigne (Point2 a, Point2 b);  // point a et b
	ObjetReflechissantLigne (Point2 b, real_t longueur, real_t angle);  // point b, longueur, et angle (horizontale,a,b)
	ObjetReflechissantLigne (const ObjetReflechissantLigne& o) : ObjetReflechissant(o), extremit_b(o.extremit_b), vecteur_ba(o.vecteur_ba), angle_segment(o.angle_segment) {}
	virtual ~ObjetReflechissantLigne () {}

	// Getter
	std::pair<Point2,Point2> extremit_segment () const;
	// Setter
	void set_extremit_segment (Point2 a, Point2 b);

	// Implémentation du test d'interception
	virtual DonnéesIntersection test_intersection (const Rayon& ray) const override;

	// Dessin
	virtual void dessiner (sf::RenderWindow& window) const override;
};

#endif