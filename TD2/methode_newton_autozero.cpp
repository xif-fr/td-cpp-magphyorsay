#include <complex>
#include <iostream>
#include <iomanip>
#include <functional>
#include <fstream>
#include <math.h>

using complexe = std::complex<double>;
using fonc_complexe_t = std::function<complexe(complexe)>;

std::tuple<complexe,int> methode_newton (
  fonc_complexe_t f, fonc_complexe_t f_deriv,
  complexe z,
  unsigned int max_it, double epsilon, double h = 1.,
  bool debug = false, std::function<void(complexe)> cb_iter = std::function<void(complexe)>()
) {
  int n = 0;
  complexe fz = f(z);
  epsilon *= epsilon;
  // on regarde plutôt |f(z)|^2 < espilon^2
  while (std::norm(fz) > epsilon) {
    if (n >= max_it)
      return { z, -1 };
    z -= h * fz / f_deriv(z);
    n += 1;
    fz = f(z);
    if (debug)
      std::cout << "newton iter " << n << " : f(z) = " << fz << " at z = " << z << std::endl;
    if (cb_iter)
      cb_iter(z);
  }
  return { z, n };
}

int main () {
  fonc_complexe_t f = [] (complexe z) { auto z2 = z*z; return z2*z2 - 2.*z2 - complexe(1,0); };
  fonc_complexe_t f_deriv = [] (complexe z) { return 4.*z*z*z - 4.*z; };
  std::vector<complexe> zeros;

  int N = 1000;
  double a = 2, da = a/N;

  std::ofstream fichier_bassins_converg ("newton_bassins_converg.txt");
  std::ofstream fichier_niter ("newton_niter.txt");

  for (int i = -N; i <= N; i++) {
    std::cout << i << std::endl;
    for (int j = -N; j <= N; j++) {

      complexe z_0 = {i*da, j*da};
      auto [z_zero, nit] = methode_newton(f, f_deriv, z_0, 200, a*1e-15, 1, false);

      // détermination du numéro du zéro
      int num_zero = -1;
      if (nit >= 0) {
        for (int k = 0; k < zeros.size(); k++) {
          if (std::norm(z_zero-zeros[k]) < 1e-10)
            num_zero = k;
        }
        if (num_zero == -1) {
          zeros.push_back(z_zero);
          num_zero = zeros.size() - 1;
        }
      }

      fichier_bassins_converg << num_zero << " ";
      fichier_niter << nit << " ";
    }
    fichier_bassins_converg << "\n";
    fichier_niter << "\n";
  }

  std::cout << "Zéros :" << std::endl;
  for (complexe z : zeros) {
    std::cout << z << std::endl;
  }

  return 0;
}