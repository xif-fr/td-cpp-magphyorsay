#ifndef _OBJARC_H_
#define _OBJARC_H_

#include "ObjetReflechissantBase.h"

//-----------------------------------------------------------------------------------------------
// Objet optique en forme d'arc de cercle, de centre `centre`, de rayon `radius`, entre les deux
//  angles par rapport à l'horizontale définis par l'intervalle angulaire `interval_angulaire`
//  dans le sens trigonométrique.

class ObjetReflechissantArc : virtual public ObjetReflechissant {
protected:
	real_t radius;
public:
	Point2 centre;
	AngleIntervalle interval_angulaire;
	
	// Construction par le centre, le rayon et l'intervalle angulaire
	ObjetReflechissantArc (Point2 cercle_centre, real_t cercle_radius, AngleIntervalle ang_interval);
	// Construction du petit arc de cercle de rayon R passant par les points `a` et `b`
	ObjetReflechissantArc (Point2 a, Point2 b, real_t cercle_radius);
	
	void set_radius (real_t cercle_radius);
	real_t get_radius () const { return radius; };
	std::pair<Point2,Point2> extremit_arc () const;

	ObjetReflechissantArc (const ObjetReflechissantArc& o)
		: ObjetReflechissant(o), radius(o.radius), centre(o.centre), interval_angulaire(o.interval_angulaire) {}
	virtual ~ObjetReflechissantArc () {}

	// Implémentation du test d'intersection
	virtual DonnéesIntersection test_intersection (const Rayon& ray) const override;

	// Dessin
	virtual void dessiner (sf::RenderWindow& window) const override;
};

 #endif