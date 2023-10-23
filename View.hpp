#ifndef GAME_OF_LIFE_VIEW_HPP
#define GAME_OF_LIFE_VIEW_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>

#include "Model.hpp"

class View {
 public:
  enum class Mode { Main, LoadFile, SaveFile };
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
    ReduceSize,
    Back
  };

  View(sf::RenderWindow &window, Model &model);

  void update();
  void zoomIn();
  void zoomOut();
  void scrollDown();
  void scrollUp();
  void pageUp();
  void pageDown();
  void closeWindow();
  void setMode(View::Mode mode);
  void dragView(sf::Vector2i offset);

  Mode mode() const;
  std::optional<View::Button> highlightedButton() const;
  std::optional<std::string> highlightedLoadFileMenuItem() const;
  std::optional<sf::Vector2i> pixelToCellPosition(sf::Vector2i pixel) const;

 private:
  enum class TextBoxStyle { Simple, Display, Button, Hidden };

  void drawFrame();
  void drawCells();
  void drawBottomRightMenu();
  void drawBottomLeftMenu();
  void drawTopRightMenu();
  void drawTopLeftMenu();
  void drawLoadFileMenu();
  void drawSaveFileMenu();
  bool drawTextBox(const std::string &content, const sf::Vector2f &position,
                   float width, TextBoxStyle style);
  void setViewOffset(const sf::Vector2f &offset);
  void setZoomLevel(float zoomLevel);

  sf::Vector2f calculateCellSize() const;
  sf::Vector2f calculateCellPosition(Cell cell) const;

  const std::set<std::string> m_loadFileMenuItems;

  Model &m_model;
  View::Mode m_mode;
  sf::RenderWindow &m_window;
  sf::Vector2f m_viewOffset;
  sf::Font m_font;
  std::optional<View::Button> m_highlightedButton;
  std::optional<std::string> m_highlightedLoadFileMenuItem;
  float m_zoomLevel;
  int m_scrollPos;
};

#endif
