#include <complex>
#include <iostream>
#include <iomanip>
#include <functional>
#include <fstream>
#include <math.h>

// note : on utilise ici le type std::complex de la bibliothèque standard, qui devrait fonctionner exactement comme votre classe complexe
using complexe = std::complex<double>;
// déclaration du type fonction complexe -> complexe
using fonc_complexe_t = std::function<complexe(complexe)>;

/*
  Algorithme de la méthode de Newton, prenant une fonction `f` et sa dérivée `f_deriv`,
  et cherchant un zéro de `f` en partant de `z`. On dit que l'on a obtenu un zéro `z_star` lorsque
  |f(z_star)| < epsilon, et `methode_newton` renvoit la paire (z_star,n_it), où `n_it` est le
  nombre d'itérations nécessaires pour l'atteindre. Si un zéro n'est pas atteint en `max_it`
  itérations, on déclare qu'il n'y a pas eu convergence en renvoyant `n_it=-1`.
  Optionellement, on peut donner un facteur d'amortissement `h`, ainsi qu'une fonction callback `cb_iter`,
  appelée à chaque itération. Si `debug=true`, un message est affiché à chaque itération.
*/
std::tuple<complexe,int> methode_newton (
  fonc_complexe_t f, fonc_complexe_t f_deriv,
  complexe z,
  unsigned int max_it, double epsilon, double h = 1.,
  bool debug = false, std::function<void(complexe)> cb_iter = std::function<void(complexe)>()
) {
  int n = 0;
  complexe fz = f(z); // valeur courant de la fonction
  // On itère tant que |f(z)| > ɛ. Pour ne pas avoir à calculer une coûteuse racine carrée, on regarde plutôt |f(z)|^2 < espilon^2.
  epsilon *= epsilon;
  while (std::norm(fz) > epsilon) {
    if (n >= max_it)
      return { z, -1 }; // pas de convergence
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
  // Note fonction et sa dérivée
  fonc_complexe_t f = [] (complexe z) { auto z2 = z*z; return z2*z2 - 2.*z2 - complexe(1,0); };
  fonc_complexe_t f_deriv = [] (complexe z) { return 4.*z*z*z - 4.*z; };
  // Stockage des zéros que l'on trouvera
  std::vector<complexe> zeros;

  /******* Scan de z_0 *******/

  // Définition de la grille
  int N = 1000;
  double a = 2, da = a/N;

  // Fichier stockant le numéro du zéro obtenu
  std::ofstream fichier_bassins_converg ("newton_bassins_converg.txt");
  // Fichier stockant le nombre d'itérations nécessaires
  std::ofstream fichier_niter ("newton_niter.txt");

  for (int i = -N; i <= N; i++) {
    std::cout << i << std::endl;
    for (int j = -N; j <= N; j++) {

      // Méthode de Newton partant de z_0
      complexe z_0 = {i*da, j*da};
      auto [z_zero, nit] = methode_newton(f, f_deriv, z_0, 200, a*1e-15);

      // Détermination du numéro du zéro (si on a convergé)
      int num_zero = -1;
      if (nit >= 0) {
        // Recherche parmi les zéros déjà obtenus
        for (int k = 0; k < zeros.size(); k++) {
          if (std::norm(z_zero-zeros[k]) < 1e-10)
            num_zero = k;
        }
        // Ajout de l'éventuel nouveau zéro
        if (num_zero == -1) {
          zeros.push_back(z_zero);
          num_zero = zeros.size() - 1;
        }
      }

      // Écriture dans les fichiers
      fichier_bassins_converg << num_zero << " ";
      fichier_niter << nit << " ";
    }
    fichier_bassins_converg << "\n";
    fichier_niter << "\n";
  }

  // Affichage de tous les zéros obtenus
  std::cout << "Zéros :" << std::endl;
  for (complexe z : zeros) {
    std::cout << z << std::endl;
  }

  return 0;
}