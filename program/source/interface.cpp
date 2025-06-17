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
            ftxui::center | ftxui::bold,
          ftxui::hbox({
            ftxui::separatorEmpty(),
            ftxui::text(player.progress_text),
            ftxui::text(" ┃") | reactive_color(),
            ftxui::gaugeRight(player.progress_percentage / 100.0f) | reactive_color(),
            ftxui::text("┃ ") | reactive_color(),
            ftxui::text(utility::seconds_to_string(player.current_song.duration)),
            ftxui::text((player.volume_percentage < 100 ? (player.volume_percentage < 10 ? "   " : "  ") : " ") +
                        std::to_string(player.volume_percentage) + "%"),
            ftxui::separatorEmpty(),
          }) |
            ftxui::bold,
          ftxui::separatorHeavy(),
          container->Render(),
        });
      });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }

  ftxui::Decorator Interface::reactive_color()
  {
    return [&](ftxui::Element element)
    { return element | (player.paused ? ftxui::color(ftxui::Color::Red) : ftxui::color(ftxui::Color::Blue)); };
  }
}
