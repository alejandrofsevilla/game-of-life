#include <thread>

#include "Controller.hpp"
#include "Model.hpp"
#include "View.hpp"

namespace {
constexpr auto f_windowTitle{"GameOfLife"};
constexpr auto f_windowStyle{sf::Style::Fullscreen};
constexpr auto f_modelMaxWidth{500};
constexpr auto f_modelMaxHeight{250};
}  // namespace

int main() {
  auto desktopMode(sf::VideoMode::getDesktopMode());
  sf::RenderWindow window{desktopMode, f_windowTitle, f_windowStyle};
  Model model{f_modelMaxWidth, f_modelMaxHeight};
  std::thread modelThread{[&model]() { model.run(); }};
  View view{window, model};
  Controller controller{view, model};
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      controller.onEvent(event);
    }
    view.update();
  }
  modelThread.join();
  return 0;
}
