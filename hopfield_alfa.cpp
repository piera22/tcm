#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "tinyfiledialogs.hpp"

struct Dims {
  unsigned int x;
  unsigned int y;
};

int sign(double n) { return n >= 0 ? 1 : -1; }

/* IDEA PER IL METODO DEFINITIVO:
1) Crea una nuova render texture
2) Applica il metodo di ridimensionamento, incolla lo sprite ridimensionato
sulla render texture creata 3) Applica il metodo di conversione bw usando la
nuova render texture*/

/* E PER L'INVERSO? OSSIA DISEGNARE UN IMMAGINE A PARTIRE DA UNA VERTEX ARRAY?
1) Crea una nuova vertex array con dimensioni note
2) Disegna la vertex array con un doppio ciclo for usando le dimensioni
predefinite 3) Coordinate da stabilire a piacimento, colore in base al numero
del vettore 4) Per individuare l'elemento del vettore usa l'indice [N_height*i +
j] */

class Hopfield {
  unsigned int N_base_;
  unsigned int N_height_;
  std::vector<std::vector<int>> patterns_;
  std::vector<sf::Sprite> images_;
  std::vector<double> matrix_;

 public:
  void load_sprite() {
    const char* path = tinyfd_openFileDialog("Seleziona un'immagine", "", 0,
                                             nullptr, "File immagine", 0);

    if (!path) {
      std::cerr << "Nessun file selezionato!" << "\n";
    }

    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
      std::cerr << "Errore nel caricamento della texture!" << "\n";
    }
    sf::Sprite sprite(texture);
    images_.push_back(sprite);
  }

  void update_patterns(std::vector<int> const& pattern) {
    patterns_.push_back(pattern);
  }

  void update_images(sf::Sprite const& image_s) { images_.push_back(image_s); }

  void update_memory(std::vector<int> const& pattern, sf::Sprite const& image) {
    update_patterns(pattern);
    update_images(image);
  }

  sf::VertexArray bw_conversion(const sf::Sprite& original_s) {
    // rendering
    sf::RenderTexture original_t;
    original_t.create(N_base_, N_height_);
    original_t.draw(original_s);
    original_t.display();  // finalizza il rendering

    sf::Image original{original_t.getTexture().copyToImage()};
    sf::VertexArray image_bw(sf::Points, N_base_ * N_height_);

    for (unsigned int i{0}; i < N_height_; ++i) {
      for (unsigned int j{0}; j < N_base_; ++j) {
        (original.getPixel(j, i).r + original.getPixel(j, i).g +
         original.getPixel(j, i).b) /
                    3 >=
                127
            ? image_bw[N_height_ * i + j].color = sf::Color::White
            : image_bw[N_height_ * i + j].color = sf::Color::Black;
      }
    }
    return image_bw;
  }

  void scale_to_standard(sf::Sprite& to_be_scaled) {
    const sf::Sprite& standard{images_[0]};
    to_be_scaled.scale(
        standard.getGlobalBounds().width / to_be_scaled.getGlobalBounds().width,
        standard.getGlobalBounds().height /
            to_be_scaled.getGlobalBounds().height);
  }

  sf::VertexArray image_conversion(sf::Sprite& image) {
    scale_to_standard(image);
    return bw_conversion(image);
  }

  std::vector<int> get_pattern(sf::VertexArray bw) {
    std::vector<int> pattern(N_base_ * N_height_);
    for (long unsigned int i{0}; i < N_height_; ++i) {
      for (long unsigned int j{0}; j < N_base_; ++j) {
        if (bw[N_height_ * i + j].color == sf::Color::White) {
          pattern.push_back(1);
        } else {
          pattern.push_back(-1);
        }
      }
    }
    return pattern;
  }

  void update_patterns(sf::Sprite const& original_s) {
    sf::VertexArray image_bw{bw_conversion(original_s)};
    patterns_.push_back(get_pattern(image_bw));
  }

  void update_matrix() {
    double N_inverse{1.0 / (N_base_ * N_height_)};
    for (long unsigned int i{0}; i < N_base_; ++i) {
      for (long unsigned int j{0}; j <= i;
           ++j) {  // questo for mi calcola solo la metà inferiore
        if (i == j) {
          matrix_[i * N_base_ + j] = 0.f;
        } else {
          matrix_[i * N_base_ + j] = std::accumulate(
              patterns_.begin(), patterns_.end(), 0.f,
              [N_inverse, i, j](
                  double acc,
                  std::vector<int> const&
                      pattern) {  // l'accumulatore è identico a prima
                return acc + (N_inverse * (pattern[i]) * (pattern[j]));
              });
          matrix_[j * N_base_ + i] =
              matrix_[i * N_base_ + j];  // sfrutto la simmetria
        }
      }
    }
  }

  void save_matrix() {
    std::ofstream mat_file("matrix.txt");

    for (unsigned int i{0}; i < N_base_; ++i) {
      for (unsigned int j{0}; j < N_base_; ++j) {
        mat_file << matrix_[i * N_base_ + j] << " ";
      }
      mat_file << "\n";
    }
    mat_file.close();
  }

  void read_matrix() {
    std::ifstream mat_file("matrix.txt");
    std::string row_string;
    std::vector<double> row;
    while (getline(mat_file, row_string)) {
      std::istringstream iss(row_string);
      double entry;
      while (iss >> entry) {
        matrix_.push_back(entry);
      }
    }
    mat_file.close();
  }

  void evolve(std::vector<int>& pattern) {
    for (long unsigned int i{0}; i != N_base_; ++i) {
      double sum{0.};
      for (long unsigned int j{0}; j != N_base_; ++j) {
        sum += matrix_[i * N_base_ + j] * pattern[j];
      }
      pattern[i] = sign(sum);
    }
  }

  double get_energy(std::vector<int> pattern) {
    double energy{0.};
    for (long unsigned int i{0}; i != N_base_; ++i) {
      for (long unsigned int j{0}; j != N_base_; ++j) {
        energy += matrix_[i * N_base_ + j] * pattern[i] * pattern[j];
      }
    }
    return {energy * (-0.5)};
  }

  std::vector<int> recognize(std::vector<int> corrupt_pattern) {
    int max_iterations{50};
    for (int i = 0; i < max_iterations; ++i) {
        std::vector<int> corrupt_previous{corrupt_pattern};
        evolve(corrupt_pattern);

        std::cout << "Pattern nergy: " << get_energy(corrupt_pattern) << "\n\n";

        if (corrupt_pattern == corrupt_previous) {
            return corrupt_pattern;
        }
    }
    return corrupt_pattern;
}
};

sf::Sprite image_loading() {
  const char* path = tinyfd_openFileDialog("Seleziona un'immagine", "", 0,
                                           nullptr, "File immagine", 0);

  if (!path) {
    std::cerr << "Nessun file selezionato!" << "\n";
  }

  sf::Texture texture;
  if (!texture.loadFromFile(path)) {
    std::cerr << "Errore nel caricamento della texture!" << "\n";
  }
  sf::Sprite sprite(texture);
  return sprite;
}



int main() {
  /*sf::Texture tralla_text;
  tralla_text.loadFromFile("img/trallallero.png");
  auto N_base{tralla_text.getSize().x};
  auto N_height{tralla_text.getSize().y};
  sf::Texture orang_text;
  orang_text.loadFromFile("img/orang.jpg");
  sf::Sprite tralla_s(tralla_text);
  sf::Sprite orang_s(orang_text);
  std::vector<int> pattern1{
      get_pattern(N_base, N_height, bw_conversion(N_base, N_height, tralla_s))};
  auto orang_new{image_conversion(N_base, N_height, tralla_s, orang_s)};
  std::vector<int> pattern2{get_pattern(N_base, N_height, orang_new)};
  std::vector<std::vector<int>> patterns{pattern1, pattern2};
  auto matrix{mat_2D(N_base, patterns)};

  auto matrix_copy{mat_from_file()};
  if (matrix_copy != matrix) {
    std::cout << "Errato..." << "\n";
  } else {
    std::cout << "Daje" << "\n";
  }*/
}