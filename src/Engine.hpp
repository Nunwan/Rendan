#pragma once

#include <GLFW/glfw3.h>

class Engine {
 private:
  int width, height;

  GLFWwindow *window;

 public:
  Engine(int width, int height);

  void init();
  void run();
  void cleanup();
};
