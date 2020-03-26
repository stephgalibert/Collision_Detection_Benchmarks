#pragma once

#include "IntRect.hpp"
#include "AvailableArray.hpp"

struct Element {
    int32_t id;
    IntRect dim;
};

struct QuadNode {
    int32_t first_child;
};

struct ElementNode {
    AvailableArray<Element> elements;
};