#include "interface.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <variant>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/ref.hpp"

#include "player.hpp"
#include "song.hpp"

namespace tuim
{
  Menu::Menu()
  {
    option.focused_entry = ftxui::Ref<int>(&selected);
    component = ftxui::Menu(&entries.second, &selected, option);
    component |= ftxui::CatchEvent(
      [&](ftxui::Event event)
      {
        if (event == ftxui::Event::j)
        {
          selected++;
          return true;
        }
        if (event == ftxui::Event::k)
        {
          selected--;
          return true;
        }
        if (event == ftxui::Event::CtrlD)
        {
          selected += 12;
          return true;
        }
        if (event == ftxui::Event::CtrlU)
        {
          selected -= 12;
          return true;
        }
        if (event == ftxui::Event::g)
        {
          selected = 0;
          return true;
        }
        if (event == ftxui::Event::G)
        {
          selected = static_cast<int>(entries.second.size()) - 1;
          return true;
        }
        if (event == ftxui::Event::p)
        {
          player.toggle_pause();
          return true;
        }
        if (event == ftxui::Event::u)
        {
          player.change_volume(1);
          return true;
        }
        if (event == ftxui::Event::d)
        {
          player.change_volume(-1);
          return true;
        }
        if (event == ftxui::Event::U)
        {
          player.change_volume(5);
          return true;
        }
        if (event == ftxui::Event::D)
        {
          player.change_volume(-5);
          return true;
        }
        if (event == ftxui::Event::Return)
        {
          if (std::holds_alternative<std::vector<Song>>(entries.first))
          {
            player.play(std::get<std::vector<Song>>(entries.first)[static_cast<size_t>(selected)]);
            return true;
          }
          else
          {
            std::cerr << "Unsupported type for entry_objects." << std::endl;
            exit(EXIT_FAILURE);
          }
        }
        if (event == ftxui::Event::n)
        {
          player.unload();
          return true;
        }
        if (event == ftxui::Event::Escape)
        {
          interface.screen.ExitLoopClosure()();
          return true;
        }
        if (event.mouse().motion == ftxui::Mouse::Motion::Moved ||
            event.mouse().motion == ftxui::Mouse::Motion::Pressed)
          return true;
        return false;
      });
  }

  void Menu::populate(const Menu_variant &objects)
  {
    entries.first = objects;
    entries.second.clear();
    if (std::holds_alternative<std::vector<Song>>(entries.first))
    {
      auto &songs = std::get<std::vector<Song>>(entries.first);
      entries.second.reserve(songs.size());
      for (const Song &song : songs) entries.second.emplace_back(song.artist + " ┃ " + song.title);
    }
    else
    {
      std::cerr << "Unsupported type for populate." << std::endl;
      exit(EXIT_FAILURE);
    }
    selected = 0;
  };

  Interface::Interface()
  {
    screen.SetCursor(cursor);
    container = ftxui::Container::Vertical({
      song_menu.component | ftxui::yframe,
    });
    renderer = ftxui::Renderer(container, [&] { return container->Render(); });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }
}
