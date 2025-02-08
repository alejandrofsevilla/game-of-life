#include "Controller.hpp"

#include <cwctype>

#include "RleHelper.hpp"

namespace {
constexpr auto f_populationGenerationRate{.05};
} // namespace

Controller::Controller(View &view, Model &model)
    : m_view{view}, m_model{model}, m_mouseReferencePosition{},
      m_isSaveFileMenuReady{true} {}

void Controller::onEvent(const sf::Event &event) {
  switch (event.type) {
  default:
    return;
  case sf::Event::Closed:
    m_view.closeWindow();
    return;
  case sf::Event::MouseButtonPressed:
    onMouseButtonPressed(event.mouseButton);
    return;
  case sf::Event::MouseWheelScrolled:
    onMouseWheelScrolled(event.mouseWheelScroll);
    return;
  case sf::Event::MouseMoved:
    onMouseMoved(event.mouseMove);
    return;
  case sf::Event::TextEntered:
    onTextEnteredEvent(event.text);
    return;
  case sf::Event::KeyPressed:
    onKeyPressed(event.key);
    return;
  }
}

void Controller::onMouseButtonPressedInMainScreen(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto cell{m_view.highlightedCell()};
  if (cell) {
    onMouseButtonPressedOnCell({cell->col, cell->row});
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (highlightedButton == View::Button::None) {
    return;
  }
  switch (highlightedButton) {
  case View::Button::Quit:
    m_view.closeWindow();
    return;
  case View::Button::LoadFile:
    m_view.setScreen(View::Screen::LoadFile);
    return;
  case View::Button::SaveFile:
    m_view.setScreen(View::Screen::SaveFile);
    return;
  case View::Button::ZoomOut:
    m_view.zoomOut();
    return;
  case View::Button::ZoomIn:
    m_view.zoomIn();
    return;
  case View::Button::SpeedUp:
    m_model.speedUp();
    return;
  case View::Button::SlowDown:
    m_model.slowDown();
    return;
  case View::Button::Run:
    m_model.run();
    return;
  case View::Button::Pause:
    m_model.pause();
    return;
  case View::Button::GeneratePopulation:
    m_model.generatePopulation(f_populationGenerationRate);
    return;
  case View::Button::Reset:
    m_model.reset();
    return;
  case View::Button::Clear:
    m_model.clear();
    return;
  case View::Button::EditRule:
    m_view.setScreen(View::Screen::EditRule);
    return;
  default:
    return;
  }
}

void Controller::onMouseButtonPressedInLoadFileScreen(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto highlightedLoadFileMenuItem{m_view.highlightedLoadFileMenuItem()};
  if (highlightedLoadFileMenuItem) {
    m_model.clear();
    m_model.insertPattern(
        rle::loadPattern(highlightedLoadFileMenuItem.value()));
    m_view.setScreen(View::Screen::Main);
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (highlightedButton != View::Button::None) {
    switch (highlightedButton) {
    case View::Button::Back:
      m_view.setScreen(View::Screen::Main);
      return;
    default:
      return;
    }
  }
}

void Controller::onMouseButtonPressedInSaveFileScreen(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (highlightedButton == View::Button::None) {
    return;
  }
  switch (highlightedButton) {
  case View::Button::Back:
    m_view.setScreen(View::Screen::Main);
    return;
  case View::Button::Save:
    if (m_view.fileNameToSave().empty()) {
      return;
    }
    rle::savePattern(m_view.fileNameToSave(), m_model.initialPattern());
    m_view.setScreen(View::Screen::Main);
    return;
  default:
    return;
  }
}

void Controller::onMouseButtonPressedInEditRuleScreen(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (highlightedButton == View::Button::None) {
    return;
  }
  switch (highlightedButton) {
  case View::Button::Back:
    m_view.setScreen(View::Screen::Main);
    return;
  default:
    return;
  }
}

void Controller::onMouseButtonPressed(
    const sf::Event::MouseButtonEvent &event) {
  switch (m_view.screen()) {
  case View::Screen::Main:
    onMouseButtonPressedInMainScreen(event);
    return;
  case View::Screen::LoadFile:
    onMouseButtonPressedInLoadFileScreen(event);
    return;
  case View::Screen::SaveFile:
    onMouseButtonPressedInSaveFileScreen(event);
    return;
  case View::Screen::EditRule:
    onMouseButtonPressedInEditRuleScreen(event);
    return;
  default:
    return;
  }
}

void Controller::onMouseWheelScrolled(
    const sf::Event::MouseWheelScrollEvent &event) {
  switch (m_view.screen()) {
  case View::Screen::Main:
    onMainModeMouseWheelScrolled(event);
    return;
  case View::Screen::LoadFile:
    onMouseWheelScrolledInLoadFileScreen(event);
    return;
  default:
    return;
  }
}

void Controller::onMainModeMouseWheelScrolled(
    const sf::Event::MouseWheelScrollEvent &event) {
  if (event.delta > 0) {
    m_view.zoomIn();
  } else if (event.delta < 0) {
    m_view.zoomOut();
  }
}

void Controller::onMouseWheelScrolledInLoadFileScreen(
    const sf::Event::MouseWheelScrollEvent &event) {
  if (event.delta > 0) {
    m_view.scrollUp();
  } else if (event.delta < 0) {
    m_view.scrollDown();
  }
}

void Controller::onMouseMoved(const sf::Event::MouseMoveEvent &event) {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
    m_view.dragView(sf::Vector2i{event.x, event.y} - m_mouseReferencePosition);
  }
  m_mouseReferencePosition = sf::Vector2i{event.x, event.y};
}

void Controller::onTextEnteredEvent(const sf::Event::TextEvent &event) {
  switch (m_view.screen()) {
  case View::Screen::SaveFile: {
    if (!m_isSaveFileMenuReady) {
      m_isSaveFileMenuReady = true;
      return;
    }
    if (m_view.screen() != View::Screen::SaveFile) {
      return;
    }
    auto character{static_cast<char>(event.unicode)};
    if (!std::isalpha(character) && !std::isalnum(character)) {
      return;
    }
    auto name{m_view.fileNameToSave()};
    name.push_back(character);
    m_view.setFileNameToSave(name);
    return;
  }
  case View::Screen::EditRule: {
    auto character{static_cast<char>(event.unicode)};
    if (!std::isalnum(character)) {
      return;
    }
    auto value{static_cast<size_t>(std::stoi(&character))};
    if (m_view.highlightedEdit() == View::Edit::BirthRule) {
      auto rule{m_model.birthRule()};
      rule.insert(value);
      m_model.setBirthRule(rule);
    } else if (m_view.highlightedEdit() == View::Edit::SurvivalRule) {
      auto rule{m_model.survivalRule()};
      rule.insert(value);
      m_model.setSurvivalRule(rule);
    }
    return;
  }
  default:
    return;
  }
}

void Controller::onKeyPressed(const sf::Event::KeyEvent &event) {
  switch (m_view.screen()) {
  case View::Screen::LoadFile:
    onKeyPressedInLoadFileScreen(event);
    return;
  case View::Screen::SaveFile:
    onKeyPressedInSaveFileScreen(event);
    return;
  case View::Screen::EditRule:
    onKeyPressedInEditRuleScreen(event);
    break;
  case View::Screen::Main:
    onKeyPressedInMainScreen(event);
    break;
  default:
    return;
  }
}

void Controller::onKeyPressedInMainScreen(const sf::Event::KeyEvent &event) {
  switch (event.code) {
  case sf::Keyboard::R:
    if (m_model.status() != Model::Status::Running &&
        m_model.status() != Model::Status::Paused) {
      return;
    }
    m_model.reset();
    return;
  case sf::Keyboard::L:
    m_view.setScreen(View::Screen::LoadFile);
    return;
  case sf::Keyboard::S:
    if (m_model.initialPattern().empty()) {
      return;
    }
    m_isSaveFileMenuReady = false;
    m_view.setScreen(View::Screen::SaveFile);
    return;
  case sf::Keyboard::G:
    if (m_model.status() == Model::Status::ReadyToRun ||
        m_model.status() == Model::Status::Stopped) {
      m_model.generatePopulation(f_populationGenerationRate);
    }
    return;
  case sf::Keyboard::C:
    if (m_model.status() != Model::Status::Running) {
      m_model.clear();
    }
    return;
  case sf::Keyboard::Escape:
    m_view.closeWindow();
    return;
  case sf::Keyboard::Space:
    switch (m_model.status()) {
    case Model::Status::ReadyToRun:
    case Model::Status::Paused:
      m_model.run();
      return;
    case Model::Status::Running:
      m_model.pause();
      return;
    default:
      return;
    }
  default:
    return;
  }
}

void Controller::onKeyPressedInLoadFileScreen(
    const sf::Event::KeyEvent &event) {
  switch (event.code) {
  case sf::Keyboard::Escape:
    m_view.setScreen(View::Screen::Main);
    return;
  case sf::Keyboard::PageUp:
    m_view.pageUp();
    return;
  case sf::Keyboard::PageDown:
    m_view.pageDown();
    return;
  default:
    return;
  }
  return;
}

void Controller::onKeyPressedInSaveFileScreen(
    const sf::Event::KeyEvent &event) {
  switch (event.code) {
  case sf::Keyboard::Escape:
    m_view.setScreen(View::Screen::Main);
    return;
  case sf::Keyboard::Return:
    if (m_view.fileNameToSave().empty()) {
      return;
    }
    rle::savePattern(m_view.fileNameToSave(), m_model.initialPattern());
    m_view.setScreen(View::Screen::Main);
    return;
  case sf::Keyboard::Space: {
    auto name{m_view.fileNameToSave()};
    name.push_back(' ');
    m_view.setFileNameToSave(name);
    return;
  }
  case sf::Keyboard::BackSpace: {
    auto name{m_view.fileNameToSave()};
    if (name.empty()) {
      return;
    }
    name.pop_back();
    m_view.setFileNameToSave(name);
    return;
  }
  case sf::Keyboard::Hyphen: {
    auto name{m_view.fileNameToSave()};
    name.push_back('_');
    m_view.setFileNameToSave(name);
    return;
  }
  case sf::Keyboard::Home: {
    auto name{m_view.fileNameToSave()};
    name.push_back('+');
    m_view.setFileNameToSave(name);
    return;
  }
  case sf::Keyboard::Add: {
    auto name{m_view.fileNameToSave()};
    name.push_back('+');
    m_view.setFileNameToSave(name);
    return;
  }
  case sf::Keyboard::Subtract: {
    auto name{m_view.fileNameToSave()};
    name.push_back('-');
    m_view.setFileNameToSave(name);
    return;
  }
  default:
    return;
  }
}

void Controller::onKeyPressedInEditRuleScreen(
    const sf::Event::KeyEvent &event) {
  switch (event.code) {
  case sf::Keyboard::Escape:
    m_view.setScreen(View::Screen::Main);
    return;
  case sf::Keyboard::BackSpace: {
    if (m_view.highlightedEdit() == View::Edit::BirthRule) {
      auto rule{m_model.birthRule()};
      rule.clear();
      m_model.setBirthRule(rule);
    } else if (m_view.highlightedEdit() == View::Edit::SurvivalRule) {
      auto rule{m_model.survivalRule()};
      rule.clear();
      m_model.setSurvivalRule(rule);
    }
    return;
  }
  default:
    return;
  }
}

void Controller::onMouseButtonPressedOnCell(const Cell &cell) {
  if (m_model.status() != Model::Status::Stopped &&
      m_model.status() != Model::Status::ReadyToRun) {
    return;
  }
  if (cell.status == Cell::Status::Alive) {
    m_model.removeCell(cell);
  } else {
    m_model.insertCell(cell);
  }
  return;
}
