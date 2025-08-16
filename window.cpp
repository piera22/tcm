#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <sstream>

#include "tinyfiledialogs.hpp"


int main() {
  sf::RenderWindow window(sf::VideoMode(1280, 720), "Hopfield", sf::Style::Default);
  const sf::Vector2f& button_size{350.f, 120.f};
  sf::RectangleShape button_load(button_size);
  sf::RectangleShape button_save(button_size);
  sf::RectangleShape button_corruptrand(button_size);
  sf::RectangleShape button_corruptnorm(button_size);
  sf::RectangleShape button_recognize(button_size);

  sf::RectangleShape line(sf::Vector2f(10.f, 720.f));

  sf::RectangleShape img_1(sf::Vector2f(400.f, 400.f));
  sf::RectangleShape img_2(sf::Vector2f(400.f, 400.f));

  img_1.setPosition(sf::Vector2f(15.f, 160.f));
  img_2.setPosition(sf::Vector2f(465.f, 160.f));

  line.setPosition(sf::Vector2f(880.f, 0.f));

  button_load.setPosition(sf::Vector2f(910.f, 20.f));
  button_save.setPosition(sf::Vector2f(910.f, 160.f));
  button_corruptrand.setPosition(sf::Vector2f(910.f, 300.f));
  button_corruptnorm.setPosition(sf::Vector2f(910.f, 440.f));
  button_recognize.setPosition(sf::Vector2f(910.f, 580.f));

  sf::Font font;
  font.loadFromFile("ARIAL.TTF");

  sf::Text load_text;
  load_text.setFont(font);
  load_text.setString("Carica immagine");
  load_text.setCharacterSize(30);
  load_text.setFillColor(sf::Color::Black);
  load_text.setPosition(sf::Vector2f(930.f, 60.f));
  
  sf::Text save_text;
  save_text.setFont(font);
  save_text.setString("Salva immagine");
  save_text.setCharacterSize(30);
  save_text.setFillColor(sf::Color::Black);
  save_text.setPosition(sf::Vector2f(930.f, 200.f));

  sf::Text corruptrand_text;
  corruptrand_text.setFont(font);
  corruptrand_text.setString("Corrompi casualmente");
  corruptrand_text.setCharacterSize(30);
  corruptrand_text.setFillColor(sf::Color::Black);
  corruptrand_text.setPosition(sf::Vector2f(930.f, 340.f));

  sf::Text corruptnorm_text1;
  corruptnorm_text1.setFont(font);
  corruptnorm_text1.setString("Corrompi con");
  corruptnorm_text1.setCharacterSize(30);
  corruptnorm_text1.setFillColor(sf::Color::Black);
  corruptnorm_text1.setPosition(sf::Vector2f(930.f, 465.f));

  sf::Text corruptnorm_text2;
  corruptnorm_text2.setFont(font);
  corruptnorm_text2.setString("distribuzione normale");
  corruptnorm_text2.setCharacterSize(30);
  corruptnorm_text2.setFillColor(sf::Color::Black);
  corruptnorm_text2.setPosition(sf::Vector2f(930.f, 495.f));

  sf::Text recognize_text;
  recognize_text.setFont(font);
  recognize_text.setString("Riconosci immagine");
  recognize_text.setCharacterSize(30);
  recognize_text.setFillColor(sf::Color::Black);
  recognize_text.setPosition(sf::Vector2f(930.f, 620.f));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.draw(img_1);
    window.draw(img_2);
    window.draw(line);
    window.draw(button_load);
    window.draw(button_save);
    window.draw(button_corruptrand);
    window.draw(button_corruptnorm);
    window.draw(button_recognize);
    window.draw(load_text);
    window.draw(save_text);
    window.draw(corruptrand_text);
    window.draw(corruptnorm_text1);
    window.draw(corruptnorm_text2);
    window.draw(recognize_text);
    window.display();
  }
  
}