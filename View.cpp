#include "View.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace {
const auto f_frameColor{sf::Color::Black};
const auto f_textColor{sf::Color::White};
const auto f_iconColor{sf::Color::White};
const auto f_livingCellColor{sf::Color::White};
const auto f_deadCellColor{sf::Color{100, 100, 100}};
const auto f_emptyCellColor{sf::Color{50, 50, 50}};
constexpr auto f_fontPath{"../resources/font.ttf"};
constexpr auto f_topBorderSize{50};
constexpr auto f_sideBorderSize{5};
constexpr auto f_bottomBorderSize{50};
constexpr auto f_titleFontSize{40};
constexpr auto f_titleYPosition{-4.f};
constexpr auto f_legendFontSize{22.f};
constexpr auto f_legendYPositionFromBottom{40};
constexpr auto f_cellOutlineThickness{1.f};
constexpr auto f_minZoomLevel{1.f};
constexpr auto f_maxZoomLevel{5.f};
constexpr auto f_zoomSensibility{1.f};
constexpr auto f_pauseIconYPosition{14.f};
constexpr auto f_pauseIconHeight{20.f};
constexpr auto f_pauseIconWidth{6.f};
constexpr auto f_playIconSize{12.f};
constexpr auto f_playIconYPosition{14.f};
constexpr auto f_zoomIndXPositionFromRight{315};
constexpr auto f_SpeedIndXPositionFromRight{175};
constexpr auto f_genIndXPositionFromRight{680};
constexpr auto f_popIndXPositionFromRight{495};
constexpr auto f_indicatorFontSize{22.f};
constexpr auto f_indicatorYPosition{10};
constexpr auto f_titleText{"Game Of Life"};
constexpr auto f_legendText{
    "Start/Pause(Space)"
    "     Generate Population(G)"
    "     Reset(R)"
    "     Control Speed(Left/Right)"
    "     Add/Remove Cell(Mouse Left)"
    "     Drag View(Mouse Right)"
    "     Zoom(Mouse Wheel)"
    "     Exit(Esc)"};
}  // namespace

View::View(sf::RenderWindow &window, Model &model)
    : m_model{model},
      m_window{window},
      m_viewOffset{f_sideBorderSize, f_topBorderSize},
      m_zoomLevel{f_minZoomLevel},
      m_font{} {
  m_font.loadFromFile(f_fontPath);
}

void View::update() {
  m_window.clear();
  drawCells();
  drawFrame();
  drawTitle();
  drawIcon();
  drawZoomIndicator();
  drawSpeedIndicator();
  drawGenerationIndicator();
  drawPopulationIndicator();
  drawLegend();
  m_window.display();
}

void View::closeWindow() { m_window.close(); }

void View::zoomIn() { setZoomLevel(m_zoomLevel + f_zoomSensibility); }

void View::zoomOut() { setZoomLevel(m_zoomLevel - f_zoomSensibility); }

void View::dragView(sf::Vector2i offset) {
  setViewOffset({m_viewOffset.x + static_cast<float>(offset.x),
                 m_viewOffset.y + static_cast<float>(offset.y)});
}

std::optional<Model::Cell> View::pixelToCell(sf::Vector2i pixel) {
  Model::Cell cell{};
  auto coord = static_cast<sf::Vector2i>(m_window.mapPixelToCoords(pixel));
  auto windowSize{static_cast<sf::Vector2i>(m_window.getView().getSize())};
  if (coord.x < f_sideBorderSize ||
      coord.x > (windowSize.x - f_sideBorderSize) ||
      coord.y < f_topBorderSize ||
      coord.y > (windowSize.y - f_bottomBorderSize)) {
    return {};
  }
  auto cellSize{calculateCellSize()};
  return {{(static_cast<float>(coord.x) - m_viewOffset.x) / cellSize.x,
           (static_cast<float>(coord.y) - m_viewOffset.y) / cellSize.y}};
}

void View::drawFrame() {
  auto windowSize{m_window.getView().getSize()};
  sf::RectangleShape rect{{static_cast<float>(windowSize.x), f_topBorderSize}};
  rect.setFillColor(f_frameColor);
  m_window.draw(rect);
  rect.setSize({static_cast<float>(windowSize.x), f_bottomBorderSize});
  rect.setPosition(0, static_cast<float>(windowSize.y) - f_bottomBorderSize);
  m_window.draw(rect);
  rect.setSize({f_sideBorderSize, static_cast<float>(windowSize.y)});
  rect.setPosition(0, 0);
  m_window.draw(rect);
  rect.setPosition(static_cast<float>(windowSize.x) - f_sideBorderSize, 0);
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

void View::drawIcon() {
  if (m_model.status() == Model::Status::Paused) {
    drawPauseIcon();
  } else {
    drawPlayIcon();
  }
}

void View::drawPauseIcon() {
  sf::RectangleShape rect({f_pauseIconWidth, f_pauseIconHeight});
  rect.setFillColor(f_iconColor);
  auto x{static_cast<float>(m_window.getView().getSize().x) -
         f_pauseIconYPosition - f_pauseIconWidth};
  rect.setPosition(x, f_pauseIconYPosition);
  m_window.draw(rect);
  rect.setPosition(x - f_pauseIconWidth * 2, f_pauseIconYPosition);
  m_window.draw(rect);
}

void View::drawPlayIcon() {
  sf::CircleShape triangle(f_playIconSize, 3);
  triangle.rotate(90);
  triangle.setFillColor(f_iconColor);
  auto windowSize{m_window.getView().getSize()};
  triangle.setPosition(
      static_cast<float>(m_window.getView().getSize().x) - f_playIconYPosition,
      f_playIconYPosition);
  m_window.draw(triangle);
}

void View::drawZoomIndicator() {
  sf::Text text{"Zoom: " + std::to_string(static_cast<int>(m_zoomLevel)) + "x",
                m_font};
  text.setPosition(static_cast<float>(m_window.getView().getSize().x) -
                       f_zoomIndXPositionFromRight,
                   f_indicatorYPosition);
  text.setCharacterSize(f_indicatorFontSize);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::drawSpeedIndicator() {
  sf::Text text{"Speed: " + std::to_string(m_model.speed()) + "/s", m_font};
  text.setPosition(static_cast<float>(m_window.getView().getSize().x) -
                       f_SpeedIndXPositionFromRight,
                   f_indicatorYPosition);
  text.setCharacterSize(f_indicatorFontSize);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::drawGenerationIndicator() {
  sf::Text text{"Generation: " + std::to_string(m_model.generation()), m_font};
  text.setPosition(static_cast<float>(m_window.getView().getSize().x) -
                       f_genIndXPositionFromRight,
                   f_indicatorYPosition);
  text.setCharacterSize(f_indicatorFontSize);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::drawPopulationIndicator() {
  sf::Text text{"Population: " + std::to_string(m_model.livingCells().size()),
                m_font};
  text.setPosition(static_cast<float>(m_window.getView().getSize().x) -
                       f_popIndXPositionFromRight,
                   f_indicatorYPosition);
  text.setCharacterSize(f_indicatorFontSize);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::drawTitle() {
  sf::Text text{f_titleText, m_font};
  text.setPosition(f_sideBorderSize, f_titleYPosition);
  text.setCharacterSize(f_titleFontSize);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::drawLegend() {
  sf::Text text{f_legendText, m_font};
  text.setCharacterSize(f_legendFontSize);
  auto windowSize{m_window.getView().getSize()};
  text.setPosition(
      (static_cast<float>(windowSize.x) - text.getLocalBounds().width) * .5f,
      static_cast<float>(windowSize.y) - f_legendYPositionFromBottom);
  text.setFillColor(f_textColor);
  m_window.draw(text);
}

void View::setZoomLevel(float zoomLevel) {
  auto windowSize{m_window.getView().getSize()};
  auto cellAtCentre{
      pixelToCell(
          {(static_cast<int>(windowSize.x * .5)),
           static_cast<int>(
               f_topBorderSize +
               (windowSize.y - f_topBorderSize - f_bottomBorderSize) * .5)})
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
                             f_sideBorderSize,
                         static_cast<float>(windowSize.y) -
                             cellSize.y * static_cast<float>(m_model.height()) -
                             f_bottomBorderSize};
  m_viewOffset.x = std::min(static_cast<float>(f_sideBorderSize),
                            std::max(position.x, minOffset.x));
  m_viewOffset.y = std::min(static_cast<float>(f_topBorderSize),
                            std::max(position.y, minOffset.y));
}

sf::Vector2f View::calculateCellSize() {
  auto windowSize{m_window.getView().getSize()};
  return {m_zoomLevel *
              (static_cast<float>(windowSize.x) - 2 * f_sideBorderSize) /
              static_cast<float>(m_model.width()),
          m_zoomLevel *
              (static_cast<float>(windowSize.y) - f_topBorderSize -
               f_bottomBorderSize) /
              static_cast<float>(m_model.height())};
}

sf::Vector2f View::calculateCellPosition(Model::Cell cell) {
  auto cellSize{calculateCellSize()};
  return {static_cast<float>(cell.first) * cellSize.x  // + f_sideBorderSize //
              + m_viewOffset.x,
          static_cast<float>(cell.second) * cellSize.y  // + f_topBorderSize //
              + m_viewOffset.y};
}
