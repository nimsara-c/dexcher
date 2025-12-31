#ifndef DS_TYPES_H
#define DS_TYPES_H

struct ConfigStruct
{
    std::vector<std::string> activeAppList;
    unsigned int totalDesktopCount;
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

#endif