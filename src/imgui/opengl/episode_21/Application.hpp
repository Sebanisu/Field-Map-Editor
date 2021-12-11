//
// Created by pcvii on 12/9/2021.
//

#ifndef MYPROJECT_APPLICATION_HPP
#define MYPROJECT_APPLICATION_HPP
#include "LayerStack.hpp"
#include "scope_guard.hpp"
#include "Window.hpp"
class Application
{
public:
  Application() = default;
  Application(std::string Title, int width, int height);
  void                       Run() const;
  static const Window * CurrentWindow();
  void                       SetCurrentWindow() const;

private:
  // window must be valid while other opengl objects exists.
  std::unique_ptr<Window> window         = nullptr;
  inline static Window   *current_window = nullptr;
  Layer::Stack            layers         = {};
};
#endif// MYPROJECT_APPLICATION_HPP
