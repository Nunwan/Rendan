#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

#include "Logger.hpp"

Engine::Engine(int width, int height) : width(width), height(height) {}

void Engine::init() {
  Logger::Info("Engine initialisation");
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, "Rendan", nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
}

void Engine::cleanup() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Engine::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}
