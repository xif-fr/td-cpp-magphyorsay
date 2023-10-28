#include <array>
#include <random>
#include <fstream>

std::random_device rd;
std::mt19937 rng (rd());
std::uniform_real_distribution<float> distrib_u01 (0,1);

constexpr uint32_t szL = 200, szH = 200;
constexpr size_t N = szL*szH;

inline uint32_t sqlatt_xy2i (uint16_t x, uint16_t y) {
	return x + szL * y;
}
inline std::array<uint32_t,4> latt_neighbors (uint32_t i) {
	int16_t x = i % szL;
	int16_t y = i / szL;
	uint32_t r = sqlatt_xy2i( (x+1)%szL, y );
	uint32_t u = sqlatt_xy2i( x, (y-1+szH)%szH );
	uint32_t l = sqlatt_xy2i( (x-1+szL)%szL, y );
	uint32_t d = sqlatt_xy2i( x, (y+1)%szH );
	return {r,u,l,d};
}

bool ising_metropolis_step (std::array<int8_t,N>& S, float β, float B) {
	uint32_t i0 = rng()%N;
	int8_t b = 0;
	for (uint32_t j : latt_neighbors(i0))
		b += S[j];
	float α_flip = expf( -β * 2 * S[i0] * (b + B) );
	if (distrib_u01(rng) < α_flip) {
		S[i0] = -S[i0];
		return true; // flip accepted
	}
	return false;
}


std::tuple<float,float> ising_magnétisation_énergie (std::array<int8_t,N>& S, float B) {
	int M = 0;
	float E = 0;
	for (uint32_t i = 0; i < N; i++) {
		M += S[i];
		for (uint32_t j : latt_neighbors(i))
			E -= S[i] * S[j] / 2.f;
		E -= S[i] * B;
	}
	return { M /(float) N, E / N };
}


int main () {

	float β = log(1+sqrt(2))/2;
	float B = 0;

	std::ofstream fichier_r ("ising_magnétisation_énergie_txaccept.txt");

	std::array<int8_t,N> S;
	for (uint32_t i = 0; i < N; i++) {
		S[i] = 2 * (rng()%2) - 1;
	}

	int steps = 0;
	int steps_accept = 0;

	while (steps < 200000000) {

		constexpr int recurr_stats = 50000;
		if (steps%recurr_stats == 0) {
			auto [m, e] = ising_magnétisation_énergie(S, B);
			fichier_r << steps << " " << m << " " << e << " " << steps_accept/(float)recurr_stats << std::endl;
			steps_accept = 0;
		}

		steps_accept += ising_metropolis_step(S, β, B);
		steps++;

	}

	return 0;
}
