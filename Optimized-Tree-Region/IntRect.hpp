#pragma once

#include <ostream>

struct IntRect {
    int16_t x, y, w, h;
    bool intersects(const IntRect& other) const
    {
        return x < other.x + other.w && x + w > other.x
            && y < other.y + other.h && y + h > other.y;
    }
};

inline std::ostream& operator<<(std::ostream& os, const IntRect& rect)
{
    os << "(" << rect.x << "," << rect.y << "," << rect.w << "," << rect.h << ")";
    return os;
}
