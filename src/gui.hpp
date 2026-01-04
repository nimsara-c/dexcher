#ifndef DEXCHER_GUI_H
#define DEXCHER_GUI_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_STATIC
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
#include <vector>
#include <thread>
#include <cstring>

#include "Application.hpp"
#include "types.hpp"
#include "utils.hpp"

class GUI
{
public:
    GUI(std::string _title, Vector2 _windowSize) : m_windowTitle(_title)
    {
        m_winSize.x = _windowSize.x;
        m_winSize.y = _windowSize.y;

        Dexcher::loadConfig(this->m_config);

        std::strcpy(m_activeAppListBuffer, Dexcher::strVecToChar(this->m_config.activeAppList));
    }

    ~GUI()
    {
    }

private:
    GLFWwindow *m_window;
    ImGuiIO *m_io;
    ImFont *m_tabFont, *m_ctntFont, *m_logFont;
    ImVec2 m_winSize;
    ImVec4 m_clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Transparent
    ImVec4 m_activeTabTextColor = ImVec4(1.0, 1.0, 1.0, 1.0);
    ImVec4 m_primaryTextColor = ImVec4(0.2, 0.2, 0.2, 1.0);
    std::string m_windowTitle;
    std::string m_fontPath = "style/fonts/Montserrat-Medium.ttf";
    bool m_isClosed = false;
    char m_activeAppListBuffer[1024 * 16] = "Hello, world!\nThis is a multiline text area in ImGui.";
    long m_totalDesktopCount = 2;
    ConfigStruct m_config;

private:
    void createNewFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void setCustomTheme(ImGuiStyle &_style)
    {
        ImVec4 *colors = _style.Colors;

        colors[ImGuiCol_Tab] = ImVec4(0.9, 0.9, 0.9, 1.0);
        colors[ImGuiCol_TabHovered] = ImVec4(0.9, 0.3, 0.2, 0.3);
        colors[ImGuiCol_TabActive] = ImVec4(0.8, 0.3, 0.2, 0.6);
        colors[ImGuiCol_WindowBg] = ImVec4(1, 1, 1, 1);
        colors[ImGuiCol_TitleBgActive] = ImVec4(1.0, 1.0, 1.0, 1.0);
        colors[ImGuiCol_Text] = this->m_primaryTextColor;
        colors[ImGuiCol_FrameBg] = ImVec4(0.9, 0.9, 0.9, 1.0);

        colors[ImGuiCol_Button] = ImVec4(0.9, 0.9, 0.9, 1.0);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.9, 0.3, 0.2, 0.5);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.8, 0.3, 0.2, 0.8);

        // Scrollbar styles
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.8, 0.8, 0.8, 1.0);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.8, 0.3, 0.2, 0.8);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.8, 0.3, 0.2, 1.0);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8, 0.3, 0.2, 1.0);

        colors[ImGuiCol_CheckMark] = ImVec4(0.8, 0.3, 0.2, 0.9);

        _style.TabRounding = 4.0;
        _style.WindowRounding = 2.0;
        _style.WindowTitleAlign = ImVec2(0.5, 0.5);
        _style.FrameRounding = 4.0;
        _style.WindowPadding = ImVec2(10, 10);
        _style.FramePadding = ImVec2(7.0, 4.0);
        _style.ItemSpacing = ImVec2(10.0, 10.0);
        _style.ItemInnerSpacing = ImVec2(2.0, 0.0);
    }

    void setTextColor(ImVec4 _color)
    {
        ImGuiStyle &_style = ImGui::GetStyle();
        ImVec4 *colors = _style.Colors;
        colors[ImGuiCol_Text] = _color;
    }

    void loadWindowIcon()
    {
        GLFWimage images[1];
        images[0].pixels = stbi_load("style/images/favicon.png", &images[0].width, &images[0].height, 0, 4); // rgba channels
        glfwSetWindowIcon(this->m_window, 1, images);
        stbi_image_free(images[0].pixels);
    }

    int init()
    {

        if (!glfwInit())
            return ERRORS::GLFW_CANNOT_INITIALIZED;

        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        this->m_window = glfwCreateWindow(this->m_winSize.x, this->m_winSize.y, this->m_windowTitle.c_str(), NULL, NULL);
        if (m_window == NULL)
            return ERRORS::WINDOW_CANNOT_BE_CREATED;

        glfwSetWindowAttrib(m_window, GLFW_ICONIFIED, GLFW_TRUE);
        this->loadWindowIcon();
        glfwMakeContextCurrent(this->m_window);
        glfwSwapInterval(1); // Enable vsync

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        this->m_io = &ImGui::GetIO();
        (void)*(this->m_io);
        this->m_io->IniFilename = NULL;
        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();
        this->setCustomTheme(style);

        if (!ImGui_ImplGlfw_InitForOpenGL(this->m_window, true))
            return ERRORS::GLFW_CANNOT_INITIALIZED;
        if (!ImGui_ImplOpenGL3_Init(glsl_version))
            return ERRORS::OPENGL_CANNOT_INITIALIZED;

        this->m_tabFont = this->m_io->Fonts->AddFontFromFileTTF(m_fontPath.c_str(), 20.0f); // 22 - Comic
        this->m_ctntFont = this->m_io->Fonts->AddFontFromFileTTF(m_fontPath.c_str(), 18.0f);
        this->m_logFont = this->m_io->Fonts->AddFontFromFileTTF(m_fontPath.c_str(), 16.0f);

        return 0;
    }

public:
    int render(bool &_isGUIRunning, bool &_isConfigFileChanged)
    {
        this->init();

        int display_w, display_h;

        while (!glfwWindowShouldClose(this->m_window))
        {
            glfwPollEvents();

            // Close Window if exit selected from tray icon
            if (!_isGUIRunning)
                glfwSetWindowShouldClose(this->m_window, GLFW_TRUE);

            this->createNewFrame();

            // Main Window
            {
                static bool use_work_area = true;
                static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize; // = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                this->m_winSize = viewport->Size;
                ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
                ImGui::SetNextWindowSize(this->m_winSize);
                ImGui::SetNextWindowFocus();

                ImGui::Begin(this->m_windowTitle.c_str(), NULL, flags); // Create the Main Window

                ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None);

                // ------------------------------- TAB: Settings -----------------------------------------
                if (ImGui::BeginTabItem("Settings"))
                {
                    ImGui::PushFont(this->m_ctntFont);
                    ImGui::Indent();

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Total Desktops: \t");
                    ImGui::SameLine();

                    if (ImGui::Button("-"))
                    {
                        if (this->m_config.totalDesktopCount > 1)
                            this->m_config.totalDesktopCount--;
                    }
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(this->m_config.totalDesktopCount).c_str());
                    // ImGui::SetNextItemWidth(40.0);
                    // ImGui::InputText("##desktops", this->m_totalDesktopCount, IM_ARRAYSIZE(this->m_totalDesktopCount), ImGuiInputTextFlags_CharsDecimal);
                    ImGui::SameLine();
                    if (ImGui::Button("+"))
                    {
                        this->m_config.totalDesktopCount++;
                    }

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Active app list:");

                    ImGui::SameLine();

                    ImGui::PushFont(this->m_logFont);
                    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
                    ImGui::Checkbox(" All apps", &this->m_config.activeForAllApps);
                    // ImGui::PopStyleVar();

                    ImVec2 size = ImVec2(-1.0f, ImGui::GetTextLineHeight() * 5);
                    ImGui::InputTextMultiline("##ActiveAppList",                                                                        // Label (hidden using "##")
                                              this->m_activeAppListBuffer,                                                              // Buffer
                                              IM_ARRAYSIZE(this->m_activeAppListBuffer),                                                // Buffer size helper macro
                                              size,                                                                                     // Size
                                              this->m_config.activeForAllApps ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None // No specific flags needed for basic use
                    );
                    ImGui::PopFont();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
                    ImGui::Checkbox(" Keyboard desktop switching", &this->m_config.isKeyboardSwitchingOn);
                    ImGui::Checkbox(" Mouse desktop switching", &this->m_config.isCursorSwitchingOn);
                    ImGui::Checkbox(" Mouse switching follows active app list", &this->m_config.isMouseSwitchingFollowsActiveAppListRule);
                    ImGui::PopStyleVar();

                    // Save Button
                    ImGui::Indent(this->m_winSize.x - 200);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f, 8.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 35.0f));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8, 0.3, 0.2, 1.0));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
                    if (ImGui::Button("Save", ImVec2(0, 0)))
                    {
                        this->m_config.activeAppList = Dexcher::charArrayToStrVec(this->m_activeAppListBuffer);
                        Dexcher::writeConfig(this->m_config);
                        _isConfigFileChanged = true;
                    }

                    ImGui::PopStyleColor(2);
                    ImGui::SameLine();

                    if (ImGui::Button("Close", ImVec2(0, 0)))
                    {
                        glfwSetWindowShouldClose(this->m_window, GLFW_TRUE);
                    }
                    ImGui::PopStyleVar(2);
                    ImGui::Unindent();

                    ImGui::EndTabItem();
                    ImGui::PopFont();
                }

                // ------------------------------- TAB: Advanced -----------------------------------------
                if (ImGui::BeginTabItem("Advanced"))
                {
                    ImGui::PushFont(this->m_ctntFont);
                    ImGui::Indent();

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Mouse Activation Offset: \t");
                    ImGui::SameLine();

                    if (ImGui::Button("-"))
                    {
                        if (this->m_config.offsetPixels > 1)
                            this->m_config.offsetPixels--;
                    }
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(this->m_config.offsetPixels).c_str());
                    // ImGui::SetNextItemWidth(40.0);
                    // ImGui::InputText("##desktops", this->m_totalDesktopCount, IM_ARRAYSIZE(this->m_totalDesktopCount), ImGuiInputTextFlags_CharsDecimal);
                    ImGui::SameLine();
                    if (ImGui::Button("+"))
                    {
                        this->m_config.offsetPixels++;
                    }

                    // Save button
                    ImGui::Indent(this->m_winSize.x - 140);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f, 8.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 35.0f));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8, 0.3, 0.2, 1.0));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
                    if (ImGui::Button("Save", ImVec2(0, 0)))
                    {
                        this->m_config.activeAppList = Dexcher::charArrayToStrVec(this->m_activeAppListBuffer);
                        Dexcher::writeConfig(this->m_config);
                        _isConfigFileChanged = true;
                    }
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar(2);
                    ImGui::Unindent();

                    ImGui::EndTabItem();
                    ImGui::PopFont();
                }

                // ------------------------------- TAB: Help -----------------------------------------
                if (ImGui::BeginTabItem("Help"))
                {

                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0, 1.0, 1.0, 1.0));
                    ImGui::BeginChildFrame(2, ImVec2(this->m_winSize.x - 20, this->m_winSize.y - 70), ImGuiWindowFlags_HorizontalScrollbar);

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 5.0f));

                    ImGui::Indent();
                    ImGui::PushFont(this->m_tabFont);
                    ImGui::TextColored(ImVec4(1, 0.517, 0.034, 0.9), "Usage:");
                    ImGui::PopFont();
                    ImGui::Unindent();

                    ImGui::Indent(20.0);
                    ImGui::PushFont(this->m_ctntFont);
                    ImGui::Text("Esc - Keyboard switching key\nMove mouse to left or right sides to switch desktops accordingly.");
                    ImGui::PopFont();
                    ImGui::Unindent(20.0);

                    ImGui::Indent();
                    ImGui::PushFont(this->m_tabFont);
                    ImGui::TextColored(ImVec4(1, 0.517, 0.034, 0.9), "\nSettings:");
                    ImGui::PopFont();
                    ImGui::Unindent();

                    ImGui::Indent(20.0);
                    ImGui::PushFont(this->m_ctntFont);
                    ImGui::Text("Active app list");
                    ImGui::PopFont();
                    ImGui::Unindent(20.0);

                    ImGui::Indent(25.0);
                    ImGui::PushFont(this->m_logFont);
                    ImGui::Text("List of apps that Dexcher switches desktops when active. The name you \nare entering must be one word from the title name when you get by \nhovering over application icon in titlebar");
                    ImGui::PopFont();
                    ImGui::Unindent(25.0);

                    ImGui::Indent();
                    ImGui::PushFont(this->m_tabFont);
                    ImGui::TextColored(ImVec4(1, 0.517, 0.034, 0.9), "\nAbout:");
                    ImGui::PopFont();
                    ImGui::Unindent();

                    ImGui::Indent(20.0);
                    ImGui::PushFont(this->m_ctntFont);
                    ImGui::Text("Developed by Nimsara Chamindu");
                    ImGui::Text("Email: ");
                    ImGui::SameLine();
                    // bool isSelected;
                    //  ImGui::Selectable("nimsarathedev@gmail.com", &isSelected, ImGuiSelectableFlags_None);
                    char email[30] = "nimsarathedev@gmail.com";
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0, 0.0));
                    ImGui::InputText("##Email", email, 30, ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopStyleVar();

                    ImGui::PopFont();
                    ImGui::Unindent(20.0);

                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor(); // Frame_BG

                    ImGui::EndChildFrame();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();

                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            glfwGetFramebufferSize(this->m_window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(this->m_clear_color.x * this->m_clear_color.w, this->m_clear_color.y * this->m_clear_color.w, this->m_clear_color.z * this->m_clear_color.w, this->m_clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(this->m_window);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        this->m_isClosed = true;

        // Cleanup
        this->terminateImGui();
        this->terminateGLFW();

        return 0;
    }

private:
    void terminateImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void terminateGLFW()
    {
        glfwDestroyWindow(this->m_window);
        glfwTerminate();
    }
};

#endif