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

class Application
{
public:
    Application()
    {
        this->initConfig();
        std::cout << "Monitoring keyboard in the background. Press 'End' to exit.\n";

        // Retrieves the handle of the current console window
        HWND hwnd = GetConsoleWindow();
        // Hides the window
        ShowWindow(hwnd, SW_HIDE);

        // Store Screen Resolution
        this->screenRes = this->getPrimaryScreenResolution();
    }

    ~Application()
    {
    }

    void run()
    {
        // Create a separate thread for the tray icon
        std::thread trayThread(&Application::runTrayIcon, this);

        // Main loop
        while (this->isRunning)
        {
            this->handleCursorSwitching();
            // Check if the 'End' key is pressed (VK_END is the virtual key code for 'End')
            if (GetAsyncKeyState(VK_END) & 0x8000)
            {
                std::cout << "End key pressed. Exiting..." << std::endl;
                break;
            }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {
                if (isCurrentWindowInActiveAppList())
                {
                    std::cout << "Switching Desktop" << "\n";
                    if (currDesktopNo < this->_config.totalDesktopCount)
                    {
                        currDesktopNo++;
                        switchDesktop(VK_RIGHT);
                    }
                    else
                    {
                        currDesktopNo = 1;
                        for (unsigned short i = this->_config.totalDesktopCount; i > currDesktopNo; i--)
                        {
                            switchDesktop(VK_LEFT);
                            Sleep(300);
                        }
                    }
                }

                Sleep(500);
            }

            // Add a small delay to prevent high CPU usage and rapid-fire detection
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        trayThread.join();
    }

private:
    ConfigStruct _config;
    unsigned int currDesktopNo = 1;
    bool isRunning = true;
    Vector2 screenRes;

private:
    // Run tray icon in a separate thread
    void runTrayIcon()
    {
        using Tray::Button;
        using Tray::Tray;

        Tray tray("Dexcher", "favicon.ico");
        tray.addEntry(Button("Exit", [&]
                             {
            tray.exit();
            this->isRunning = false; }));

        tray.run();
    }

    void initConfig(std::string jsonFileName = "settings.json")
    {
        using json = nlohmann::json;

        std::ifstream jsonFile(jsonFileName);
        json data = json::parse(jsonFile);
        jsonFile.close();

        this->_config.activeAppList = data["activeAppList"].get<std::vector<std::string>>();
        this->_config.totalDesktopCount = data["totalDesktopCount"].get<unsigned int>();
    }

    Vector2 getPrimaryScreenResolution()
    {
        Vector2 screenRes;
        // SM_CXSCREEN for screen width, SM_CYSCREEN for screen height
        screenRes.x = GetSystemMetrics(SM_CXSCREEN);
        screenRes.y = GetSystemMetrics(SM_CYSCREEN);

        return screenRes;
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

        if (mousePos.x + error >= this->screenRes.x)
        {
            return CURSORPOS::RIGHT_SIDE;
        }

        return CURSORPOS::MIDDLE;
    }

    void handleCursorSwitching()
    {
        CURSORPOS status = this->checkCursorPosStatus();

        if (status == CURSORPOS::LEFT_SIDE)
        {
            if (this->currDesktopNo > 1)
            {
                this->switchDesktop(VK_LEFT);
                this->currDesktopNo--;
                Sleep(300);
            }
        }

        if (status == CURSORPOS::RIGHT_SIDE)
        {
            if (this->currDesktopNo < this->_config.totalDesktopCount)
            {
                this->switchDesktop(VK_RIGHT);
                this->currDesktopNo++;
                Sleep(300);
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
        std::string currWindowName = getActiveWindowTitle();
        for (const auto &appName : _config.activeAppList)
        {
            size_t status = currWindowName.find(appName);
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