#pragma once

#include "interface.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/util/ref.hpp"

#include "player.hpp"
#include "song.hpp"

namespace tuim
{
  template <typename Type> Menu<Type>::Menu()
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
          if constexpr (std::is_same_v<Type, Song>)
          {
            player.play(entries.first[static_cast<size_t>(selected)]);
            return true;
          }
          else
          {
            std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
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

  template <typename Type> void Menu<Type>::populate(const std::vector<Type> &objects)
  {
    entries.first = objects;
    entries.second.clear();
    if constexpr (std::is_same_v<Type, Song>)
    {
      entries.second.reserve(entries.first.size());
      for (const Song &song : entries.first) entries.second.emplace_back(song.artist + " ┃ " + song.title);
    }
    else
    {
      std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
      exit(EXIT_FAILURE);
    }
  };

}
