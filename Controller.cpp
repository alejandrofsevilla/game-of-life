#include "Controller.hpp"

namespace {
constexpr auto f_populationGenerationRate{.05};
}  // namespace

Controller::Controller(View& view, Model& model)
    : m_view{view}, m_model{model}, m_mouseReferencePosition{} {}

void Controller::onEvent(const sf::Event& event) {
  switch (event.type) {
    default:
      return;
    case sf::Event::Closed:
      m_model.finish();
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
    case sf::Event::KeyPressed:
      onKeyPressed(event.key);
      return;
  }
}

void Controller::onMouseButtonPressed(
    const sf::Event::MouseButtonEvent& event) {
  switch (event.button) {
    default:
      return;
    case sf::Mouse::Button::Left: {
      auto highlightedButton{m_view.highlightedButton()};
      if (highlightedButton) {
        switch (highlightedButton.value()) {
          case View::Button::Quit:
            m_model.finish();
            m_view.closeWindow();
            return;
          case View::Button::LoadFile:
            return;
          case View::Button::SaveFile:
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
      auto cell{m_view.pixelToCell({event.x, event.y})};
      if (!cell) {
        return;
      }
      if (m_model.livingCells().count(cell.value()) == 0) {
        m_model.addLivingCell(cell.value());
      } else {
        m_model.removeLivingCell(cell.value());
      }
    }
  }
}

void Controller::onMouseWheelScrolled(
    const sf::Event::MouseWheelScrollEvent& event) {
  if (event.delta > 0) {
    m_view.zoomIn();
  } else if (event.delta < 0) {
    m_view.zoomOut();
  }
}

void Controller::onMouseMoved(const sf::Event::MouseMoveEvent& event) {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
    m_view.dragView(sf::Vector2i{event.x, event.y} - m_mouseReferencePosition);
  }
  m_mouseReferencePosition = sf::Vector2i{event.x, event.y};
}

void Controller::onKeyPressed(const sf::Event::KeyEvent& event) {
  switch (event.code) {
    default:
      return;
    case sf::Keyboard::Up:
      m_model.increaseSize();
      return;
    case sf::Keyboard::Down:
      m_model.reduceSize();
      return;
    case sf::Keyboard::Left:
      m_model.slowDown();
      return;
    case sf::Keyboard::Right:
      m_model.speedUp();
      return;
    case sf::Keyboard::R:
      m_model.reset();
      return;
    case sf::Keyboard::C:
      m_model.clear();
      return;
    case sf::Keyboard::G:
      m_model.generatePopulation(f_populationGenerationRate);
      return;
    case sf::Keyboard::Escape:
      m_model.finish();
      m_view.closeWindow();
      return;
    case sf::Keyboard::Space:
      if (m_model.livingCells().empty()) {
        return;
      }
      switch (m_model.status()) {
        case Model::Status::Stopped:
        case Model::Status::Paused:
          m_model.run();
          return;
        case Model::Status::Running:
          m_model.pause();
        default:
          return;
      }
      return;
  }
}
