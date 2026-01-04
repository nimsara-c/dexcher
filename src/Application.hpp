#ifndef APPLICATION_H
#define APPLICATION_H

// From Standard Sources
#include <iostream>
#include <windows.h> // Required for Windows API functions
#include <chrono>
#include <thread>
#include <tchar.h> // Required for _tprintf and TCHAR
#include <string>
#include <vector>
#include <fstream>

// From External Sources
#include <nlohmann/json.hpp>
#include <tray/tray.hpp>

// Custom Files
#include "types.hpp"
#include "gui.hpp"
#include "utils.hpp"

class Application
{
public:
    Application()
    {
        Dexcher::loadConfig(this->m_config);
        std::cout << "Monitoring keyboard in the background. Press 'End' to exit.\n";

        // Retrieves the handle of the current console window
        HWND hwnd = GetConsoleWindow();
        // Hides the window
        ShowWindow(hwnd, SW_HIDE);
        CloseHandle(hwnd);

        // Store Screen Resolution
        this->m_screenRes = this->getPrimaryScreenResolution();
    }

    ~Application()
    {
    }

    void run()
    {
        std::thread guiThread(&Application::runGUI, this);

        /*
        std::thread trayThread(&Application::runTrayIcon, this);

        // Main loop
        while (this->m_isRunning)
        {
            if (this->m_isConfigFileChanged)
            {
                Dexcher::loadConfig(this->m_config);
                this->m_isConfigFileChanged = false;
            }

            if (this->m_config.isCursorSwitchingOn)
                this->handleCursorSwitching();

            if (this->m_config.isKeyboardSwitchingOn)
                this->handleKeyboardSwitching();

            // Add a small delay to prevent high CPU usage and rapid-fire detection
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        trayThread.join();
        */
        guiThread.join();
    }

private:
    ConfigStruct m_config;
    unsigned int m_currDesktopNo = 1;
    bool m_isRunning = true;
    bool m_isConfigFileChanged;
    Vector2 m_screenRes;

private:
    // Run tray icon in a separate thread
    void runTrayIcon()
    {
        Tray::Tray tray("Dexcher", "favicon.ico");

        long activeTotalDesktopCountState = (this->m_config.totalDesktopCount < 4) ? this->m_config.totalDesktopCount : 4;
        bool totalDesktopCountStatesList[4] = {false, false, false, false};
        totalDesktopCountStatesList[activeTotalDesktopCountState - 1] = true;

        tray.addEntry(Tray::Button("Configure", [&]
                                   {
                std::thread guiThread(&Application::runGUI, this);
                guiThread.join(); }));

        Tray::Submenu submenu("Total Desktop Count");
        submenu.addEntries(
            Tray::SyncedToggle("1", totalDesktopCountStatesList[0], [&](bool _state)
                               { if(_state == true){ setTotalDesktopCountStatesList(totalDesktopCountStatesList, 4, 1); this->m_isConfigFileChanged = true; this->m_config.totalDesktopCount = 1; Dexcher::writeConfig(this->m_config); } }),
            Tray::SyncedToggle("2", totalDesktopCountStatesList[1], [&](bool _state)
                               { if(_state == true){ setTotalDesktopCountStatesList(totalDesktopCountStatesList, 4, 2); this->m_isConfigFileChanged = true; this->m_config.totalDesktopCount = 2; Dexcher::writeConfig(this->m_config); } }),
            Tray::SyncedToggle("3", totalDesktopCountStatesList[2], [&](bool _state)
                               { if(_state == true){ setTotalDesktopCountStatesList(totalDesktopCountStatesList, 4, 3); this->m_isConfigFileChanged = true; this->m_config.totalDesktopCount = 3; Dexcher::writeConfig(this->m_config); } }),
            Tray::SyncedToggle("Custom", totalDesktopCountStatesList[3], [&](bool _state)
                               { setTotalDesktopCountStatesList(totalDesktopCountStatesList, 4, 4); std::thread guiThread(&Application::runGUI, this); guiThread.join(); }));

        tray.addEntry(submenu);

        tray.addEntry(Tray::Button("Exit", [&]
                                   { tray.exit(); this->m_isRunning = false; }));

        tray.run();
    }

    void setTotalDesktopCountStatesList(bool *_list, int _length, int _activeDesktopCount)
    {
        for (int i = 0; i < _length; i++)
        {
            _list[i] = false;
        }

        _list[_activeDesktopCount - 1] = true;
    }

    void runGUI()
    {
        GUI gui("Dexcher", Vector2(560, 350));
        gui.render(this->m_isRunning, this->m_isConfigFileChanged);
    }

    /*
    void initConfig(std::string jsonFileName = "settings.json")
    {
        using json = nlohmann::json;

        std::ifstream jsonFile(jsonFileName);
        json data = json::parse(jsonFile);
        jsonFile.close();

        this->m_config.activeAppList = data["activeAppList"].get<std::vector<std::string>>();
        this->m_config.totalDesktopCount = data["totalDesktopCount"].get<unsigned int>();
        this->m_config.offsetPixels = data["offsetPixels"].get<unsigned int>();
        this->m_config.isKeyboardSwitchingOn = data["turnOnKeyboardSwitching"].get<bool>();
        this->m_config.isCursorSwitchingOn = data["turnOnMouseSwitching"].get<bool>();
        this->m_config.isMouseSwitchingFollowsActiveAppListRule = data["doesMouseSwitchingFollowsActiveAppListRule"].get<bool>();
    }
    */

    Vector2 getPrimaryScreenResolution()
    {
        Vector2 m_screenRes;
        // SM_CXSCREEN for screen width, SM_CYSCREEN for screen height
        m_screenRes.x = GetSystemMetrics(SM_CXSCREEN);
        m_screenRes.y = GetSystemMetrics(SM_CYSCREEN);

        return m_screenRes;
    }

    Vector2 getCurrentCursorPos()
    {
        POINT p;
        // Get the cursor position in screen coordinates
        if (GetCursorPos(&p))
        {
            return Vector2(p.x, p.y);
        }
        else
        {
            return Vector2(-1, -1);
        }
    }

    CURSORPOS checkCursorPosStatus(long error = 5)
    {
        Vector2 mousePos = this->getCurrentCursorPos();

        if (mousePos.x <= error)
        {
            return CURSORPOS::LEFT_SIDE;
        }

        if (mousePos.x + error >= this->m_screenRes.x)
        {
            return CURSORPOS::RIGHT_SIDE;
        }

        return CURSORPOS::MIDDLE;
    }

    void handleKeyboardSwitching()
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            if (isCurrentWindowInActiveAppList() || this->m_config.activeForAllApps)
            {
                std::cout << "Switching Desktop" << "\n";
                if (m_currDesktopNo < this->m_config.totalDesktopCount)
                {
                    m_currDesktopNo++;
                    switchDesktop(VK_RIGHT);
                }
                else
                {
                    m_currDesktopNo = 1;
                    for (unsigned short i = this->m_config.totalDesktopCount; i > m_currDesktopNo; i--)
                    {
                        switchDesktop(VK_LEFT);
                        Sleep(300);
                    }
                }
            }

            Sleep(300);
        }
    }

    void handleCursorSwitching()
    {
        // Check if cursor follows active app list rule and a window in list is active or doesn't follow the rule at all
        if ((this->m_config.isMouseSwitchingFollowsActiveAppListRule && this->isCurrentWindowInActiveAppList()) || !this->m_config.isMouseSwitchingFollowsActiveAppListRule || this->m_config.activeForAllApps)
        {

            CURSORPOS status = this->checkCursorPosStatus(this->m_config.offsetPixels);

            if (status == CURSORPOS::LEFT_SIDE)
            {
                if (this->m_currDesktopNo > 1)
                {
                    this->switchDesktop(VK_LEFT);
                    this->m_currDesktopNo--;
                    Sleep(300);
                }
            }

            if (status == CURSORPOS::RIGHT_SIDE)
            {
                if (this->m_currDesktopNo < this->m_config.totalDesktopCount)
                {
                    this->switchDesktop(VK_RIGHT);
                    this->m_currDesktopNo++;
                    Sleep(300);
                }
            }
        }
    }

    std::string getActiveWindowTitle()
    {
        // Get a handle to the foreground window (the active window)
        HWND hwnd = GetForegroundWindow();

        if (hwnd != NULL)
        {
            // Buffer to store the window title (maximum length 256 characters)
            TCHAR window_title[256];

            // Get the window text (title) using the handle
            // The TCHAR alias automatically selects ANSI or Unicode versions based on project settings.
            GetWindowText(hwnd, window_title, sizeof(window_title) / sizeof(TCHAR));

            std::wstring _windowTitle;
            _windowTitle.assign(window_title, window_title + strlen(window_title));

            std::string windowTitle(_windowTitle.begin(), _windowTitle.end());
            return windowTitle;
        }
        else
        {
            std::cout << "No foreground window found or another desktop is active (e.g., screen saver)./n";
            return std::string("");
        }
    }

    bool isCurrentWindowInActiveAppList()
    {
        std::string currWindowName = Dexcher::strToLowerCase(getActiveWindowTitle());
        for (const auto &appName : m_config.activeAppList)
        {
            size_t status = currWindowName.find(Dexcher::strToLowerCase(appName));
            if (status != std::string::npos)
                return true;
        }
        return false;
    }

    // Function to simulate a key press (key down event)
    void keyPress(WORD vkCode)
    {
        INPUT input = {}; // Initialize the INPUT structure to zeros
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vkCode; // Virtual-key code for the key
        input.ki.dwFlags = 0;  // 0 for key press
        SendInput(1, &input, sizeof(INPUT));
    }

    // Function to simulate a key release (key up event)
    void keyRelease(WORD vkCode)
    {
        INPUT input = {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vkCode;              // Virtual-key code for the key
        input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
        SendInput(1, &input, sizeof(INPUT));
    }

    void switchDesktop(unsigned int side)
    {
        keyPress(VK_CONTROL);
        keyPress(VK_LWIN);
        keyPress(side);

        keyRelease(VK_CONTROL);
        keyRelease(VK_LWIN);
        keyRelease(side);
    }
};

#endif