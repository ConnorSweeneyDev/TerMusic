#pragma once

#include <string>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/screen.hpp"

#include "song.hpp"

namespace tuim
{
  class Menu
  {
    friend class Interface;

  public:
    Menu();

    void populate(const std::vector<Song> &songs);

  public:
    std::vector<std::string> entries;
    int selected;

  private:
    ftxui::MenuOption option = ftxui::MenuOption::Vertical();
    ftxui::Component component = {};
  };

  class Interface
  {
  public:
    Interface();

    ftxui::Loop create_loop();

  public:
    Menu song_menu;

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

  private:
    ftxui::Screen::Cursor cursor = ftxui::Screen::Cursor(ftxui::Screen::Cursor::Shape::Hidden);
    ftxui::Component container = {};
    ftxui::Component renderer = {};
  };

  inline Interface interface;
}
