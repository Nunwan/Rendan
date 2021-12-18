#include "Engine.hpp"
#include "Logger.hpp"
#include "spdlog/spdlog.h"

int main() {
  Logger::Info("Starting Rendan");
  Engine engine = Engine(800, 600);
  engine.init();
  engine.run();
  engine.cleanup();
  Logger::Info("Rendan stopped");
  return 0;
}
