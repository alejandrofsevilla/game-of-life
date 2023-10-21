#ifndef GAME_OF_LIFE_CONTROLLER_HPP
#define GAME_OF_LIFE_CONTROLLER_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include "Model.hpp"
#include "View.hpp"

class Controller {
 public:
  Controller(View &view, Model &model);

  void onEvent(const sf::Event &event);
  void update();

 private:
  void onMouseButtonPressed(const sf::Event::MouseButtonEvent &event);
  void onMainModeMouseButtonPressed(const sf::Event::MouseButtonEvent &event);
  void onLoadFileModeMouseButtonPressed(
      const sf::Event::MouseButtonEvent &event);
  void onSaveFileModeMouseButtonPressed(
      const sf::Event::MouseButtonEvent &event);
  void onMouseWheelScrolled(const sf::Event::MouseWheelScrollEvent &event);
  void onMouseMoved(const sf::Event::MouseMoveEvent &event);
  void onKeyPressed(const sf::Event::KeyEvent &event);
  void onMainModeKeyPressed(const sf::Event::KeyEvent &event);
  void onLoadFileModeKeyPressed(const sf::Event::KeyEvent &event);
  void onSaveFileModeKeyPressed(const sf::Event::KeyEvent &event);

  View &m_view;
  Model &m_model;
  sf::Vector2i m_mouseReferencePosition;
};

#endif
