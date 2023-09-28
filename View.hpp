#ifndef GAME_OF_LIFE_VIEW_HPP
#define GAME_OF_LIFE_VIEW_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>

#include "Model.hpp"

class View {
 public:
  View(sf::RenderWindow &window, Model &model);

  void update();
  void zoomIn();
  void zoomOut();
  void closeWindow();
  void dragView(sf::Vector2i offset);
  std::optional<Model::Cell> pixelToCell(sf::Vector2i pixel);

 private:
  void drawFrame();
  void drawCells();
  void drawIcon();
  void drawPauseIcon();
  void drawPlayIcon();
  void drawZoomIndicator();
  void drawSpeedIndicator();
  void drawGenerationIndicator();
  void drawPopulationIndicator();
  void drawTitle();
  void drawLegend();
  void setViewOffset(const sf::Vector2f &offset);
  void setZoomLevel(float zoomLevel);

  sf::Vector2f calculateCellSize();
  sf::Vector2f calculateCellPosition(Model::Cell cell);

  Model &m_model;
  sf::RenderWindow &m_window;
  sf::Vector2f m_viewOffset;
  float m_zoomLevel;
  sf::Font m_font;
};

#endif
