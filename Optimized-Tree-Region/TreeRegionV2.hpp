#pragma once

#include <vector>

#include "IntRect.hpp"
#include "v2.hpp"


class TreeRegionV2
{
private:
    const size_t MaxElementPerNode = 256;
    const size_t MaxDepth = 9;

public:
    TreeRegionV2();
    ~TreeRegionV2() = default;

    void insert(int32_t id, const IntRect &rect);
    void remove(int32_t id, const IntRect& rect);
    void intersect(const IntRect& elem_rect, std::vector<int32_t>& out) const;
    std::vector<int32_t> intersectv2(const IntRect& elem_rect) const;

    void dump(void) const;

private:
    void _initialize_root();

    void _insert(size_t index, int32_t id, const IntRect& node_rect, const IntRect& elem_rect, size_t depth);
    void _remove(size_t index, int32_t id, const IntRect& node_rect, const IntRect& elem_rect);
    void _intersect(size_t index, const IntRect& node_rect, const IntRect& elem_rect, std::vector<int32_t>& out) const;

    void _subdivide(size_t index, const IntRect& node_rect, size_t depth);
    void _transfer_elements(int32_t qnode_index, int32_t element_node_index, const IntRect& node_rect, size_t depth);
    void _dump(int32_t index, const IntRect& node_rect, int32_t depth = 0) const;

private:
    IntRect dimension_;
    AvailableArray<QuadNode> quad_nodes_;
    AvailableArray<Elements> element_nodes_;
};

