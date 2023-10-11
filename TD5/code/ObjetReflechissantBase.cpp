#include "ObjetReflechissantBase.h"
#include <cmath>

constexpr real_t interception_dist2_minimale = 1e-12;

// L'objet intercepte-t-il le rayon ? Si oui, donne la distance géométrique au carré
//  parcourue par le rayon depuis son point d'émission (Réémission::distance_carrée),
//  ainsi que le rayon réfléchi (Réémission::rayon_réémis). Sinon, renvoie Inf comme
//  distance, ainsi qu'un rayon invalide.
//
ObjetReflechissant::Réémission ObjetReflechissant::interception_réémission (const Rayon& ray) const {
	// Appel de la méthode de test d'intersection dépendant de la géométrie
	DonnéesIntersection isect = test_intersection(ray);

	if (isect.intersection) {
		real_t dist2 = (ray.orig - isect.point_intersect).norm2();
		// on introduit une distance minimale qu'un rayon peut parcourit avant d'être intercepté
		// par une courbe pour éviter qu'un objet ré-intercepte immédiatement le rayon qu'il vient
		// d'émettre, ce qui arriverait souvent en simple précision
		if (dist2 < interception_dist2_minimale) {
			return {Inf, Rayon::rayon_invalide()};
		}
		else {
			Réémission r;
			// Loi de la réflection : i_refl = i par rapport à la normale
			r.rayon_réémis.orig = isect.point_intersect;
			r.rayon_réémis.dir_angle = isect.angle_normale - isect.angle_incidence;
			r.distance_carrée = dist2;
			return r;
		}
	} else {
		return {Inf, Rayon::rayon_invalide()};
	}
}
