#pragma once

#include "IntRect.hpp"

struct Item {
    using Id = int32_t;
    Item(Id id, const IntRect& dim) : id(id), dim(dim) {}
    Id id;
    IntRect dim;
};

inline std::ostream& operator<<(std::ostream& os, const std::vector<Item>& elements)
{
    os << "{";
    for (const auto& element : elements)
        os << "[" << element.id << ":" << element.dim << "]" << ",";
    os << "}";
    return os;
}
