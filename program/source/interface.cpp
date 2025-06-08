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
          {ftxui::hbox({ftxui::text(std::to_string(player.volume_percentage) + "%"), ftxui::separatorEmpty(),
                        ftxui::text(std::to_string(player.current_progress))}),
           ftxui::hbox({ftxui::text("Test"), ftxui::text("Test")}), ftxui::separatorHeavy(), container->Render()});
      });
  }

  ftxui::Loop Interface::create_loop() { return ftxui::Loop(&screen, renderer); }
}
