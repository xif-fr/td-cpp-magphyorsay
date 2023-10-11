#include "ObjetReflechissantArc.h"
#include <SFML/Graphics.hpp>
#include <cmath>
using namespace std;

// Construction par le centre, le rayon et l'intervalle angulaire
//
ObjetReflechissantArc::ObjetReflechissantArc (Point2 cercle_centre, real_t cercle_radius, AngleIntervalle ang_interval)
	: radius(0), centre(cercle_centre), interval_angulaire(ang_interval)
{
	set_radius(cercle_radius); // vérification du rayon
}

// Setter pour le rayon
//
void ObjetReflechissantArc::set_radius (real_t cercle_radius) {
	if (cercle_radius <= 0.f)
		throw std::domain_error("ObjetReflechissantArc : un rayon ne peut être négatif.");
	radius = cercle_radius;
}

// Construction du petit arc de cercle de rayon R passant par les points `a` et `b`
//
ObjetReflechissantArc::ObjetReflechissantArc (Point2 a, Point2 b, real_t cercle_radius)
	: radius(0), centre({0,0}), interval_angulaire(0)
{
	if (cercle_radius < 0) {
		std::swap(a,b);
		radius = -cercle_radius;
	} else
		radius = cercle_radius;
	// translation
	Vec2 ab = b - a;
	// vérification
	real_t ab2 = ab.norm2();
	if (ab2 > 4*radius*radius)
		throw std::domain_error("ObjetReflechissantArc(A,B,R) : points A et B trop éloignés");
	// rotation
	real_t theta0 = atan2(ab.y, ab.x) - M_PI/2;
	// calcul dans le repère 'prime'
	Vec2 c_ = {
		.x = -sqrt( radius*radius - ab2/4 ),
		.y = sqrt(ab2)/2
	};
	real_t theta_ = atan( c_.y / c_.x );
	AngleIntervalle ang_ = AngleIntervalle( theta_-1e-10, -theta_+1e-10 );
	// dé-rotation
	interval_angulaire = ang_ + theta0;
	c_ = c_.rotate(theta0);
	// dé-translation
	centre = a + c_;
}

std::pair<Point2,Point2> ObjetReflechissantArc::extremit_arc () const {
	return {
		centre + radius * Vec2::u_angle(interval_angulaire.beg()),
		centre + radius * Vec2::u_angle(interval_angulaire.end())
	};
}

// Implémentation du test d'interception entre l'arc de cerle et la demi-droite 
// (notations de `lois.pdf`)
//
ObjetReflechissant::DonnéesIntersection ObjetReflechissantArc::test_intersection (const Rayon& ray) const {
	Vec2 oc = centre - ray.orig;
	real_t b = !oc / radius;  // d/R
	real_t base_ang = atan2(oc.y, oc.x);
	// angles relatifs à l'axe OC
	real_t alpha = angle_mod2pi_11( ray.dir_angle - base_ang );
	AngleIntervalle thetaAB = interval_angulaire + -base_ang;
	// intersection avec le cecle si en dessous de l'angle critique
	real_t y = b * sin(alpha);
	if ((b > 1 and abs(alpha) >= M_PI/2) or abs(y) > 1)
		return DonnéesIntersection{false};
	// angles repérant les points d'intersection avec le cercle
	real_t arcsiny = asin(y);
	real_t theta1 = alpha - arcsiny + M_PI,
	       theta2 = alpha + arcsiny;
	real_t theta_incidence;
	/// si on est bien sur notre arc de cercle :
	DonnéesIntersection isect;
	// θ1 n'est accessible que si la rayon vient de l'extérieur
	if ( b > 1+1e-10 and thetaAB.inclus(theta1) ) {
		// isect.sens_reg = true; // ext vers int du cercle
		theta_incidence = theta1 + base_ang;
		isect.angle_normale = theta_incidence;
		isect.angle_incidence = M_PI - theta1 + alpha; // c'est un angle relatif, pas besoin de base_ang
	}
	// test de θ1 pour b<1 ou si θ1 a échoué pour b>1
	else if ( thetaAB.inclus(theta2) ) {
		// isect.sens_reg = false; // int vers ext du cercle
		theta_incidence = theta2 + base_ang;
		isect.angle_normale = theta_incidence + M_PI; // normale vers l'intérieur du cercle
		isect.angle_incidence = alpha - theta2;
	}
	else
		return DonnéesIntersection{false};
	// calcul du point d'incidence
	isect.point_intersect = centre + radius * Vec2::u_angle(theta_incidence);

	isect.intersection = true;
	return isect;
}

// Affichage
//
void ObjetReflechissantArc::dessiner (sf::RenderWindow& window) const {
	int N = std::max<int>(7, radius * 400 * interval_angulaire.longueur());
	sf::VertexArray lines (sf::LineStrip, N+1);
	for (int k = 0; k <= N; k++) {
		real_t theta = interval_angulaire.beg() + interval_angulaire.longueur() * (real_t) k / N;
		lines[k] = sf::Vertex(
			coord_01_vers_fenetre( centre + radius * Vec2::u_angle(theta) ),
			sf::Color::White
		); 
	}
	window.draw(lines);
}
