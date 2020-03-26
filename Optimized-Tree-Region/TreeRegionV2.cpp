#include "TreeRegionV2.hpp"

#include <iostream>


TreeRegionV2::TreeRegionV2()
    : dimension_({ 0, 0, 1920, 1080 })
{
    _initialize_root();
}

void TreeRegionV2::insert(int32_t id, const IntRect& rect)
{
     // std::clog << "inserting #" << id << ": " << rect << std::endl;
    _insert(0, id, dimension_, rect, 0);
}

void TreeRegionV2::intersect(const IntRect& elem_rect, std::vector<int32_t>& out) const
{
    _intersect(0, dimension_, elem_rect, out);
}

void TreeRegionV2::dump(void) const
{
    std::clog << "====== dump ======" << std::endl;
    _dump(0, dimension_);
    std::clog << "====== end dump ======" << std::endl;
    std::clog << "qnodes size=" << quad_nodes_.size() << std::endl;
    std::clog << "enodes size=" << element_nodes_.size() << std::endl;
}

void TreeRegionV2::_initialize_root()
{
    quad_nodes_.push({ 0 });
    element_nodes_.push({});
}

void TreeRegionV2::_insert(size_t index, int32_t id, const IntRect& node_rect, const IntRect& elem_rect, size_t depth)
{
    if (quad_nodes_[index].first_child <= 0) {
        element_nodes_[-quad_nodes_[index].first_child].elements.push({ id, elem_rect });
        if (depth < MaxDepth && element_nodes_[-quad_nodes_[index].first_child].elements.size() > MaxElementPerNode - 1)
            _subdivide(index, node_rect, depth);
    }
    else {
        const int16_t x = node_rect.x; const int16_t y = node_rect.y;
        const int16_t w = node_rect.w >> 1; const int16_t h = node_rect.h >> 1;
        if (elem_rect.intersects({ x, y, w, h }))
            _insert(quad_nodes_[index].first_child + 0, id, { x, y, w, h }, elem_rect, depth + 1); // NW
        if (elem_rect.intersects({ x + w, y, w, h }))
            _insert(quad_nodes_[index].first_child + 1, id, { x + w, y, w, h }, elem_rect, depth + 1); // NE
        if (elem_rect.intersects({ x, y + h, w, h }))
            _insert(quad_nodes_[index].first_child + 2, id, { x, y + h, w, h }, elem_rect, depth + 1); // SW 
        if (elem_rect.intersects({ x + w, y + h, w, h }))
            _insert(quad_nodes_[index].first_child + 3, id, { x + w, y + h, w, h }, elem_rect, depth + 1); // SE
    }
}

void TreeRegionV2::_intersect(size_t index, const IntRect& node_rect, const IntRect& elem_rect, std::vector<int32_t>& out) const
{
    if (quad_nodes_[index].first_child <= 0) {
        const AvailableArray<Element>& elements = element_nodes_[-quad_nodes_[index].first_child].elements;
        for (size_t i = 0; i < elements.size(); ++i)
            if (elem_rect.intersects(elements[i].dim))
                out.push_back(elements[i].id);
    }
    else {
        const int16_t x = node_rect.x; const int16_t y = node_rect.y;
        const int16_t w = node_rect.w >> 1; const int16_t h = node_rect.h >> 1;
        if (elem_rect.intersects({ x, y, w, h }))
            _intersect(quad_nodes_[index].first_child + 0, { x, y, w, h }, elem_rect, out); // NW
        if (elem_rect.intersects({ x + w, y, w, h }))
            _intersect(quad_nodes_[index].first_child + 1, { x + w, y, w, h }, elem_rect, out); // NE
        if (elem_rect.intersects({ x, y + h, w, h }))
            _intersect(quad_nodes_[index].first_child + 2, { x, y + h, w, h }, elem_rect, out); // SW 
        if (elem_rect.intersects({ x + w, y + h, w, h }))
            _intersect(quad_nodes_[index].first_child + 3, { x + w, y + h, w, h }, elem_rect, out); // SE
    }
}

void TreeRegionV2::_subdivide(size_t parent_index, const IntRect& node_rect, size_t depth)
{
    // Could be more optimized by initializing element_nodes while inserting
    // But we'll have to add if branches on intersection
    quad_nodes_[parent_index].first_child = quad_nodes_.push({ quad_nodes_[parent_index].first_child });;
    quad_nodes_.push({ -element_nodes_.push({}) });
    quad_nodes_.push({ -element_nodes_.push({}) });
    quad_nodes_.push({ -element_nodes_.push({}) });
    _transfer_elements(parent_index, quad_nodes_[parent_index].first_child, node_rect, depth + 1);
}

void TreeRegionV2::_transfer_elements(int32_t from_qnode, int32_t to_qnode, const IntRect& node_rect, size_t depth)
{
    const int16_t x = node_rect.x; const int16_t y = node_rect.y;
    const int16_t w = node_rect.w >> 1; const int16_t h = node_rect.h >> 1;
    AvailableArray<Element> elements;

    element_nodes_[-quad_nodes_[to_qnode].first_child].elements.swap(elements);
    for (size_t i = 0; i < elements.size() ; ++i) {
        if (elements[i].dim.intersects({ x, y, w, h }))
            element_nodes_[-quad_nodes_[to_qnode + 0].first_child].elements.push(elements[i]);
        if (elements[i].dim.intersects({ x + w, y, w, h }))
            element_nodes_[-quad_nodes_[to_qnode + 1].first_child].elements.push(elements[i]);
        if (elements[i].dim.intersects({ x, y + h, w, h }))
            element_nodes_[-quad_nodes_[to_qnode + 2].first_child].elements.push(elements[i]);
        if (elements[i].dim.intersects({ x + w, y + h, w, h }))
            element_nodes_[-quad_nodes_[to_qnode + 3].first_child].elements.push(elements[i]);
    }
    if (depth < MaxDepth && element_nodes_[-quad_nodes_[to_qnode + 0].first_child].elements.size() > MaxElementPerNode)
        _subdivide(to_qnode + 0, node_rect, depth);
    if (depth < MaxDepth && element_nodes_[-quad_nodes_[to_qnode + 1].first_child].elements.size() > MaxElementPerNode)
        _subdivide(to_qnode + 1, node_rect, depth);
    if (depth < MaxDepth && element_nodes_[-quad_nodes_[to_qnode + 2].first_child].elements.size() > MaxElementPerNode)
        _subdivide(to_qnode + 2, node_rect, depth);
    if (depth < MaxDepth && element_nodes_[-quad_nodes_[to_qnode + 3].first_child].elements.size() > MaxElementPerNode)
        _subdivide(to_qnode + 3, node_rect, depth);

}

void TreeRegionV2::_dump(int32_t index, const IntRect& node_rect, int32_t depth) const
{
    const std::string prefix = std::string(depth * 2, ' ');

    if (quad_nodes_[index].first_child > 0) {
        std::clog << prefix << "#" << index << ": " << node_rect << std::endl;
        const int16_t x = node_rect.x; const int16_t y = node_rect.y;
        const int16_t w = node_rect.w >> 1; const int16_t h = node_rect.h >> 1;
        _dump(quad_nodes_[index].first_child + 0, { x, y, w, h }, depth + 1);
        _dump(quad_nodes_[index].first_child + 1, { x + w, y, w, h }, depth + 1);
        _dump(quad_nodes_[index].first_child + 2, { x, y + h, w, h }, depth + 1);
        _dump(quad_nodes_[index].first_child + 3, { x + w, y + h, w, h }, depth + 1);
    }
    else {
        std::clog << prefix << "#" << index << ": ~" << -quad_nodes_[index].first_child << ": " << node_rect << " "
            << "size: " << element_nodes_[-quad_nodes_[index].first_child].elements.size() << ": ";
        for (const auto& element : element_nodes_[-quad_nodes_[index].first_child].elements)
            std::clog << element.id << ",";
        std::clog << std::endl;
    }
}