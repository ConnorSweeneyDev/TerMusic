#include "interface.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/dom/elements.hpp"

namespace tuim
{
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
