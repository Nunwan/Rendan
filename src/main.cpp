#include "Engine.hpp"
#include "Logger.hpp"
#include "spdlog/spdlog.h"
#include <spdlog/logger.h>


int main(int argc, char* argv[])
{
#ifndef NDEBUG
    Logger::Warn("Mode debug");
#endif

    //auto myLog = spdlog::default_logger();
    //myLog->set_level(spdlog::level::trace);
    //spdlog::set_default_logger(myLog);

    Logger::Info("Starting Rendan");
    Engine engine = Engine(800, 600);
    engine.init();
    engine.run();
    engine.cleanup();
    Logger::Info("Rendan stopped");
    return 0;
}
