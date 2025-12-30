
/*
    Main source file of the project dexcher(Desktop Switching application)
        Authour: Nimsara Chamindu
*/

#include "Application.hpp"

/*
#include <tray/tray.hpp>
using Tray::Button;
using Tray::Tray;

void runTrayIcon()
{
    Tray::Tray tray("My Tray", "favicon.ico");
    tray.addEntry(Button("Exit", [&]
                         { tray.exit(); }));

    tray.run();
}
*/

int main()
{

    // std::thread trayThread(runTrayIcon);

    Application app;
    app.run();

    // trayThread.join();

    return 0;
}
