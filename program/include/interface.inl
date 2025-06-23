#pragma once

#include "interface.hpp"

#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/util/ref.hpp"

#include "database.hpp"
#include "player.hpp"
#include "song.hpp"
#include "utility.hpp"

namespace tuim
{
  template <typename Type> Menu<Type>::Menu()
  {
    option.focused_entry = ftxui::Ref<int>(&selected);
    option.entries_option.transform = [&](ftxui::EntryState state)
    {
      int max_index_length = static_cast<int>(std::log10(entries.second.size())) + 1;
      std::string extra_spaces = "";
      std::string index_string = "";
      if (current_entry == state.index)
      {
        for (int i = 0; i < max_index_length - 1; i++) extra_spaces += " ";
        index_string = " " + extra_spaces + "> ";
      }
      else
      {
        int index_length = static_cast<int>(std::log10(state.index + 1)) + 1;
        for (int i = 0; i < max_index_length - index_length; i++) extra_spaces += " ";
        index_string = " " + extra_spaces + std::to_string(state.index + 1) + " ";
      }
      ftxui::Element display = ftxui::text(index_string + state.label);
      if (state.active || current_entry == state.index)
        display = display | interface.search_based_color(true);
      else
        display = display | ftxui::color(ftxui::Color::Grey70);
      display = display | ftxui::bold | ftxui::bgcolor(ftxui::Color::RGBA(0, 0, 0, 0));
      return display;
    };
    component = ftxui::Menu(&entries.second, &selected, option);
    component |= ftxui::CatchEvent(
      [&](ftxui::Event event)
      {
        if (interface.searching)
        {
          if (event == ftxui::Event::Escape)
          {
            interface.searching = false;
            interface.search_term = "";
            if constexpr (std::is_same_v<Type, Song>)
            {
              interface.song_menu.populate(
                database.query<Song>("SELECT * FROM songs ORDER BY LOWER(artist) ASC, LOWER(title) ASC;"));
              selected = 0;
              move_to(player.current_song);
            }
            else
            {
              std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
              exit(EXIT_FAILURE);
            }
            return true;
          }
          if (event == ftxui::Event::Return)
          {
            interface.searching = false;
            if constexpr (std::is_same_v<Type, Song>)
            {
              std::vector<Song> results =
                database.query<Song>("SELECT * FROM songs WHERE LOWER(artist) LIKE LOWER(?) "
                                     "OR LOWER(title) LIKE LOWER(?) ORDER BY LOWER(artist) ASC, LOWER(title) ASC;",
                                     "%" + interface.search_term + "%", "%" + interface.search_term + "%");
              if (results.empty())
              {
                interface.search_term = "";
                interface.song_menu.populate(
                  database.query<Song>("SELECT * FROM songs ORDER BY LOWER(artist) ASC, LOWER(title) ASC;"));
              }
              else
                interface.song_menu.populate(results);
              selected = 0;
              move_to(player.current_song);
            }
            else
            {
              std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
              exit(EXIT_FAILURE);
            }
            return true;
          }
          if (event == ftxui::Event::Backspace)
          {
            if (interface.search_term.empty()) return true;
            interface.search_term.pop_back();
            return true;
          }
          if (event.is_character())
          {
            interface.search_term += event.character();
            return true;
          }
        }

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
        if (event == ftxui::Event::c)
        {
          if constexpr (std::is_same_v<Type, Song>)
            move_to(player.current_song);
          else
          {
            std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
            exit(EXIT_FAILURE);
          }
          return true;
        }
        if (event == ftxui::Event::Character("/"))
        {
          interface.searching = true;
          interface.search_term = "";
          return true;
        }
        if (event == ftxui::Event::Escape)
        {
          if (interface.search_term.empty()) return true;
          interface.search_term = "";
          if constexpr (std::is_same_v<Type, Song>)
          {
            interface.song_menu.populate(
              database.query<Song>("SELECT * FROM songs ORDER BY LOWER(artist) ASC, LOWER(title) ASC;"));
            selected = 0;
            move_to(player.current_song);
          }
          else
          {
            std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
            exit(EXIT_FAILURE);
          }
          return true;
        }
        if (event == ftxui::Event::p)
        {
          player.toggle_pause();
          return true;
        }
        if (event == ftxui::Event::h)
        {
          player.seek_by(-5);
          return true;
        }
        if (event == ftxui::Event::l)
        {
          player.seek_by(5);
          return true;
        }
        if (event.is_character() && utility::is_number(event.character()))
        {
          player.seek_to(std::stoi(event.character()) * 10);
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
        if (event == ftxui::Event::q)
        {
          database.execute("UPDATE state SET song_path = ?, song_percentage = ?, volume_percentage = ? WHERE id = ?;",
                           player.current_song.path.string(), static_cast<int>(std::round(player.progress_percentage)),
                           player.volume_percentage, 0);
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
      for (const Song &song : entries.first)
        entries.second.emplace_back((song.artist == "" || song.title == "") ? song.path.stem().string()
                                                                            : (song.artist + " ┃ " + song.title));
    }
    else
    {
      std::cerr << "Unsupported type " << typeid(Type).name() << " for Menu." << std::endl;
      exit(EXIT_FAILURE);
    }
  };

  template <typename Type> void Menu<Type>::move_to(const Type &object)
  {
    for (size_t i = 0; i < entries.first.size(); i++)
      if (entries.first[i] == object)
      {
        selected = static_cast<int>(i);
        current_entry = selected;
        return;
      }
    current_entry = INT_MAX;
  }
}
