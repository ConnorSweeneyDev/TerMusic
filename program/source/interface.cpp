#include "interface.hpp"

#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/ref.hpp"

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
        if (event == ftxui::Event::g)
        {
          selected = 0;
          return true;
        }
        if (event == ftxui::Event::G)
        {
          selected = static_cast<int>(entries.size()) - 1;
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
    container = ftxui::Container::Vertical({
      song_menu.component | ftxui::yframe,
    });
    renderer = ftxui::Renderer(container, [&] { return container->Render(); });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }
}
