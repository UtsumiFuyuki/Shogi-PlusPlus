#include <array>
#include <cstdlib>
#include <ui.hpp>
#include <vector>
#include <string>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>

ftxui::App *screen{};

void uiInit() {
    using namespace ftxui;

    std::system("clear");

    std::vector<std::string> entries{
        "New Match",
        "Settings",
        "Quit"
    };
    int selection{};

    auto menu = Menu(&entries, &selection);
    auto renderer = Renderer(menu, [&] {
        return vbox({
            hbox({text("Shogi++")}) | center,
            separator(),
            menu->Render()
        }) | border;
    });

    auto app = App::TerminalOutput();
    screen = &app;
    screen->Loop(renderer);
}