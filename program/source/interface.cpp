#include "interface.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/ref.hpp"

#include "database.hpp"
#include "song.hpp"

namespace tuim
{
  Menu::Menu()
  {
    option.focused_entry = ftxui::Ref<int>(&selected);
    component = ftxui::Menu(&entries, &selected, option);
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
        if (event.mouse().motion == ftxui::Mouse::Motion::Moved ||
            event.mouse().motion == ftxui::Mouse::Motion::Pressed)
          return true;
        return false;
      });
  }

  void Menu::populate(const std::vector<Song> &songs)
  {
    entries.clear();
    entries.reserve(songs.size());
    for (const Song &song : songs) entries.emplace_back(song.artist + " | " + song.title);
    selected = 0;
  };

  Interface::Interface()
  {
    screen.SetCursor(cursor);

    std::vector<tuim::Song> all_songs =
      tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table.name + " ORDER BY artist ASC, title ASC");
    if (all_songs.empty())
    {
      std::cerr << "No songs found!" << std::endl;
      exit(EXIT_FAILURE);
    }
    song_menu.populate(all_songs);

    container = ftxui::Container::Vertical({
      song_menu.component | ftxui::yframe,
    });
    renderer = ftxui::Renderer(container, [&] { return container->Render(); });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }
}
