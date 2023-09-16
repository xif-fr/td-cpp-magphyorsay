#include "Util.h"
#include "Scene.h"
#include <array>
#include <inttypes.h>
#include <cmath>

template <int Npts>
struct PhaseSpaceDistrib {

	// Tableau 2D représentant une distribution dans l'espace des phases 2D.
	// Chaque case compte le nombre de points de trajectoire étant passé par là.
	std::array<std::array<uint16_t,Npts>,Npts> phase_space_accumul;
	// Competeur de points pour normaliser la distribution
	uint64_t pts_accumul;

	// Mize à zéro
	void reset () {
		pts_accumul = 0;
		for (int i = 0; i < Npts; i++)
			for (int j = 0; j < Npts; j++) 
				phase_space_accumul[i][j] = 0;
	}
	PhaseSpaceDistrib() { reset(); }

	// Ajout des points d'une trajectoire dans l'espace des phases,
	// après une transformation appropriée des coordonnées
	void ajout_traj_dans_distrib (const std::vector<Rayon>& traj, int i_fold = 1, int j_fold = 1) {
		using namespace std;

		for (size_t n = 1; n < traj.size()-1; n++) {
			// p = angle du rayon dans [-pi,+pi[ par rapport à la verticale
			// (ceci pour centrer les orbites dans l'espace des phases)
			float p = angle_mod2pi_11(traj[n].dir_angle+M_PI/2);
			// q = point d'intersection avec le bord du billard par rapport au centre de la scène,
			// représenté par son angle dans [-pi,+pi[ par rapport à l'horizontale
			Vec2 icept = traj[n].orig - Point2{0.5,0.5};
			float q = atan2( icept.y, icept.x );
			// une transformation pour remettre droit l'espace des phases
			q = angle_mod2pi_11( q - p + M_PI );
			// discrétisation de (p,q) en entiers (i,j)
			int i = lroundf( (p/M_PI+1)/2.*i_fold * Npts ) % Npts;
			int j = lroundf( (q/M_PI+1)/2.*j_fold * Npts ) % Npts;
			// accumulation
			phase_space_accumul[i][j]++;
			pts_accumul++;
		}
	}

	// Retourne la densité dans l'espace des phases en (i,j) après passage par un log
	//  pour mieux visualiser les zones peu explorées.
	float log_phase_space_pixel (int i, int j) {
		using namespace std;
		float normalized = phase_space_accumul[i][j] / (float)pts_accumul;
		return log( 1 + normalized*Npts*Npts );
	}
};