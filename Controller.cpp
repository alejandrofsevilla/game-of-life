#include "Controller.hpp"

#include <cwctype>

#include "RleHelper.hpp"

namespace {
constexpr auto f_populationGenerationRate{.05};
}  // namespace

Controller::Controller(View &view, Model &model)
    : m_view{view},
      m_model{model},
      m_mouseReferencePosition{},
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

void Controller::onMainModeMouseButtonPressed(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto cell{m_view.pixelToCellPosition({event.x, event.y})};
  if (cell) {
    onMouseButtonPressedOnCell({cell->x, cell->y});
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (!highlightedButton) {
    return;
  }
  switch (highlightedButton.value()) {
    case View::Button::Quit:
      m_view.closeWindow();
      return;
    case View::Button::LoadFileMenu:
      m_view.setMode(View::Mode::LoadFile);
      return;
    case View::Button::SaveFileMenu:
      m_view.setMode(View::Mode::SaveFile);
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
    case View::Button::IncreaseSize:
      m_model.increaseSize();
      return;
    case View::Button::ReduceSize:
      m_model.reduceSize();
      return;
    default:
      return;
  }
}

void Controller::onLoadFileModeMouseButtonPressed(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto highlightedLoadFileMenuItem{m_view.highlightedLoadFileMenuItem()};
  if (highlightedLoadFileMenuItem) {
    m_model.clear();
    m_model.insertPattern(
        rle::loadPattern(highlightedLoadFileMenuItem.value()));
    m_view.setMode(View::Mode::Main);
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (highlightedButton) {
    switch (highlightedButton.value()) {
      case View::Button::Back:
        m_view.setMode(View::Mode::Main);
        return;
      default:
        return;
    }
  }
}

void Controller::onSaveFileModeMouseButtonPressed(
    const sf::Event::MouseButtonEvent &event) {
  if (event.button != sf::Mouse::Button::Left) {
    return;
  }
  auto highlightedButton{m_view.highlightedButton()};
  if (!highlightedButton) {
    return;
  }
  switch (highlightedButton.value()) {
    case View::Button::Back:
      m_view.setMode(View::Mode::Main);
      return;
    case View::Button::SaveFile:
      if (m_view.fileNameToSave().empty()) {
        return;
      }
      rle::savePattern(m_view.fileNameToSave(), m_model.initialPattern());
      m_view.setMode(View::Mode::Main);
      return;
    default:
      return;
  }
}

void Controller::onMouseButtonPressed(
    const sf::Event::MouseButtonEvent &event) {
  switch (m_view.mode()) {
    case View::Mode::Main:
      onMainModeMouseButtonPressed(event);
      return;
    case View::Mode::LoadFile:
      onLoadFileModeMouseButtonPressed(event);
      return;
    case View::Mode::SaveFile:
      onSaveFileModeMouseButtonPressed(event);
      return;
    default:
      return;
  }
}

void Controller::onMouseWheelScrolled(
    const sf::Event::MouseWheelScrollEvent &event) {
  switch (m_view.mode()) {
    case View::Mode::Main:
      onMainModeMouseWheelScrolled(event);
      return;
    case View::Mode::LoadFile:
      onLoadFileModeMouseWheelScrolled(event);
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

void Controller::onLoadFileModeMouseWheelScrolled(
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
  if (!m_isSaveFileMenuReady) {
    m_isSaveFileMenuReady = true;
    return;
  }
  if (m_view.mode() != View::Mode::SaveFile) {
    return;
  }
  if (!std::iswalpha(event.unicode) && !std::iswalnum(event.unicode)) {
    return;
  }
  std::wstring ws{static_cast<wchar_t>(event.unicode)};
  auto name{m_view.fileNameToSave()};
  name.append(ws.begin(), ws.end());
  m_view.setFileNameToSave(name);
}

void Controller::onKeyPressed(const sf::Event::KeyEvent &event) {
  switch (m_view.mode()) {
    case View::Mode::LoadFile:
      onLoadFileModeKeyPressed(event);
      return;
    case View::Mode::SaveFile:
      onSaveFileModeKeyPressed(event);
      return;
    case View::Mode::Main:
      onMainModeKeyPressed(event);
      break;
    default:
      return;
  }
}

void Controller::onMainModeKeyPressed(const sf::Event::KeyEvent &event) {
  switch (event.code) {
    case sf::Keyboard::Up:
      if (m_model.status() != Model::Status::Stopped) {
        return;
      }
      m_model.increaseSize();
      return;
    case sf::Keyboard::Down:
      if (m_model.status() != Model::Status::Stopped) {
        return;
      }
      m_model.reduceSize();
      return;
    case sf::Keyboard::Left:
      m_model.slowDown();
      return;
    case sf::Keyboard::Right:
      m_model.speedUp();
      return;
    case sf::Keyboard::R:
      if (m_model.status() != Model::Status::Running &&
          m_model.status() != Model::Status::Paused) {
        return;
      }
      m_model.reset();
      return;
    case sf::Keyboard::C:
      if (m_model.aliveCells().empty() && m_model.deadCells().empty()) {
        return;
      }
      m_model.clear();
      return;
    case sf::Keyboard::L:
      m_view.setMode(View::Mode::LoadFile);
      return;
    case sf::Keyboard::S:
      if (m_model.initialPattern().empty()) {
        return;
      }
      m_isSaveFileMenuReady = false;
      m_view.setMode(View::Mode::SaveFile);
      return;
    case sf::Keyboard::G:
      if (m_model.status() != Model::Status::Stopped &&
          m_model.status() != Model::Status::ReadyToRun) {
        return;
      }
      m_model.generatePopulation(f_populationGenerationRate);
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

void Controller::onLoadFileModeKeyPressed(const sf::Event::KeyEvent &event) {
  switch (event.code) {
    case sf::Keyboard::Escape:
      m_view.setMode(View::Mode::Main);
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

void Controller::onSaveFileModeKeyPressed(const sf::Event::KeyEvent &event) {
  switch (event.code) {
    case sf::Keyboard::Escape:
      m_view.setMode(View::Mode::Main);
      return;
    case sf::Keyboard::Return:
      if (m_view.fileNameToSave().empty()) {
        return;
      }
      rle::savePattern(m_view.fileNameToSave(), m_model.initialPattern());
      m_view.setMode(View::Mode::Main);
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

void Controller::onMouseButtonPressedOnCell(const Cell &cell) {
  if (m_model.status() != Model::Status::Stopped &&
      m_model.status() != Model::Status::ReadyToRun) {
    return;
  }
  auto cells{m_model.aliveCells()};
  auto match{cells.find(cell)};
  if (match != cells.end()) {
    m_model.removeCell(cell);
  } else {
    m_model.insertCell(cell);
  }
  return;
}
