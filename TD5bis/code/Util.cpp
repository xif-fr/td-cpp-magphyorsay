#include "Util.h"
#include <cmath>
#include <stdexcept>
#include <cassert>
using namespace std;

Vec2 Vec2::u_angle (real_t theta) {
	return {
		.x = cos(theta),
		.y = sin(theta)
	};
}

real_t Vec2::operator! () const {
	return hypot(x, y);
}

Vec2 Vec2::rotate (real_t theta) const {
	real_t c = cos(theta), s = sin(theta);
	return { .x = c * x - s * y,
	         .y = s * x + c * y };
}

// ⎡a  b⎤ ⎡x⎤   ⎡e⎤
// ⎣c  d⎦ ⎣y⎦ = ⎣f⎦
//
void mat22_sol (real_t a, real_t b, real_t c, real_t d, real_t e, real_t f, real_t& x, real_t& y) {
	real_t det = a * d - b * c;
	x = (e * d - b * f) / det;
	y = (a * f - e * c) / det;
}

const AngleIntervalle AngleIntervalle::cercle_entier (2*M_PI);

AngleIntervalle::AngleIntervalle (real_t theta_beg, real_t b) : a(theta_beg), l(0) {
	if (abs(b-a) > 2*M_PI+1e-6)
		throw std::domain_error("angle interval > 2π");
	if (b < 0 and a >= 0)
		b += 2*M_PI;
	l = b - a;
	a = angle_mod2pi_02(a);
//	assert(0 <= a and a <= 2*M_PI and 0 <= l and l <= 2*M_PI);
}

AngleIntervalle::AngleIntervalle (real_t lenght) : a(0), l(lenght) {
	if (l < 0 or l > 2*M_PI+1e-6)
		throw std::domain_error("invalid angle interval length");
}

AngleIntervalle AngleIntervalle::operator+ (real_t delta_theta) const {
	AngleIntervalle o = *this;
	o.a = angle_mod2pi_02(a + delta_theta);
	return o;
}

real_t angle_mod2pi_11 (real_t theta) {
	while (theta > M_PI)
		theta -= 2*M_PI;
	while (theta < -M_PI)
		theta += 2*M_PI;
	return theta;
}

real_t angle_mod2pi_02 (real_t theta) {
	return theta - 2*M_PI* floor( theta/(2*M_PI) );
}

bool AngleIntervalle::inclus (real_t theta) const {
	theta = angle_mod2pi_02(theta);
	if (a <= theta and theta <= a+l)
		return true;
	theta += 2*M_PI;
	return (a <= theta and theta <= a+l);
}

std::pair<Vec2,Vec2> AngleIntervalle::vec_a_b () const {
	return { Vec2{ .x = cos(a),   .y = sin(a) },
	         Vec2{ .x = cos(a+l), .y = sin(a+l) } };
}
