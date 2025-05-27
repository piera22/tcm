#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(1080, 720), "Biancallero Biancallà",
                          sf::Style::Default);

  // carica l'immagine come texture
  sf::Texture tralla_text;
  tralla_text.loadFromFile("img/trallallero.png");

  sf::Image tralla(
      tralla_text
          .copyToImage());  // salva l'immagine come Image per accedere ai pixel

  sf::Sprite tralla_s(
      tralla_text);  // salva l'immagine su uno sprite per disegnarla

  // ottengo le dimensioni dell'immagine
  auto N_base{tralla_text.getSize().x};
  auto N_height{tralla_text.getSize().y};

  tralla_s.setPosition(sf::Vector2f(10.f, 10.f));  // dove disegnare lo sprite

  sf::VertexArray image_bw(
      sf::Points, N_base * N_height);  // l'immagine in bianco e nero è un'array
                                       // di punti (cioè pixel)

  for (int i{0}; i < N_height; ++i) {
    for (int j{0}; j < N_base; ++j) {
      image_bw[N_height * i + j].position = sf::Vector2f(
          600.f + j, 10.f + i);  // per disegnare l'immagine in bianco e nero
                                 // seguendo l'ordine dei pixel dell'originale
      (tralla.getPixel(j, i).r + tralla.getPixel(j, i).g +
       tralla.getPixel(j, i).b) /
                  3 >=
              127
          ? image_bw[N_height * i + j].color = sf::Color::White
          : image_bw[N_height * i + j].color = sf::Color::Black; // per convertire il pixel in bianco o nero
    }
  }
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.draw(tralla_s);
    window.draw(image_bw);
    window.display();
  }
}