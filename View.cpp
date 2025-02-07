#include "View.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <future>
#include <sstream>
#include <thread>

#include "RleHelper.hpp"

namespace {
const auto f_frameColor{sf::Color{35, 35, 35}};
const auto f_livingCellColor{sf::Color::White};
const auto f_gridColor{sf::Color::Black};
const auto f_deadCellColor{sf::Color{60, 60, 60}};
const auto f_backgroundColor{sf::Color{35, 35, 35}};
const auto f_simpleTextBoxFillColor{sf::Color{35, 35, 35}};
const auto f_simpleTextBoxOutlineColor{sf::Color{35, 35, 35}};
const auto f_simpleTextBoxTextColor{sf::Color::White};
const auto f_clickedButtonFillColor{sf::Color{15, 15, 15}};
const auto f_clickedButtonOutlineColor{sf::Color{35, 35, 35}};
const auto f_clickedButtonTextColor{sf::Color::White};
const auto f_highlightedButtonFilledColor{sf::Color::White};
const auto f_highlightedButtonOutlineColor{sf::Color::White};
const auto f_highlightedButtonTextColor{sf::Color{35, 35, 35}};
const auto f_buttonFillColor{sf::Color{60, 60, 60}};
const auto f_buttonOutlineColor{sf::Color{35, 35, 35}};
const auto f_buttonTextColor{sf::Color::White};
const auto f_displayTextBoxFillColor{sf::Color::Black};
const auto f_displayTextBoxOutlineColor{sf::Color{35, 35, 35}};
const auto f_displayTextBoxTextFillColor{sf::Color::White};
const auto f_hiddenTextBoxFillColor{sf::Color{35, 35, 35}};
const auto f_hiddenTextBoxOutlineColor{sf::Color{35, 35, 35}};
const auto f_hiddenTextBoxTextColor{sf::Color{100, 100, 100}};
const auto f_hiddenButtonFillColor{sf::Color{45, 45, 45}};
const auto f_hiddenButtonOutlineColor{sf::Color{35, 35, 35}};
const auto f_hiddenButtonTextColor{sf::Color{110, 110, 110}};
constexpr auto f_fontPath{"../resources/futura.ttf"};
constexpr auto f_frameHorizontalThickness{50.f};
constexpr auto f_frameVerticalThickness{0.f};
constexpr auto f_fontSize{18};
constexpr auto f_textBoxOutlineThickness{1.f};
constexpr auto f_textBoxHeight{f_frameHorizontalThickness};
constexpr auto f_defaultZoomLevel{5};
constexpr auto f_minZoomLevel{1};
constexpr auto f_maxZoomLevel{10};
constexpr auto f_zoomSensibility{1};
constexpr auto f_textBoxTextVerticalPosition{12.f};
constexpr auto f_defaultButtonWidth{1920.f / 13};
constexpr auto f_addRemoveCellTextWidth{290.f};
constexpr auto f_dragViewTextWidth{240.f};
constexpr auto f_zoomTextWidth{235.f};
constexpr auto f_displayBoxWidth{131.f};
constexpr auto f_plusMinusButtonWidth{f_defaultButtonWidth * .5f};
constexpr auto f_defaultTextWidth{195.f};
constexpr auto f_saveMenuInfoTextWidth{180.f};
constexpr auto f_pageUpDownTextWidth{370.f};
constexpr auto f_scrollUpDownTextWidth{330.f};
constexpr auto f_ruleEditBoxWidth{220.f};
constexpr auto f_editRuleMenuInfoTextWidth{120.f};
constexpr auto f_defaultScreenWidth{1920};
constexpr auto f_defaultScreenHeight{1080};

inline std::string toString(const std::set<std::size_t> &values) {
  std::stringstream s;
  for (const auto &val : values) {
    s << val;
  }
  return s.str();
}

inline sf::Color toCellColor(Cell::Status status) {
  switch (status) {
  case Cell::Status::Alive:
    return f_livingCellColor;
  case Cell::Status::Dead:
    return f_deadCellColor;
  default:
    return f_backgroundColor;
  }
}
} // namespace

View::View(sf::RenderWindow &window, Model &model)
    : m_model{model}, m_screen{Screen::Main}, m_window{window},
      m_topLeftCellPos{f_frameVerticalThickness, f_frameHorizontalThickness},
      m_cellsVertexArray{sf::Quads, 4 * model.width() * model.height()},
      m_font{}, m_highlightedButton{Button::None},
      m_highlightedEdit{Edit::None}, m_highlightedLoadFileMenuItem{},
      m_zoomLevel{f_defaultZoomLevel}, m_scrollPos{}, m_fileNameToSave{} {
  m_font.loadFromFile(f_fontPath);
}

void View::setFileNameToSave(const std::string &name) {
  m_fileNameToSave = name;
}

View::Screen View::screen() const { return m_screen; }

std::optional<std::string> View::highlightedLoadFileMenuItem() const {
  return m_highlightedLoadFileMenuItem;
}

View::Button View::highlightedButton() const { return m_highlightedButton; }

View::Edit View::highlightedEdit() const { return m_highlightedEdit; }

std::optional<Cell> View::highlightedCell() const {
  return cellAtCoord(
      m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
}

const std::string &View::fileNameToSave() const { return m_fileNameToSave; }

void View::update() {
  m_window.clear();
  m_highlightedButton = Button::None;
  drawBackground();
  switch (m_screen) {
  case Screen::EditRule:
    drawEditRuleScreen();
    break;
  case Screen::SaveFile:
    drawSaveFileScreen();
    break;
  case Screen::LoadFile:
    drawLoadFileScreen();
    break;
  case Screen::Main:
  default:
    drawMainScreen();
    break;
  }
  updateWindowView();
  m_window.display();
}

void View::zoomIn() { applyZoomLevel(m_zoomLevel + f_zoomSensibility); }

void View::zoomOut() { applyZoomLevel(m_zoomLevel - f_zoomSensibility); }

void View::scrollDown() { m_scrollPos++; }

void View::scrollUp() { m_scrollPos = std::max(0, m_scrollPos - 1); }

void View::pageDown() {
  auto maxNumberOfItems{
      static_cast<int>(f_defaultScreenHeight /
                       (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  m_scrollPos = std::max(0, m_scrollPos + maxNumberOfItems);
}

void View::pageUp() {
  auto maxNumberOfItems{
      static_cast<int>(f_defaultScreenHeight /
                       (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  m_scrollPos = std::max(0, m_scrollPos - maxNumberOfItems);
}

void View::closeWindow() { m_window.close(); }

void View::dragView(sf::Vector2i offset) {
  applyViewOffset({m_topLeftCellPos.x + static_cast<float>(offset.x),
                   m_topLeftCellPos.y + static_cast<float>(offset.y)});
}

void View::setScreen(View::Screen screen) { m_screen = screen; }

void View::drawMainScreen() {
  m_fileNameToSave.clear();
  m_scrollPos = 0;
  drawCells();
  drawGrid();
  drawFrame();
  drawTopMenu();
}

void View::drawLoadFileScreen() {
  m_highlightedLoadFileMenuItem.reset();
  sf::Vector2f position{f_frameVerticalThickness + f_textBoxOutlineThickness,
                        f_textBoxOutlineThickness};
  if (drawTextBox("Back [Esc]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Back;
  }
  position.x += f_defaultButtonWidth;
  drawTextBox("Scroll Up/Down [Mouse Wheel]", position, f_scrollUpDownTextWidth,
              TextBoxStyle::Text);
  position.x += f_scrollUpDownTextWidth;
  drawTextBox("Page Up/Down [PageUp/PageDown]", position, f_pageUpDownTextWidth,
              TextBoxStyle::Text);
  auto maxNumberOfItems{
      static_cast<int>(f_defaultScreenHeight /
                       (f_textBoxHeight + f_textBoxOutlineThickness * 2.))};
  auto items{rle::listPatternNames()};
  if (items.empty()) {
    auto screenMiddleHeight{f_defaultScreenHeight * .5f};
    position.x = f_frameVerticalThickness + f_textBoxOutlineThickness;
    position.y = screenMiddleHeight;
    drawTextBox("No files found in patterns directory.", position,
                f_defaultScreenWidth - 2 * f_frameVerticalThickness,
                TextBoxStyle::Text);
    return;
  }
  auto itemsSize{static_cast<int>(items.size())};
  auto maxScrollPos{std::min(itemsSize, maxNumberOfItems)};
  m_scrollPos = std::min(m_scrollPos, maxScrollPos - 1);
  auto topItem{std::next(items.cbegin(), m_scrollPos)};
  auto maxItem{std::next(items.cbegin(), maxScrollPos)};
  for (auto it = topItem; it != maxItem; it++) {
    auto width{f_defaultScreenWidth - 2 * f_frameVerticalThickness};
    auto x{f_frameVerticalThickness};
    auto y{f_frameHorizontalThickness + f_textBoxOutlineThickness +
           static_cast<float>(std::distance(topItem, it)) * f_textBoxHeight};
    if (drawTextBox(*it, {x, y}, width, TextBoxStyle::Button)) {
      m_highlightedLoadFileMenuItem = *it;
    }
  }
}

void View::drawSaveFileScreen() {
  sf::Vector2f position{f_frameVerticalThickness + f_textBoxOutlineThickness,
                        f_textBoxOutlineThickness};
  if (drawTextBox("Back [Esc]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Back;
  }
  position.x += f_defaultButtonWidth;
  if (drawTextBox("Save [Enter]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Save;
  }
  auto screenMiddleHeight{f_defaultScreenHeight * .5f};
  position.x = f_frameVerticalThickness + f_textBoxOutlineThickness;
  position.y = screenMiddleHeight;
  drawTextBox(m_fileNameToSave, position,
              f_defaultScreenWidth - 2 * f_frameVerticalThickness,
              TextBoxStyle::Display);
  position.y -= f_textBoxHeight;
  drawTextBox("Enter pattern name...", position, f_saveMenuInfoTextWidth,
              TextBoxStyle::Text);
}

void View::drawEditRuleScreen() {
  sf::Vector2f position{f_frameVerticalThickness + f_textBoxOutlineThickness,
                        f_textBoxOutlineThickness};
  if (drawTextBox("Back [Esc]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Back;
  }
  auto screenMiddleHeight{f_defaultScreenHeight * .5f};
  position.x = f_frameVerticalThickness + f_textBoxOutlineThickness;
  position.y = screenMiddleHeight;
  auto buttonWidth{f_defaultScreenWidth * .5f - f_textBoxOutlineThickness};
  std::string rule{"B"};
  rule.append(toString(m_model.birthRule()));
  auto style{m_highlightedEdit == Edit::BirthRule ? TextBoxStyle::Display
                                                  : TextBoxStyle::Button};
  auto isBirthRuleEditHighlighted{
      drawTextBox(rule, position, buttonWidth, style)};
  if (isBirthRuleEditHighlighted) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      m_highlightedEdit = Edit::BirthRule;
    }
  }
  position.x += buttonWidth + 2 * f_textBoxOutlineThickness;
  rule = "S";
  rule.append(toString(m_model.survivalRule()));
  style = m_highlightedEdit == Edit::SurvivalRule ? TextBoxStyle::Display
                                                  : TextBoxStyle::Button;
  auto isSurvivalRuleEditHighlighted{
      drawTextBox(rule, position, buttonWidth, style)};
  if (isSurvivalRuleEditHighlighted) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      m_highlightedEdit = Edit::SurvivalRule;
    }
  }
  position.x = f_textBoxOutlineThickness;
  position.y -= f_textBoxHeight;
  drawTextBox("Click to edit...", position, f_editRuleMenuInfoTextWidth,
              TextBoxStyle::Text);
}

void View::drawFrame() {
  auto viewSize{m_window.getView().getSize()};
  auto thickness{std::max((viewSize.x - f_defaultScreenWidth) * .5f,
                          (viewSize.y - f_defaultScreenHeight) * .5f)};
  sf::RectangleShape rect{{f_defaultScreenWidth, f_defaultScreenHeight}};
  rect.setOutlineThickness(thickness);
  rect.setOutlineColor(sf::Color::Black);
  rect.setFillColor(sf::Color::Transparent);
  m_window.draw(rect);
}

void View::drawBackground() {
  sf::RectangleShape background{{f_defaultScreenWidth, f_defaultScreenHeight}};
  background.setFillColor(f_backgroundColor);
  m_window.draw(background);
}

void View::drawGrid() {
  auto cellSize{calculateCellSize()};
  sf::VertexArray lines{sf::Lines, 2 * (m_model.width() + m_model.height())};
  std::size_t index{0};
  for (std::size_t x = 0; x < m_model.width(); x++) {
    auto pos{static_cast<float>(x) * cellSize.x + m_topLeftCellPos.x};
    lines[index].position = sf::Vector2f(pos, 0);
    lines[index++].color = f_gridColor;
    lines[index].position = sf::Vector2f(pos, f_defaultScreenHeight);
    lines[index++].color = f_gridColor;
  }
  for (std::size_t y = 0; y < m_model.height(); y++) {
    auto pos{static_cast<float>(y) * cellSize.y + m_topLeftCellPos.y};
    lines[index].position = sf::Vector2f(0, pos);
    lines[index++].color = f_gridColor;
    lines[index].position = sf::Vector2f(f_defaultScreenWidth, pos);
    lines[index++].color = f_gridColor;
  }
  m_window.draw(lines);
}

void View::updateCellVertexArray(std::size_t minCol, std::size_t maxCol) {
  auto size{calculateCellSize()};
  sf::Color cellColor;
  sf::Vector2f cellPosition;
  std::size_t id{4 * minCol * m_model.height()};
  for (std::size_t col = minCol; col < maxCol; col++) {
    for (std::size_t row = 0; row < m_model.height(); row++) {
      cellColor = toCellColor(m_model.cellStatus(col, row));
      cellPosition = calculateCellPosition(col, row);
      m_cellsVertexArray[id].position = cellPosition;
      m_cellsVertexArray[id++].color = cellColor;
      m_cellsVertexArray[id].position = cellPosition + sf::Vector2f{size.x, 0};
      m_cellsVertexArray[id++].color = cellColor;
      m_cellsVertexArray[id].position =
          cellPosition + sf::Vector2f{size.x, size.y};
      m_cellsVertexArray[id++].color = cellColor;
      m_cellsVertexArray[id].position = cellPosition + sf::Vector2f{0, size.y};
      m_cellsVertexArray[id++].color = cellColor;
    }
  }
}

void View::drawCells() {
  auto size{4 * m_model.width() * m_model.height()};
  if (m_cellsVertexArray.getVertexCount() != size) {
    m_cellsVertexArray.resize(size);
  }
  auto numOfThreads{std::thread::hardware_concurrency()};
  auto numColPerThread{static_cast<std::size_t>(
      std::ceil(static_cast<double>(m_model.width()) / numOfThreads))};
  {
    std::vector<std::future<void>> tasks;
    for (std::size_t i = 0; i < numOfThreads; i++) {
      auto begin{std::min(i * numColPerThread, m_model.width())};
      auto end{std::min((i + 1) * numColPerThread, m_model.width())};
      tasks.emplace_back(
          std::async(&View::updateCellVertexArray, this, begin, end));
    }
  }
  m_window.draw(m_cellsVertexArray);
}

void View::drawTopMenu() {
  sf::RectangleShape rect{{f_defaultScreenWidth, f_frameHorizontalThickness}};
  rect.setFillColor(f_frameColor);
  m_window.draw(rect);
  sf::Vector2f position{f_frameVerticalThickness, f_textBoxOutlineThickness};
  if (drawTextBox("Quit [Esc]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::Quit;
  }
  position.x += f_defaultButtonWidth;
  if (drawTextBox("Load File [L]", position, f_defaultButtonWidth,
                  TextBoxStyle::Button)) {
    m_highlightedButton = Button::LoadFile;
  }
  position.x += f_defaultButtonWidth;
  auto style{m_model.initialPattern().empty() ? TextBoxStyle::HiddenButton
                                              : TextBoxStyle::Button};
  if (drawTextBox("Save File [S]", position, f_defaultButtonWidth, style)) {
    m_highlightedButton = Button::SaveFile;
  }
  position.x += f_defaultButtonWidth;
  style = m_model.status() == Model::Status::Stopped
              ? TextBoxStyle::HiddenButton
              : TextBoxStyle::Button;
  switch (m_model.status()) {
  case Model::Status::Stopped:
  case Model::Status::ReadyToRun:
    if (drawTextBox("Start [_]", position, f_defaultButtonWidth, style)) {
      m_highlightedButton = Button::Run;
    }
    break;
  case Model::Status::Running:
    if (drawTextBox("Pause [_]", position, f_defaultButtonWidth, style)) {
      m_highlightedButton = Button::Pause;
    }
    break;
  case Model::Status::Paused:
    if (drawTextBox("Continue [_]", position, f_defaultButtonWidth, style)) {
      m_highlightedButton = Button::Run;
    }
    break;
  default:
    break;
  }
  position.x += f_defaultButtonWidth;
  style = (m_model.status() == Model::Status::ReadyToRun ||
           m_model.status() == Model::Status::Stopped)
              ? TextBoxStyle::HiddenButton
              : TextBoxStyle::Button;
  if (drawTextBox("Reset [R]", position, f_defaultButtonWidth, style)) {
    m_highlightedButton = Button::Reset;
  }
  position.x += f_defaultButtonWidth;
  style =
      (m_model.status() == Model::Status::ReadyToRun ||
       m_model.status() == Model::Status::Stopped)
          ? TextBoxStyle::HiddenButton
          : (m_model.speed() == m_model.minSpeed() ? TextBoxStyle::HiddenButton
                                                   : TextBoxStyle::Button);
  if (drawTextBox(">", position, f_plusMinusButtonWidth, style)) {
    m_highlightedButton = Button::SlowDown;
  }
  style =
      (m_model.status() == Model::Status::ReadyToRun ||
       m_model.status() == Model::Status::Stopped)
          ? TextBoxStyle::HiddenButton
          : (m_model.speed() == m_model.maxSpeed() ? TextBoxStyle::HiddenButton
                                                   : TextBoxStyle::Button);
  position.x += f_plusMinusButtonWidth;
  if (drawTextBox(">>", position, f_plusMinusButtonWidth, style)) {
    m_highlightedButton = Button::SpeedUp;
  }
  position.x += f_plusMinusButtonWidth;
  style = (m_model.status() != Model::Status::Running)
              ? TextBoxStyle::Button
              : TextBoxStyle::HiddenButton;
  if (drawTextBox("Clear", position, f_defaultButtonWidth, style)) {
    m_highlightedButton = Button::Clear;
  }
  position.x += f_defaultButtonWidth;
  style = (m_model.status() == Model::Status::ReadyToRun ||
           m_model.status() == Model::Status::Stopped)
              ? TextBoxStyle::Button
              : TextBoxStyle::HiddenButton;
  if (drawTextBox("Random", position, f_defaultButtonWidth, style)) {
    m_highlightedButton = Button::GeneratePopulation;
  }
  style = (m_model.status() == Model::Status::ReadyToRun ||
           m_model.status() == Model::Status::Stopped)
              ? TextBoxStyle::Text
              : TextBoxStyle::HiddenText;
  position.x += f_defaultButtonWidth;
  std::string rule("RLE B");
  rule.append(toString(m_model.birthRule()));
  rule.append("/S");
  rule.append(toString(m_model.survivalRule()));
  style = (m_model.status() != Model::Status::Stopped &&
           m_model.status() != Model::Status::ReadyToRun)
              ? TextBoxStyle::Display
              : TextBoxStyle::Button;
  if (drawTextBox(rule, position, f_defaultButtonWidth, style)) {
    m_highlightedButton = Button::EditRule;
  }
  position.x += f_defaultButtonWidth;
  drawTextBox("Generation", position, f_defaultButtonWidth, TextBoxStyle::Text);
  position.x += f_defaultButtonWidth;
  drawTextBox(std::to_string(m_model.generation()), position,
              f_defaultButtonWidth, TextBoxStyle::Display);
  position.x += f_defaultButtonWidth;
  drawTextBox("Population", position, f_defaultButtonWidth, TextBoxStyle::Text);
  position.x += f_defaultButtonWidth;
  drawTextBox(std::to_string(m_model.population()), position,
              f_defaultButtonWidth, TextBoxStyle::Display);
  position.x += f_defaultButtonWidth;
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
  if (text.getLocalBounds().width > rect.getSize().x) {
    text.setString("RLE...");
  }
  text.setPosition(position.x + (width - text.getLocalBounds().width) * .5f,
                   position.y + f_textBoxTextVerticalPosition);
  switch (style) {
  case TextBoxStyle::Button:
    if (rect.getGlobalBounds().contains(
            m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)))) {
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
      rect.setFillColor(f_buttonFillColor);
      rect.setOutlineColor(f_buttonOutlineColor);
      text.setFillColor(f_buttonTextColor);
    }
    break;
  case TextBoxStyle::Display:
    rect.setFillColor(f_displayTextBoxFillColor);
    rect.setOutlineColor(f_displayTextBoxOutlineColor);
    text.setFillColor(f_displayTextBoxTextFillColor);
    break;
  case TextBoxStyle::HiddenText:
    rect.setFillColor(f_hiddenTextBoxFillColor);
    rect.setOutlineColor(f_hiddenTextBoxOutlineColor);
    text.setFillColor(f_hiddenTextBoxTextColor);
    break;
  case TextBoxStyle::HiddenButton:
    rect.setFillColor(f_hiddenButtonFillColor);
    rect.setOutlineColor(f_hiddenButtonOutlineColor);
    text.setFillColor(f_hiddenButtonTextColor);
    break;
  case TextBoxStyle::Text:
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

void View::applyViewOffset(const sf::Vector2f &position) {
  auto cellSize{calculateCellSize()};
  sf::Vector2f minOffset{static_cast<float>(f_defaultScreenWidth) -
                             cellSize.x * static_cast<float>(m_model.width()) -
                             f_frameVerticalThickness,
                         static_cast<float>(f_defaultScreenHeight) -
                             cellSize.y *
                                 (static_cast<float>(m_model.height()) -
                                  f_frameHorizontalThickness)};
  m_topLeftCellPos.x = std::min(static_cast<float>(f_frameVerticalThickness),
                                std::max(position.x, minOffset.x));
  m_topLeftCellPos.y =
      std::min(f_frameHorizontalThickness, std::max(position.y, minOffset.y));
}

void View::applyZoomLevel(int zoomLevel) {
  auto cellAtCentre{cellAtCoord(
      {f_defaultScreenWidth * .5f,
       f_frameHorizontalThickness +
           (f_defaultScreenHeight - f_frameHorizontalThickness) * .5f})};
  m_zoomLevel = std::min(f_maxZoomLevel, std::max(f_minZoomLevel, zoomLevel));
  auto cellSize{calculateCellSize()};
  applyViewOffset(
      {-(static_cast<float>(cellAtCentre.value().col) + .5f) * cellSize.x +
           static_cast<float>(f_defaultScreenWidth) * .5f,
       -(static_cast<float>(cellAtCentre.value().row) + 0.5f) * cellSize.y +
           static_cast<float>(f_defaultScreenHeight)});
}

void View::updateWindowView() {
  auto view{m_window.getView()};
  view.setSize(f_defaultScreenWidth, f_defaultScreenHeight);
  view.setCenter(f_defaultScreenWidth / 2.f, f_defaultScreenHeight / 2.f);
  m_window.setView(view);
}

sf::Vector2f View::calculateCellSize() const {
  return {static_cast<float>(m_zoomLevel) *
              (static_cast<float>(f_defaultScreenWidth) -
               2 * f_frameVerticalThickness) /
              static_cast<float>(m_model.width()),
          static_cast<float>(m_zoomLevel) *
              (static_cast<float>(f_defaultScreenHeight) -
               f_frameHorizontalThickness) /
              static_cast<float>(m_model.height())};
}

sf::Vector2f View::calculateCellPosition(std::size_t row,
                                         std::size_t column) const {
  auto size{calculateCellSize()};
  return {static_cast<float>(row) * size.x + m_topLeftCellPos.x,
          static_cast<float>(column) * size.y + m_topLeftCellPos.y};
}

std::optional<Cell> View::cellAtCoord(sf::Vector2f coord) const {
  if (coord.x < f_frameVerticalThickness ||
      coord.x > (f_defaultScreenWidth - f_frameVerticalThickness) ||
      coord.y < f_frameHorizontalThickness ||
      coord.y > (f_defaultScreenHeight)) {
    return {};
  }
  auto cellSize{calculateCellSize()};
  return {
      {static_cast<std::size_t>((coord.x - m_topLeftCellPos.x) / cellSize.x),
       static_cast<std::size_t>((coord.y - m_topLeftCellPos.y) / cellSize.y)}};
}
