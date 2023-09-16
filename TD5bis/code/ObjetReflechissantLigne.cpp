#include "ObjetReflechissantLigne.h"
#include <SFML/Graphics.hpp>
#include <cmath>
using namespace std;

//-------------------------- ObjetReflechissantLigne --------------------------//


void ObjetReflechissantLigne::set_extremit_segment (Point2 a, Point2 b) {
	vecteur_ba = a - b;
	extremit_b = b;
	angle_segment = atan2(vecteur_ba.y, vecteur_ba.x);
}

std::pair<Point2,Point2> ObjetReflechissantLigne::extremit_segment () const {
	return { extremit_b + vecteur_ba, extremit_b };
}

ObjetReflechissantLigne::ObjetReflechissantLigne (Point2 a, Point2 b) {
	set_extremit_segment(a, b);
}

ObjetReflechissantLigne::ObjetReflechissantLigne (Point2 b, real_t longueur, real_t angle) :
	extremit_b(b),
	vecteur_ba(longueur * Vec2::u_angle(angle)),
	angle_segment(angle)
{}

// Test d'intersection du rayon sur le segment.
// Cas particulier d'alignement non pris en compte
//
ObjetReflechissant::DonnéesIntersection ObjetReflechissantLigne::test_intersection (const Rayon& ray) const {
	DonnéesIntersection isect;

	Vec2 u_rayon = Vec2::u_angle(ray.dir_angle);
	real_t s, t;
	mat22_sol(vecteur_ba.x, -u_rayon.x,
	          vecteur_ba.y, -u_rayon.y,
	          ray.orig.x - extremit_b.x, ray.orig.y - extremit_b.y,
	          s, t);
	if ((0 <= s and s <= 1) and t >= 0) {
		isect.intersection = true;
		// point d'incidence
		isect.point_intersect = ray.orig + t * u_rayon;
		// angle d'incidence
		real_t alpha = angle_mod2pi_11(ray.dir_angle);
		real_t i = alpha - (angle_segment - M_PI/2);
		if (fabs(angle_mod2pi_11(i)) < M_PI/2) {
			isect.angle_normale = angle_segment + M_PI/2;
			isect.angle_incidence = i;
			// isect.sens_reg = true;
		} else {
			isect.angle_incidence = i - M_PI;
			if (isect.angle_incidence < -M_PI/2)
				isect.angle_incidence += 2*M_PI;
			isect.angle_normale = angle_segment - M_PI/2;
			// isect.sens_reg = false;
		}
	} else {
		isect.intersection = false;
	}
	return isect;
}

// Affichage
//
void ObjetReflechissantLigne::dessiner (sf::RenderWindow& window) const {
	auto [a,b] = extremit_segment();
	sf::VertexArray line (sf::Lines, 2);
	line[0] = sf::Vertex( coord_01_vers_fenetre(a), sf::Color::White );
	line[1] = sf::Vertex( coord_01_vers_fenetre(b), sf::Color::White );
	window.draw(line);
}
