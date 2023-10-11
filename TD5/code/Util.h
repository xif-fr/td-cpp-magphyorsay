#ifndef _UTIL_H_
#define _UTIL_H_

#include <utility>
#include <limits>

using real_t = double; // peut être utile de de définir un type real_t pour faire des tests entre simple et double précision

// Nombre flottant infini
#define Inf std::numeric_limits<real_t>::infinity()
// Nombre flottant invalide (Not a Number)
#define NaN std::numeric_limits<real_t>::signaling_NaN()

/********************************************************************
 * Vecteur 2D. Simple structure (x,y) avec opérateurs surchargés
 * et quelques méthodes utiles. Ne pas utiliser pour représenter un
 * point dans un espace carthésien, il y a Point2 pour ça.
 */
struct Vec2 {
	real_t x, y;
	void operator+=  (Vec2 o)         { x += o.x; y += o.y; }
	void operator-=  (Vec2 o)         { x -= o.x; y -= o.y; }
	Vec2 operator+   (Vec2 o)   const { return Vec2{ x+o.x, y+o.y }; }
	Vec2 operator-   ()         const { return Vec2{ -x, -y }; }
	Vec2 operator-   (Vec2 o)   const { return Vec2{ x-o.x, y-o.y }; }
	void operator*=  (real_t k)       { x *= k; y *= k; }
	void operator/=  (real_t k)       { x /= k; y /= k; }
	Vec2 operator*   (real_t k) const { return Vec2{ k*x, k*y }; }
	Vec2 operator/   (real_t k) const { return Vec2{ x/k, y/k }; }
	real_t operator| (Vec2 o)   const { return x*o.x + y*o.y ; }	// produit scalaire
	real_t norm2     ()         const { return x*x + y*y; }			// norme au carré
	real_t operator! ()         const;								// norme
	Vec2  rotate     (real_t theta) const;							// rotation d'un angle `theta`
	static Vec2 u_angle (real_t theta);  // fonction statique de création d'un vecteur unité à partir d'un angle
};
inline Vec2 operator* (double k, const Vec2& v) { return v*k; }


/********************************************************************
 * Point dans un espace 2D, en coordonnées carthésiennes.
 * Simple structure (x,y) avec opérateurs surchargés : la soustraction
 * de deux points est un vecteur, l'addition d'un vecteur à un point
 * donne un point. 
 */
struct Point2 {
	real_t x, y;
	Vec2 operator- (Point2 o) const { return Vec2{ x-o.x, y-o.y }; }  // vecteur entre deux points
	Point2 operator+ (Vec2 v) const { return Point2{ x+v.x, y+v.y }; }  // translation du point par un vecteur
};


/********************************************************************
 * Intervalle d'angle dans ℝ/2πℝ, représentant par exemple un arc de
 * cercle ou un intervalle angulaire d'émission. 
 */
class AngleIntervalle {
private:
	real_t a, l; // angle de début (0 ≤ a ≤ 2π) et longueur de l'intervalle (0 ≤ l ≤ 2π)
public:
	AngleIntervalle (const AngleIntervalle&) = default;
	static const AngleIntervalle cercle_entier;
	
	// construction à partir de l'angle de début et de fin
	// si (theta_beg > 0 > theta_end) numériquement, interprète l'intervalle comme [theta_beg,theta_end+2π]
	// si |theta_end-theta_beg| > 2π, lances une exception (n'est pas censé arriver dans ce programme)
	AngleIntervalle (real_t theta_beg, real_t theta_end);
	// construction d'un intervalle de longeur donnée, partant de θ=0
	AngleIntervalle (real_t lenght);
	
	AngleIntervalle operator+ (real_t delta_theta) const;	// rotation de l'intervalle
	real_t longueur () const { return l; }				// longueur de l'intervalle, entre 0 et 2π
	bool inclus (real_t theta) const;					// test si un angle (dans ℝ/2πℝ) est inclus dans l'intervalle
	real_t beg () const { return a; }					// angle de début, entre 0 et 2π
	real_t end () const { return a+l; }					// angle de fin, entre 0 et 4π
	std::pair<Vec2,Vec2> vec_a_b () const;			// vecteurs unité définissant les angles début et fin
};

// réduit un angle à [-π,+π]
real_t angle_mod2pi_11 (real_t);
// réduit un angle à [0,2π]
real_t angle_mod2pi_02 (real_t);


/********************************************************************
 * Rayon (= demi-droite) : son origine, son angle à l'horizontale
 */
struct Rayon {
	Point2 orig;
	real_t dir_angle;

	// méthode statique renvoyant un rayon invalide
	static Rayon rayon_invalide () { return Rayon{{NaN,NaN},NaN}; }
};


/// Divers

/********************************************************************
 * Réslution du système linéaire
 * ⎡a  b⎤ ⎡x⎤   ⎡e⎤
 * ⎣c  d⎦ ⎣y⎦ = ⎣f⎦
 */
void mat22_sol (real_t a, real_t b, real_t c, real_t d, real_t e, real_t f, real_t& x, real_t& y);


/********************************************************************
 * Conversion de coordonnées SFML <-> point dans l'espace [0,1]x[0,1] usuel
 */

#include <SFML/System/Vector2.hpp>

constexpr int TAILLE_FENETRE_PIXELS = 700;  // Taille de la fenêtre principale SFML

inline Point2 coord_fenetre_vers_01 (sf::Vector2f v) {
	return { .x = v.x/TAILLE_FENETRE_PIXELS, .y = 1-v.y/TAILLE_FENETRE_PIXELS };
}

inline sf::Vector2f coord_01_vers_fenetre (Point2 p) {
	return sf::Vector2f( p.x*TAILLE_FENETRE_PIXELS, (1-p.y)*TAILLE_FENETRE_PIXELS );
}

#endif
