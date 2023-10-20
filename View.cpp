#include "View.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>

#include "SFML/Graphics/Color.hpp"

namespace {
const auto f_frameColor{sf::Color::Black};
const auto f_livingCellColor{sf::Color::White};
const auto f_deadCellColor{sf::Color{80, 80, 80}};
const auto f_emptyCellColor{sf::Color{40, 40, 40}};
const auto f_simpleTextBoxFillColor{sf::Color::Black};
const auto f_simpleTextBoxOutlineColor{sf::Color::Black};
const auto f_simpleTextBoxTextColor{sf::Color::White};
const auto f_clickedButtonFillColor{sf::Color::Black};
const auto f_clickedButtonOutlineColor{sf::Color::White};
const auto f_clickedButtonTextColor{sf::Color::White};
const auto f_highlightedButtonFilledColor{sf::Color::White};
const auto f_highlightedButtonOutlineColor{sf::Color::White};
const auto f_highlightedButtonTextColor{sf::Color::Black};
const auto f_unclickedButtonFillColor{sf::Color{40, 40, 40}};
const auto f_unclickedButtonOutlineColor{sf::Color::Black};
const auto f_unclickedButtonTextColor{sf::Color::White};
const auto f_displayTextBoxFillColor{sf::Color::Black};
const auto f_displayTextBoxOutlineColor{sf::Color::White};
const auto f_displayTextBoxTextFillColor{sf::Color::White};
constexpr auto f_fontPath{"../resources/futura.ttf"};
constexpr auto f_frameHorizontalThickness{50.f};
constexpr auto f_frameVerticalThickness{5.f};
constexpr auto f_fontSize{18};
constexpr auto f_cellOutlineThickness{1.f};
constexpr auto f_buttonOutlineThickness{1.f};
constexpr auto f_minZoomLevel{1.f};
constexpr auto f_maxZoomLevel{5.f};
constexpr auto f_zoomSensibility{1.f};
constexpr auto f_textBoxTextVerticalPosition{12.f};
}  // namespace

View::View(sf::RenderWindow &window, Model &model)
    : m_model{model},
      m_window{window},
      m_viewOffset{f_frameVerticalThickness, f_frameHorizontalThickness},
      m_font{},
      m_highlightedButton{},
      m_zoomLevel{f_minZoomLevel} {
  m_font.loadFromFile(f_fontPath);
}

void View::update() {
  m_window.clear();
  m_highlightedButton.reset();
  drawCells();
  drawFrame();
  drawBottomLeftMenu();
  drawBottomRightMenu();
  drawTopLeftMenu();
  drawTopRightMenu();
  m_window.display();
}

void View::zoomIn() { setZoomLevel(m_zoomLevel + f_zoomSensibility); }

void View::zoomOut() { setZoomLevel(m_zoomLevel - f_zoomSensibility); }

void View::closeWindow() { m_window.close(); }

void View::dragView(sf::Vector2i offset) {
  setViewOffset({m_viewOffset.x + static_cast<float>(offset.x),
                 m_viewOffset.y + static_cast<float>(offset.y)});
}

std::optional<View::Button> View::highlightedButton() const {
  return m_highlightedButton;
}

std::optional<Model::Cell> View::pixelToCell(sf::Vector2i pixel) const {
  Model::Cell cell{};
  auto coord = m_window.mapPixelToCoords(pixel);
  auto windowSize{m_window.getView().getSize()};
  if (coord.x < f_frameVerticalThickness ||
      coord.x > (windowSize.x - f_frameVerticalThickness) ||
      coord.y < f_frameHorizontalThickness ||
      coord.y > (windowSize.y - f_frameHorizontalThickness)) {
    return {};
  }
  auto cellSize{calculateCellSize()};
  return {{(coord.x - m_viewOffset.x) / cellSize.x,
           (coord.y - m_viewOffset.y) / cellSize.y}};
}

void View::drawFrame() {
  auto windowSize{static_cast<sf::Vector2f>(m_window.getView().getSize())};
  sf::RectangleShape rect{{windowSize.x, f_frameHorizontalThickness}};
  rect.setFillColor(f_frameColor);
  m_window.draw(rect);
  rect.setSize({windowSize.x, f_frameHorizontalThickness});
  rect.setPosition(0, windowSize.y - f_frameHorizontalThickness);
  m_window.draw(rect);
  rect.setSize({f_frameVerticalThickness, windowSize.y});
  rect.setPosition(0, 0);
  m_window.draw(rect);
  rect.setPosition(windowSize.x - f_frameVerticalThickness, 0);
  m_window.draw(rect);
}

void View::drawCells() {
  auto cellSize{calculateCellSize()};
  sf::RectangleShape cell{cellSize};
  cell.setOutlineThickness(f_cellOutlineThickness);
  cell.setOutlineColor(f_frameColor);
  for (std::size_t x = 0; x < m_model.width(); x++) {
    for (std::size_t y = 0; y < m_model.height(); y++) {
      cell.setPosition(static_cast<float>(x) * cellSize.x + m_viewOffset.x,
                       static_cast<float>(y) * cellSize.y + m_viewOffset.y);
      if (m_model.livingCells().count({x, y}) > 0) {
        cell.setFillColor(f_livingCellColor);
      } else if (m_model.deadCells().count({x, y}) > 0) {
        cell.setFillColor(f_deadCellColor);
      } else {
        cell.setFillColor(f_emptyCellColor);
      }
      m_window.draw(cell);
    }
  }
}

void View::drawBottomLeftMenu() {
  auto x{f_frameVerticalThickness};
  auto y{static_cast<float>(m_window.getView().getSize().y) -
         f_frameHorizontalThickness + f_buttonOutlineThickness};
  auto width{140.f};
  if (m_model.status() != Model::Status::Running) {
    if (drawTextBox("Run(space)", {x, y}, width, TextBoxStyle::Button)) {
      m_highlightedButton = Button::Run;
    }
  } else {
    if (drawTextBox("Pause(space)", {x, y}, width, TextBoxStyle::Button)) {
      m_highlightedButton = Button::Pause;
    }
  }
  x += width;
  width = 260.f;
  if (drawTextBox("Generate Population(G)", {x, y}, width,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::GeneratePopulation;
  }
  x += width;
  width = 110.f;
  if (drawTextBox("Reset(R)", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::Reset;
  }
  x += width;
  width = 300.f;
  drawTextBox("Add/RemoveCell(Mouse Left)", {x, y}, width,
              TextBoxStyle::Simple);
  x += width;
  width = 250.f;
  drawTextBox("Drag View(Mouse Right)", {x, y}, width, TextBoxStyle::Simple);
}

void View::drawBottomRightMenu() {
  auto x{m_window.getView().getSize().x - f_frameVerticalThickness};
  auto y{static_cast<float>(m_window.getView().getSize().y) -
         f_frameHorizontalThickness + f_buttonOutlineThickness};
  auto width{90.f};
  x -= width;
  drawTextBox(std::to_string(m_model.livingCells().size()), {x, y}, width,
              TextBoxStyle::Display);
  width = 130.f;
  x -= width;
  drawTextBox("Population", {x, y}, width, TextBoxStyle::Simple);
  width = 90.f;
  x -= width;
  drawTextBox(std::to_string(m_model.generation()), {x, y}, width,
              TextBoxStyle::Display);
  width = 130.f;
  x -= width;
  drawTextBox("Generation", {x, y}, width, TextBoxStyle::Simple);
}

void View::drawTopRightMenu() {
  auto x{m_window.getView().getSize().x - f_frameVerticalThickness};
  auto y{f_buttonOutlineThickness};
  auto width{90.f};
  x -= width;
  drawTextBox(std::to_string(m_model.speed()) + "gen/s", {x, y}, width,
              TextBoxStyle::Display);
  width = 50.f;
  x -= width;
  if (drawTextBox("+", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::SpeedUp;
  }
  width = 50.f;
  x -= width;
  if (drawTextBox("-", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::SlowDown;
  }
  width = 200.f;
  x -= width;
  drawTextBox("Speed(Left/Right)", {x, y}, width, TextBoxStyle::Simple);
  width = 90.f;
  x -= width;
  drawTextBox(std::to_string(static_cast<int>(m_zoomLevel)) + "x", {x, y},
              width, TextBoxStyle::Display);
  width = 50.f;
  x -= width;
  if (drawTextBox("+", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::ZoomIn;
  }
  width = 50.f;
  x -= width;
  if (drawTextBox("-", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::ZoomOut;
  }
  width = 230.f;
  x -= width;
  drawTextBox("Zoom(Mouse Wheel)", {x, y}, width, TextBoxStyle::Simple);
  // TODO:
  // if (drawButton("Mesh Size(Up)", {x, y}, width, true)) {
  //   blabla;
  // }
  // x += width;
  // if (drawButton("Mesh Size(Down)", {x, y}, width, true)) {
  //   blabla;
  // }
}

void View::drawTopLeftMenu() {
  auto x{f_frameVerticalThickness};
  auto y{f_buttonOutlineThickness};
  auto width{120.f};
  if (drawTextBox("Quit(Esc)", {x, y}, width, TextBoxStyle::Button)) {
    m_highlightedButton = Button::Quit;
  }
  x += width;
  width = 140.f;
  // TODO:
  // if (drawButton("load file(l)", {x, y}, width, true)) {
  //   m_highlightedButton = Button::LoadFile;
  // }
  // x += width;
  // if (drawButton("save file(s)", {x, y}, width, true)) {
  //   m_highlightedButton = Button::SaveFile;
  // }
}

bool View::drawTextBox(const std::string &content, const sf::Vector2f &position,
                       float width, TextBoxStyle style) {
  auto highlighted{false};
  auto height{f_frameHorizontalThickness};
  sf::RectangleShape rect{{width - 2 * f_buttonOutlineThickness,
                           height - 2 * f_buttonOutlineThickness}};
  rect.setPosition(position.x, position.y);
  rect.setOutlineThickness(f_buttonOutlineThickness);
  sf::Text text{content, m_font};
  text.setCharacterSize(f_fontSize);
  text.setPosition(position.x + (width - text.getLocalBounds().width) * .5f,
                   position.y + f_textBoxTextVerticalPosition);

  switch (style) {
    case TextBoxStyle::Button:
      if (rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(
              m_window.mapPixelToCoords(sf::Mouse::getPosition())))) {
        highlighted = true;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
          rect.setFillColor(f_clickedButtonFillColor);
          rect.setOutlineColor(f_clickedButtonOutlineColor);
          text.setFillColor(f_clickedButtonTextColor);
        } else {
          rect.setFillColor(f_highlightedButtonFilledColor);
          rect.setOutlineColor(f_highlightedButtonOutlineColor);
          text.setFillColor(f_highlightedButtonTextColor);
        }
      } else {
        rect.setFillColor(f_unclickedButtonFillColor);
        rect.setOutlineColor(f_unclickedButtonOutlineColor);
        text.setFillColor(f_unclickedButtonTextColor);
      }
      break;
    case TextBoxStyle::Display:
      rect.setFillColor(f_displayTextBoxFillColor);
      rect.setOutlineColor(f_displayTextBoxOutlineColor);
      text.setFillColor(f_displayTextBoxTextFillColor);
      break;
    case TextBoxStyle::Simple:
    default:
      rect.setFillColor(f_simpleTextBoxFillColor);
      rect.setOutlineColor(f_unclickedButtonOutlineColor);
      text.setFillColor(f_simpleTextBoxTextColor);
      break;
  }
  m_window.draw(rect);
  m_window.draw(text);
  return highlighted;
}

void View::setZoomLevel(float zoomLevel) {
  auto windowSize{m_window.getView().getSize()};
  auto cellAtCentre{
      pixelToCell({(static_cast<int>(windowSize.x * .5)),
                   static_cast<int>(f_frameHorizontalThickness +
                                    (windowSize.y - f_frameHorizontalThickness -
                                     f_frameHorizontalThickness) *
                                        .5)})
          .value()};
  m_zoomLevel = std::min(f_maxZoomLevel, std::max(f_minZoomLevel, zoomLevel));
  auto cellSize{calculateCellSize()};
  setViewOffset(
      {-(static_cast<float>(cellAtCentre.first) + .5f) * cellSize.x +
           static_cast<float>(windowSize.x) * .5f,
       -(static_cast<float>(cellAtCentre.second) + 0.5f) * cellSize.y +
           static_cast<float>(windowSize.y) * .5f});
}

void View::setViewOffset(const sf::Vector2f &position) {
  auto windowSize{m_window.getView().getSize()};
  auto cellSize{calculateCellSize()};
  sf::Vector2f minOffset{static_cast<float>(windowSize.x) -
                             cellSize.x * static_cast<float>(m_model.width()) -
                             f_frameVerticalThickness,
                         static_cast<float>(windowSize.y) -
                             cellSize.y * static_cast<float>(m_model.height()) -
                             f_frameHorizontalThickness};
  m_viewOffset.x = std::min(static_cast<float>(f_frameVerticalThickness),
                            std::max(position.x, minOffset.x));
  m_viewOffset.y =
      std::min(f_frameHorizontalThickness, std::max(position.y, minOffset.y));
}

sf::Vector2f View::calculateCellSize() const {
  auto windowSize{m_window.getView().getSize()};
  return {
      m_zoomLevel *
          (static_cast<float>(windowSize.x) - 2 * f_frameVerticalThickness) /
          static_cast<float>(m_model.width()),
      m_zoomLevel *
          (static_cast<float>(windowSize.y) - f_frameHorizontalThickness -
           f_frameHorizontalThickness) /
          static_cast<float>(m_model.height())};
}

sf::Vector2f View::calculateCellPosition(Model::Cell cell) const {
  auto cellSize{calculateCellSize()};
  return {static_cast<float>(cell.first) * cellSize.x +
              f_frameHorizontalThickness  //
              + m_viewOffset.x,
          static_cast<float>(cell.second) * cellSize.y +
              f_frameHorizontalThickness  //
              + m_viewOffset.y};
}
