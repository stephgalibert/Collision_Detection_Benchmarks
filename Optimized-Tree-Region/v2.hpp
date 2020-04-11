#pragma once

#include "IntRect.hpp"
#include "AvailableArray.hpp"

struct Element {
    int32_t id;
    IntRect dim;
};

struct ElementNode {
    int32_t index;
};

struct QuadNode {
    int32_t first_child;
};

struct Elements {
    AvailableArray<Element> elements;
};