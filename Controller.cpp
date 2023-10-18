#include "Controller.hpp"

namespace {
constexpr auto f_populationGenerationRate{.05};
}  // namespace

Controller::Controller(View &view, Model &model)
    : m_view{view}, m_model{model}, m_mouseReferencePosition{} {}

void Controller::onEvent(const sf::Event &event) {
  switch (event.type) {
    default:
      return;
    case sf::Event::Closed:
      m_model.stop();
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
    const sf::Event::MouseButtonEvent &event) {
  switch (event.button) {
    default:
      return;
    case sf::Mouse::Button::Left: {
      if (m_model.status() != Model::Status::Paused) {
        return;
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
      return;
  }
}

void Controller::onMouseWheelScrolled(
    const sf::Event::MouseWheelScrollEvent &event) {
  if (event.delta > 0) {
    m_view.zoomIn();
  } else if (event.delta < 0) {
    m_view.zoomOut();
  }
}

void Controller::onMouseMoved(const sf::Event::MouseMoveEvent &event) {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
    m_view.dragView(sf::Vector2i{event.x, event.y} - m_mouseReferencePosition);
  }
  m_mouseReferencePosition = sf::Vector2i{event.x, event.y};
}

void Controller::onKeyPressed(const sf::Event::KeyEvent &event) {
  switch (event.code) {
    default:
      return;
    case sf::Keyboard::Left:
      m_model.decreaseSpeed();
      return;
    case sf::Keyboard::Right:
      m_model.increaseSpeed();
      return;
    case sf::Keyboard::R:
      m_model.reset();
      return;
    case sf::Keyboard::G:
      if (m_model.status() != Model::Status::Paused) {
        return;
      }
      m_model.generateLivingCells(static_cast<std::size_t>(
          static_cast<float>(m_model.width()) *
          static_cast<float>(m_model.height()) * f_populationGenerationRate));
      return;
    case sf::Keyboard::Escape:
      m_model.stop();
      m_view.closeWindow();
      return;
    case sf::Keyboard::Space:
      if (m_model.status() == Model::Status::Paused) {
        m_model.run();
      } else {
        m_model.pause();
      }
      return;
  }
}
