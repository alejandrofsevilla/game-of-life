#ifndef GAME_OF_LIFE_VIEW_HPP
#define GAME_OF_LIFE_VIEW_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>

#include "Model.hpp"

class View {
 public:
  enum class Button {
    Quit,
    LoadFile,
    SaveFile,
    ZoomOut,
    ZoomIn,
    SpeedUp,
    SlowDown,
    Run,
    Pause,
    Reset,
    Clear,
    GeneratePopulation,
    IncreaseSize,
    ReduceSize
  };

  enum class TextBoxStyle { Simple, Display, Button, Hidden };

  View(sf::RenderWindow &window, Model &model);

  void update();
  void zoomIn();
  void zoomOut();
  void closeWindow();
  void dragView(sf::Vector2i offset);

  std::optional<View::Button> highlightedButton() const;
  std::optional<Model::Cell> pixelToCell(sf::Vector2i pixel) const;

 private:
  void drawFrame();
  void drawCells();
  void drawBottomRightMenu();
  void drawBottomLeftMenu();
  void drawTopRightMenu();
  void drawTopLeftMenu();
  bool drawTextBox(const std::string &content, const sf::Vector2f &position,
                   float width, TextBoxStyle style);
  void setViewOffset(const sf::Vector2f &offset);
  void setZoomLevel(float zoomLevel);

  sf::Vector2f calculateCellSize() const;
  sf::Vector2f calculateCellPosition(Model::Cell cell) const;

  Model &m_model;
  sf::RenderWindow &m_window;
  sf::Vector2f m_viewOffset;
  sf::Font m_font;
  std::optional<View::Button> m_highlightedButton;
  float m_zoomLevel;
};

#endif
