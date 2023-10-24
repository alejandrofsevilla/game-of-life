#include "View.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>

#include "RleHelper.hpp"

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
const auto f_hiddenTextBoxFillColor{sf::Color::Black};
const auto f_hiddenTextBoxOutlineColor{sf::Color::Black};
const auto f_hiddenTextBoxTextColor{sf::Color{80, 80, 80}};
constexpr auto f_fontPath{"../resources/futura.ttf"};
constexpr auto f_frameHorizontalThickness{50.f};
constexpr auto f_frameVerticalThickness{1.f};
constexpr auto f_fontSize{18};
constexpr auto f_cellOutlineThickness{1.f};
constexpr auto f_textBoxOutlineThickness{1.f};
constexpr auto f_textBoxHeight{f_frameHorizontalThickness};
constexpr auto f_minZoomLevel{1.f};
constexpr auto f_maxZoomLevel{10.f};
constexpr auto f_zoomSensibility{1.f};
constexpr auto f_textBoxTextVerticalPosition{12.f};
constexpr auto f_startButtonWidth{180.f};
constexpr auto f_resetButtonWidth{130.f};
constexpr auto f_clearButtonWidth{130.f};
constexpr auto f_generatePopButtonWidth{240.f};
constexpr auto f_addRemoveCellButtonWidth{280.f};
constexpr auto f_dragViewButtonWidth{230.f};
constexpr auto f_displayBoxWidth{90.f};
constexpr auto f_generationButtonWidth{130.f};
constexpr auto f_populationButtonWidth{130.f};
constexpr auto f_plusMinusButtonWidth{50.f};
constexpr auto f_speedButtonWidth{200.f};
constexpr auto f_zoomButtonWidth{230.f};
constexpr auto f_sizeButtonWidth{190.f};
constexpr auto f_quitButtonWidth{120.f};
constexpr auto f_loadButtonWidth{140.f};
constexpr auto f_saveButtonWidth{140.f};
constexpr auto f_backButtonWidth{140.f};
constexpr auto f_pageUpDownButtonWidth{280.f};
constexpr auto f_scrollUpDownButtonWidth{280.f};
constexpr auto f_showHideGridButton{170.f};
}  // namespace

View::View(sf::RenderWindow &window, Model &model)
    : m_loadFileMenuItems{rle::listPatternNames()},
      m_model{model},
      m_mode{Mode::Main},
      m_window{window},
      m_viewOffset{f_frameVerticalThickness, f_frameHorizontalThickness},
      m_font{},
      m_highlightedButton{},
      m_highlightedLoadFileMenuItem{},
      m_zoomLevel{f_minZoomLevel},
      m_scrollPos{},
      m_isGridVisible{true} {
  m_font.loadFromFile(f_fontPath);
}

void View::update() {
  m_window.clear();
  m_highlightedButton.reset();
  m_highlightedLoadFileMenuItem.reset();
  switch (m_mode) {
    case Mode::SaveFile:
      drawSaveFileMenu();
      break;
    case Mode::LoadFile:
      drawLoadFileMenu();
      break;
    case Mode::Main:
    default:
      m_scrollPos = 0;
      drawCells();
      drawFrame();
      drawBottomLeftMenu();
      drawBottomRightMenu();
      drawTopLeftMenu();
      drawTopRightMenu();
      break;
  }
  m_window.display();
}

void View::zoomIn() { setZoomLevel(m_zoomLevel + f_zoomSensibility); }

void View::zoomOut() { setZoomLevel(m_zoomLevel - f_zoomSensibility); }

void View::scrollDown() { m_scrollPos++; }

void View::scrollUp() { m_scrollPos = std::max(0, m_scrollPos - 1); }

void View::pageDown() {
  auto windowSize{static_cast<sf::Vector2f>(m_window.getView().getSize())};
  auto maxNumberOfItems{static_cast<int>(
      windowSize.y / (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  m_scrollPos = std::max(0, m_scrollPos + maxNumberOfItems);
}

void View::pageUp() {
  auto windowSize{static_cast<sf::Vector2f>(m_window.getView().getSize())};
  auto maxNumberOfItems{static_cast<int>(
      windowSize.y / (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  m_scrollPos = std::max(0, m_scrollPos - maxNumberOfItems);
}

void View::showGrid() { m_isGridVisible = true; }

void View::hideGrid() { m_isGridVisible = false; }

void View::closeWindow() { m_window.close(); }

void View::setMode(View::Mode mode) { m_mode = mode; }

void View::dragView(sf::Vector2i offset) {
  setViewOffset({m_viewOffset.x + static_cast<float>(offset.x),
                 m_viewOffset.y + static_cast<float>(offset.y)});
}

View::Mode View::mode() const { return m_mode; }

bool View::isGridVisible() const { return m_isGridVisible; }

std::optional<std::string> View::highlightedLoadFileMenuItem() const {
  return m_highlightedLoadFileMenuItem;
}

std::optional<View::Button> View::highlightedButton() const {
  return m_highlightedButton;
}

std::optional<sf::Vector2i> View::pixelToCellPosition(
    sf::Vector2i pixel) const {
  auto coord = m_window.mapPixelToCoords(pixel);
  auto windowSize{m_window.getView().getSize()};
  if (coord.x < f_frameVerticalThickness ||
      coord.x > (windowSize.x - f_frameVerticalThickness) ||
      coord.y < f_frameHorizontalThickness ||
      coord.y > (windowSize.y - f_frameHorizontalThickness)) {
    return {};
  }
  auto cellSize{calculateCellSize()};
  return {{static_cast<int>((coord.x - m_viewOffset.x) / cellSize.x),
           static_cast<int>(((coord.y - m_viewOffset.y) / cellSize.y))}};
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
  sf::RectangleShape rect{cellSize};
  rect.setOutlineThickness(f_cellOutlineThickness);
  rect.setOutlineColor(f_frameColor);
  auto cells{m_model.cells()};
  for (auto x = 0; x < m_model.width(); x++) {
    for (auto y = 0; y < m_model.height(); y++) {
      auto cell{cells.find({x, y})};
      if (cell == cells.end()) {
        if (!m_isGridVisible) {
          continue;
        }
        rect.setFillColor(f_emptyCellColor);
      } else if (cell->status == Cell::Status::Dead) {
        rect.setFillColor(f_deadCellColor);
      } else {
        rect.setFillColor(f_livingCellColor);
      }
      rect.setPosition(static_cast<float>(x) * cellSize.x + m_viewOffset.x,
                       static_cast<float>(y) * cellSize.y + m_viewOffset.y);
      m_window.draw(rect);
    }
  }
}

void View::drawBottomLeftMenu() {
  sf::Vector2f position{f_frameVerticalThickness,
                        static_cast<float>(m_window.getView().getSize().y) -
                            f_frameHorizontalThickness +
                            f_textBoxOutlineThickness};
  auto style{m_model.cells().empty() ? TextBoxStyle::Hidden
                                     : TextBoxStyle::Button};
  switch (m_model.status()) {
    case Model::Status::Stopped:
      if (drawTextBox("Start(space)", position, f_startButtonWidth, style)) {
        m_highlightedButton = Button::Run;
      }
      break;
    case Model::Status::Running:
      if (drawTextBox("Pause(space)", position, f_startButtonWidth, style)) {
        m_highlightedButton = Button::Pause;
      }
      break;
    case Model::Status::Paused:
      if (drawTextBox("Continue(Space)", position, f_startButtonWidth, style)) {
        m_highlightedButton = Button::Run;
      }
      break;
    default:
      break;
  }
  position.x += f_startButtonWidth;
  style = m_model.status() == Model::Status::Stopped ? TextBoxStyle::Hidden
                                                     : TextBoxStyle::Button;
  if (drawTextBox("Reset(R)", position, f_resetButtonWidth, style)) {
    m_highlightedButton = Button::Reset;
  }
  position.x += f_resetButtonWidth;
  if (drawTextBox("Clear(C)", position, f_clearButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Clear;
  }
  position.x += f_clearButtonWidth;
  style = m_model.status() == Model::Status::Stopped ? TextBoxStyle::Button
                                                     : TextBoxStyle::Hidden;
  if (drawTextBox("Generate Population(P)", position, f_generatePopButtonWidth,
                  style)) {
    m_highlightedButton = Button::GeneratePopulation;
  }
  position.x += f_generatePopButtonWidth;
  if (drawTextBox(m_isGridVisible ? "Hide Grid(H)" : "Show Grid(H)", position,
                  f_showHideGridButton, TextBoxStyle::Button)) {
    m_highlightedButton = m_isGridVisible ? Button::HideGrid : Button::ShowGrid;
  }
  position.x += f_showHideGridButton;
  style = m_model.status() == Model::Status::Stopped ? TextBoxStyle::Simple
                                                     : TextBoxStyle::Hidden;
  drawTextBox("Add/RemoveCell(Mouse Left)", position,
              f_addRemoveCellButtonWidth, style);
  position.x += f_addRemoveCellButtonWidth;
  drawTextBox("Drag View(Mouse Right)", position, f_dragViewButtonWidth,
              TextBoxStyle::Simple);
}

void View::drawBottomRightMenu() {
  sf::Vector2f position{
      m_window.getView().getSize().x - f_frameVerticalThickness,
      static_cast<float>(m_window.getView().getSize().y) -
          f_frameHorizontalThickness + f_textBoxOutlineThickness};
  position.x -= f_displayBoxWidth;
  drawTextBox(std::to_string(m_model.cells().size()), position,
              f_displayBoxWidth, TextBoxStyle::Display);
  position.x -= f_populationButtonWidth;
  drawTextBox("Population", position, f_populationButtonWidth,
              TextBoxStyle::Simple);
  position.x -= f_displayBoxWidth;
  drawTextBox(std::to_string(m_model.generation()), position, f_displayBoxWidth,
              TextBoxStyle::Display);
  position.x -= f_generationButtonWidth;
  drawTextBox("Generation", position, f_generationButtonWidth,
              TextBoxStyle::Simple);
}

void View::drawTopRightMenu() {
  sf::Vector2f position{
      m_window.getView().getSize().x - f_frameVerticalThickness,
      f_textBoxOutlineThickness};
  position.x -= f_displayBoxWidth;
  drawTextBox(std::to_string(m_model.speed()) + "x", position,
              f_displayBoxWidth, TextBoxStyle::Display);
  position.x -= f_plusMinusButtonWidth;
  if (drawTextBox("+", position, f_plusMinusButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::SpeedUp;
  }
  position.x -= f_plusMinusButtonWidth;
  if (drawTextBox("-", position, f_plusMinusButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::SlowDown;
  }
  position.x -= f_speedButtonWidth;
  drawTextBox("Speed(Left/Right)", position, f_speedButtonWidth,
              TextBoxStyle::Simple);
  position.x -= f_displayBoxWidth;
  drawTextBox(std::to_string(static_cast<int>(m_zoomLevel)) + "x", position,
              f_displayBoxWidth, TextBoxStyle::Display);
  position.x -= f_plusMinusButtonWidth;
  if (drawTextBox("+", position, f_plusMinusButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::ZoomIn;
  }
  position.x -= f_plusMinusButtonWidth;
  if (drawTextBox("-", position, f_plusMinusButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::ZoomOut;
  }
  position.x -= f_zoomButtonWidth;
  drawTextBox("Zoom(Mouse Wheel)", position, f_zoomButtonWidth,
              TextBoxStyle::Simple);
  position.x -= f_displayBoxWidth;
  drawTextBox(
      std::to_string(m_model.width()) + "x" + std::to_string(m_model.height()),
      position, f_displayBoxWidth, TextBoxStyle::Display);
  position.x -= f_plusMinusButtonWidth;
  auto style{
      (m_model.status() == Model::Status::Stopped && m_model.cells().empty())
          ? TextBoxStyle::Button
          : TextBoxStyle::Hidden};
  if (drawTextBox("+", position, f_plusMinusButtonWidth, style)) {
    m_highlightedButton = Button::IncreaseSize;
  }
  position.x -= f_plusMinusButtonWidth;
  if (drawTextBox("-", position, f_plusMinusButtonWidth, style)) {
    m_highlightedButton = Button::ReduceSize;
  }
  position.x -= f_sizeButtonWidth;
  style =
      (m_model.status() == Model::Status::Stopped && m_model.cells().empty())
          ? TextBoxStyle::Simple
          : TextBoxStyle::Hidden;
  drawTextBox("Size(Up/Down)", position, f_sizeButtonWidth, style);
}

void View::drawTopLeftMenu() {
  sf::Vector2f position{f_frameVerticalThickness, f_textBoxOutlineThickness};
  if (drawTextBox("Quit(Esc)", position, f_quitButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Quit;
  }
  position.x += f_quitButtonWidth;
  auto style{m_model.status() == Model::Status::Stopped ? TextBoxStyle::Button
                                                        : TextBoxStyle::Hidden};
  if (drawTextBox("Load File(L)", position, f_loadButtonWidth, style)) {
    m_highlightedButton = Button::LoadFile;
  }
  position.x += f_loadButtonWidth;
  if (drawTextBox("Save File(S)", position, f_saveButtonWidth, style)) {
    m_highlightedButton = Button::SaveFile;
  }
}

void View::drawLoadFileMenu() {
  sf::Vector2f position{f_textBoxOutlineThickness, f_textBoxOutlineThickness};
  if (drawTextBox("Back(Esc)", position, f_backButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Back;
  }
  position.x += f_backButtonWidth;
  drawTextBox("Scroll Up/Down(Mouse Wheel)", position,
              f_scrollUpDownButtonWidth, TextBoxStyle::Simple);
  position.x += f_scrollUpDownButtonWidth;
  drawTextBox("Page Up/Down(Up/Down)", position, f_pageUpDownButtonWidth,
              TextBoxStyle::Simple);

  auto windowSize{static_cast<sf::Vector2f>(m_window.getView().getSize())};
  auto maxNumberOfItems{static_cast<int>(
      windowSize.y / (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  auto maxScrollPos{static_cast<int>(m_loadFileMenuItems.size()) -
                    maxNumberOfItems};
  m_scrollPos = std::min(m_scrollPos, maxScrollPos);
  auto topItem{m_loadFileMenuItems.cbegin()};
  std::advance(topItem, m_scrollPos);
  auto maxItem{topItem};
  std::advance(maxItem, maxNumberOfItems);
  for (auto it = topItem; it != maxItem; it++) {
    auto width{windowSize.x - 2 * f_frameVerticalThickness};
    auto x{f_frameVerticalThickness};
    auto y{f_frameHorizontalThickness + f_textBoxOutlineThickness +
           static_cast<float>(std::distance(topItem, it)) * f_textBoxHeight};
    if (drawTextBox(*it, {x, y}, width, TextBoxStyle::Button)) {
      m_highlightedLoadFileMenuItem = *it;
    }
  }
}

void View::drawSaveFileMenu() {
  if (drawTextBox("Back(Esc)",
                  {f_textBoxOutlineThickness, f_textBoxOutlineThickness}, 200.,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Back;
  }
}

bool View::drawTextBox(const std::string &content, const sf::Vector2f &position,
                       float width, TextBoxStyle style) {
  auto highlighted{false};
  sf::RectangleShape rect{{width - 2 * f_textBoxOutlineThickness,
                           f_textBoxHeight - 2 * f_textBoxOutlineThickness}};
  rect.setPosition(position.x, position.y);
  rect.setOutlineThickness(f_textBoxOutlineThickness);
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
    case TextBoxStyle::Hidden:
      rect.setFillColor(f_hiddenTextBoxFillColor);
      rect.setOutlineColor(f_hiddenTextBoxOutlineColor);
      text.setFillColor(f_hiddenTextBoxTextColor);
      break;
    case TextBoxStyle::Simple:
    default:
      rect.setFillColor(f_simpleTextBoxFillColor);
      rect.setOutlineColor(f_simpleTextBoxOutlineColor);
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
      pixelToCellPosition(
          {static_cast<int>(windowSize.x * .5),
           static_cast<int>(f_frameHorizontalThickness +
                            (windowSize.y - f_frameHorizontalThickness -
                             f_frameHorizontalThickness) *
                                .5)})
          .value()};
  m_zoomLevel = std::min(f_maxZoomLevel, std::max(f_minZoomLevel, zoomLevel));
  auto cellSize{calculateCellSize()};
  setViewOffset({-(static_cast<float>(cellAtCentre.x) + .5f) * cellSize.x +
                     static_cast<float>(windowSize.x) * .5f,
                 -(static_cast<float>(cellAtCentre.y) + 0.5f) * cellSize.y +
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

sf::Vector2f View::calculateCellPosition(Cell cell) const {
  auto cellSize{calculateCellSize()};
  return {static_cast<float>(cell.x) * cellSize.x + f_frameHorizontalThickness +
              m_viewOffset.x,
          static_cast<float>(cell.y) * cellSize.y + f_frameHorizontalThickness +
              m_viewOffset.y};
}
