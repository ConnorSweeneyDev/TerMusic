#include "interface.hpp"

#include <string>

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"

#include "player.hpp"
#include "utility.hpp"

namespace tuim
{
  Interface::Interface()
  {
    screen.SetCursor(cursor);
    container = ftxui::Container::Vertical({
      song_menu.component | ftxui::yframe,
    });
    renderer = ftxui::Renderer(
      container,
      [&]
      {
        return ftxui::vbox({
          ftxui::hbox({
            ftxui::separatorEmpty(),
            ftxui::text(player.current_song.artist + " ┃ " + player.current_song.title),
            ftxui::separatorEmpty(),
          }) |
            ftxui::color(ftxui::Color::GrayLight) | ftxui::center | ftxui::bold,
          ftxui::hbox({
            ftxui::separatorEmpty(),
            ftxui::text(player.progress_text),
            ftxui::text(" ┃"),
            ftxui::gaugeRight(player.progress_percentage / 100.0f),
            ftxui::text("┃ "),
            ftxui::text(utility::seconds_to_string(player.current_song.duration)),
            ftxui::text((player.volume_percentage < 100 ? (player.volume_percentage < 10 ? "   " : "  ") : " ") +
                        std::to_string(player.volume_percentage) + "%"),
            ftxui::text(" " + (search_term.empty() ? "~" : search_term)) | search_based_color(),
            ftxui::separatorEmpty(),
          }) |
            ftxui::color(ftxui::Color::GrayLight) | ftxui::bold,
          ftxui::separatorHeavy() | pause_based_color(),
          container->Render(),
        });
      });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }

  ftxui::Decorator Interface::pause_based_color()
  {
    return [&](ftxui::Element element)
    { return element | (player.paused ? ftxui::color(ftxui::Color::Red) : ftxui::color(ftxui::Color::Blue)); };
  }

  ftxui::Decorator Interface::search_based_color(bool reverse)
  {
    if (reverse)
      return [&](ftxui::Element element)
      { return element | (searching ? ftxui::color(ftxui::Color::Grey70) : ftxui::color(ftxui::Color::GrayLight)); };

    return [&](ftxui::Element element)
    { return element | (searching ? ftxui::color(ftxui::Color::GrayLight) : ftxui::color(ftxui::Color::Grey70)); };
  }
}
