#ifndef GAME_OF_LIFE_VIEW_HPP
#define GAME_OF_LIFE_VIEW_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>

#include "Model.hpp"

class View {
 public:
  enum class Mode { Main, LoadFile, SaveFile };
  enum class Button {
    Quit,
    LoadFileMenu,
    SaveFileMenu,
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
    Back,
    None
  };

  View(sf::RenderWindow &window, Model &model);

  void update();
  void zoomIn();
  void zoomOut();
  void scrollDown();
  void scrollUp();
  void pageDown();
  void pageUp();
  void closeWindow();
  void setMode(View::Mode mode);
  void dragView(sf::Vector2i offset);
  void setFileNameToSave(const std::string &name);

  Mode mode() const;
  Button highlightedButton() const;
  std::optional<std::string> highlightedLoadFileMenuItem() const;
  std::optional<Cell> highlightedCell() const;
  const std::string &fileNameToSave() const;

 private:
  enum class TextBoxStyle { Simple, Display, Button, Hidden };

  void drawMainScreen();
  void drawLoadFileScreen();
  void drawSaveFileScreen();
  void drawFrame();
  void drawBackground();
  void drawGrid();
  void drawCells();
  void drawBottomLeftMenu();
  void drawBottomRightMenu();
  void drawTopLeftMenu();
  void drawTopRightMenu();
  void drawCells(const std::set<Cell> &cells, const sf::Color &color);
  bool drawTextBox(const std::string &content, const sf::Vector2f &position,
                   float width, TextBoxStyle style);
  void applyViewOffset(const sf::Vector2f &offset);
  void applyZoomLevel(int zoomLevel);

  sf::Vector2f calculateCellSize() const;
  sf::Vector2f calculateCellPosition(const Cell &cell) const;
  std::optional<Cell> cellAtCoord(sf::Vector2f coord) const;

  Model &m_model;
  View::Mode m_mode;
  sf::RenderWindow &m_window;
  sf::Vector2f m_viewOffset;
  sf::Font m_font;
  View::Button m_highlightedButton;
  std::optional<std::string> m_highlightedLoadFileMenuItem;
  int m_zoomLevel;
  int m_scrollPos;
  std::string m_fileNameToSave;
};

#endif
