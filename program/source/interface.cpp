#include "interface.hpp"

#include <string>

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/dom/elements.hpp"

#include "player.hpp"

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
        return ftxui::vbox(
          {ftxui::hbox({ftxui::separatorEmpty(),
                        ftxui::text(player.current_song.artist + " ┃ " + player.current_song.title + " ┃ " +
                                    std::to_string(player.current_song.plays) + " ┃ " +
                                    std::to_string(player.current_song.mean_volume)),
                        ftxui::separatorEmpty()}) |
             ftxui::center,
           ftxui::hbox(
             {ftxui::separatorEmpty(), ftxui::text(player.progress_text), ftxui::text(" ┃"),
              ftxui::gaugeRight(player.progress_percentage / 100.0f), ftxui::text("┃ "),
              ftxui::text((player.volume_percentage < 100 ? (player.volume_percentage < 10 ? "  " : " ") : "") +
                          std::to_string(player.volume_percentage) + "%"),
              ftxui::separatorEmpty()}),
           ftxui::separatorHeavy(), container->Render()});
      });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }
}
