#pragma once

#include <ostream>

struct Point {
    int16_t x;
    int16_t y;
};

struct IntRect {
    int16_t x, y, w, h;
    bool intersects(const IntRect& other) const
    {
        return x < other.x + other.w && x + w > other.x
            && y < other.y + other.h && y + h > other.y;
    }
    bool contains(const Point& point) const
    {
        return point.x > x && point.x < x + w
            && point.y > y && point.y < y + h;
    }
};

inline std::ostream& operator<<(std::ostream& os, const IntRect& rect)
{
    os << "(" << rect.x << "," << rect.y << "," << rect.w << "," << rect.h << ")";
    return os;
}