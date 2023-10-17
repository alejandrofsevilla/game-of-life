#include <thread>

#include "Controller.hpp"
#include "Model.hpp"
#include "View.hpp"

namespace {
constexpr auto f_windowTitle{"GameOfLife"};
constexpr auto f_windowStyle{sf::Style::Fullscreen};
constexpr std::pair<std::size_t, std::size_t> f_modelSize{200, 100};
}  // namespace

int main() {
  sf::RenderWindow window{sf::VideoMode::getDesktopMode(), f_windowTitle,
                          f_windowStyle};
  Model model{f_modelSize.first, f_modelSize.second};
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
