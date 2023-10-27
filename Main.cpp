#include <future>
#include <thread>

#include "Controller.hpp"
#include "Model.hpp"
#include "View.hpp"

namespace {
constexpr auto f_windowTitle{"Game Of Life"};
constexpr auto f_windowStyle{sf::Style::Fullscreen};
constexpr auto f_modelMaxWidth{960};
constexpr auto f_modelMaxHeight{490};
constexpr auto f_defaultModelUpdatePeriod{std::chrono::milliseconds{500}};
}  // namespace

int main() {
  auto desktopMode(sf::VideoMode::getDesktopMode());
  sf::RenderWindow window{desktopMode, f_windowTitle, f_windowStyle};
  window.setVerticalSyncEnabled(true);
  Model model{f_modelMaxWidth, f_modelMaxHeight};
  View view{window, model};
  Controller controller{view, model};
  std::future<void> scheduler;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      controller.onEvent(event);
    }
    if (model.status() == Model::Status::Running) {
      scheduler = std::async(&std::this_thread::sleep_for<int, std::milli>,
        f_defaultModelUpdatePeriod / model.speed());
      model.update();
      scheduler.wait();
    }
    view.update();
  }
  return 0;
}
