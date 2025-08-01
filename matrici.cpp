#include <iostream>
#include <numeric>
#include <vector>

struct Pixel {
  int r;
  int g;
  int b;
};

int sign(double n) {
  return n>=0 ? 1 : -1;
}

std::vector<int> binary_pattern(
    std::vector<Pixel> p0) {  // costruisce il pattern bianco/nero
  std::vector<int> p_binary;
  for (auto it = p0.begin(); it != p0.end(); ++it) {
    ((it->r + it->g + it->b) / 3 >= 127) ? p_binary.push_back(1)
                                         : p_binary.push_back(-1);
  }
  return p_binary;
}

std::vector<std::vector<float>>
build_matrix(  // prima implementazione della costruzione della matrice
    int const& N, std::vector<std::vector<int>> patterns) {
  std::vector<std::vector<float>> matrix(N, std::vector<float>(N));
  for (long unsigned int mu{0}; mu < patterns.size();
       ++mu) {  // itero ogni pattern
    for (int i{0}; i < N; ++i) {
      for (int j{0}; j < N; ++j) {  // itero ogni i,j del pattern
        if (i == j) {
          matrix[i][j] = 0;
        } else {
          matrix[i][j] =
              std::accumulate(  // implementazione della somma tra il prodotto
                                // fra neuroni con accumulate
                  patterns.begin(), patterns.end(), 0.,
                  [N, i, j](double acc, const std::vector<int>& pattern) {
                    return acc + ((1.0 / N) * (pattern[i]) * (pattern[j]));
                  });
        }
      }
    }
  }
  return matrix;
}

std::vector<std::vector<float>>
build_alter(  // costruzione della matrice sfruttando la simmetria
    int const& N, std::vector<std::vector<int>> const& patterns) {
  std::vector<std::vector<float>> matrix(N, std::vector<float>(N));
  float const& N_inverse{1.0f / N};
  for (int i{0}; i < N; ++i) {
    for (int j{0}; j <= i;
         ++j) {  // questo for mi calcola solo la metà inferiore
      if (i == j) {
        matrix[i][j] = 0.f;
      } else {
        matrix[i][j] = std::accumulate(
            patterns.begin(), patterns.end(), 0.f,
            [N_inverse, i, j](float acc,
                              std::vector<int> const&
                                  pattern) {  // l'accumulatore è identico a prima
              return acc + (N_inverse * (pattern[i]) * (pattern[j]));
            });
        matrix[j][i] = matrix[i][j];  // sfrutto la simmetria
      }
    }
  }
  return matrix;
}  // FUNZIONA!

template <typename V, typename F>
std::vector<V> mat_vec_product(
    std::vector<std::vector<F>> mat,
    std::vector<V> vec) {  // prodotto matrice-vettore con inner product
  std::vector<V> result;
  for (std::vector<F> row : mat) {
    result.push_back(
        std::inner_product(row.begin(), row.end(), vec.begin(), 0));
  }
  return result;
}

std::vector<int> evolve(std::vector<std::vector<float>> mat,
                        std::vector<int> pattern) {
  std::vector<int> return_pattern;
  for (long unsigned int i{0}; i != pattern.size(); ++i) {
    double sum{0.};
    for (long unsigned int j{0}; j != pattern.size(); ++j) {
      sum += mat[i][j] * pattern[j];
    }
    pattern[i]=sign(sum);
  }
  return_pattern=pattern;
  return return_pattern;
}

double pattern_energy(std::vector<std::vector<float>> mat,
                      std::vector<int> pattern) {
  double energy{0.};
  for (long unsigned int i{0}; i != pattern.size(); ++i) {
    for (long unsigned int j{0}; j != pattern.size(); ++j) {
      energy += mat[i][j] * pattern[i] * pattern[j];
    }
  }
  return {energy * (-0.5)};
}

std::vector<int> recognize(std::vector<std::vector<float>> mat, //riconoscimento
                           std::vector<int> corrupt_pattern) {
int i{0};
  while (true) {
    ++i;
    if (i==50) {
      return corrupt_pattern; // se lo stato di equilibrio non converge interrompo il ciclo
    }
    std::vector<int> corrupt_evo{evolve(mat, corrupt_pattern)};
    std::cout << "Energy: " << pattern_energy(mat, corrupt_evo) << "\n" << "\n";
    if (evolve(mat, corrupt_evo) == corrupt_evo) {
      return corrupt_evo;
    } else {
      corrupt_pattern = evolve(mat, corrupt_pattern);
      continue;
    }
  }
}

int main() {
  std::vector<int> x1{-1, 1, 1, -1};
  std::vector<int> x2{1, -1, -1, 1};
  std::vector<std::vector<int>> mu{x1, x2};
  auto matrix{build_alter(4, mu)};
  std::vector<int> xc{1, -1, 1, -1};
  
  auto x_saved{recognize(matrix, xc)};

  for (int x : x_saved) {
    std::cout << x << "\n";
  }
}
