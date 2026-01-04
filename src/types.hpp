#ifndef DS_TYPES_H
#define DS_TYPES_H

struct ConfigStruct
{
    std::vector<std::string> activeAppList;
    bool activeForAllApps;
    unsigned int totalDesktopCount;
    bool isKeyboardSwitchingOn;
    bool isCursorSwitchingOn;
    bool isMouseSwitchingFollowsActiveAppListRule;
    unsigned int offsetPixels;
};

struct Vector2
{
    Vector2() : x(0), y(0) {}
    Vector2(long _x, long _y) : x(_x), y(_y) {}
    long x;
    long y;
};

enum CURSORPOS
{
    MIDDLE,
    LEFT_SIDE,
    RIGHT_SIDE
};

enum ERRORS
{
    FILE_OK,
    FILE_NOT_FOUND,
    FILE_CANNOT_BE_CREATED,
    FILE_CANNOT_OPEN,
    CANNOT_WRITE_TO_THE_FILE,
    WINDOW_CANNOT_BE_CREATED,
    GLFW_CANNOT_INITIALIZED,
    OPENGL_CANNOT_INITIALIZED

};

#endif