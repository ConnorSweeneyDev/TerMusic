#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/screen.hpp"

#include "song.hpp"

namespace tuim
{
  template <typename Type> class Menu
  {
    friend class Interface;

  public:
    Menu();

    void populate(const std::vector<Type> &objects);

  private:
    std::pair<std::vector<Type>, std::vector<std::string>> entries = {};
    int selected = 0;

    ftxui::MenuOption option = ftxui::MenuOption::Vertical();
    ftxui::Component component = {};
  };

  class Interface
  {
  public:
    Interface();

    ftxui::Loop create_loop();

  public:
    Menu<Song> song_menu;

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

  private:
    ftxui::Screen::Cursor cursor = ftxui::Screen::Cursor(ftxui::Screen::Cursor::Shape::Hidden);
    ftxui::Component container = {};
    ftxui::Component renderer = {};
  };

  inline Interface interface;
}

#include "interface.inl"
