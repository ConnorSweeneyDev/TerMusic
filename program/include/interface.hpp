#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
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
    void move_to(const Type &object);

  private:
    std::pair<std::vector<Type>, std::vector<std::string>> entries = {};
    int selected = 0;
    int current_entry = 0;

    ftxui::MenuOption option = ftxui::MenuOption::Vertical();
    ftxui::Component component = {};
  };

  class Interface
  {
    friend class Menu<Song>;

  public:
    Interface();

    ftxui::Loop create_loop();

  private:
    ftxui::Decorator pause_based_color();
    ftxui::Decorator search_based_color(bool reverse = false);

  public:
    Menu<Song> song_menu = {};
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

  private:
    bool searching = false;
    std::string search_term = "";

    ftxui::Screen::Cursor cursor = ftxui::Screen::Cursor(ftxui::Screen::Cursor::Shape::Hidden);
    ftxui::Component container = {};
    ftxui::Component renderer = {};
  };

  inline Interface interface;
}

#include "interface.inl"
