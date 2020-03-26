#include "Optimized-Tree-Region.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>

#include <iomanip>

#include "IntRect.hpp"

OptimizedQuadtree::OptimizedQuadtree()
{
    nodes_.root.push_back(RootNodeId);
    nodes_.nodes.insert({ RootNodeId, Node({0, 0, 1920, 1080}) });
    _it_subdivide(0, 0);
}

bool OptimizedQuadtree::rq_insert(Item::Id id, const IntRect & dim)
{
    return _do_rq_insert(0, id, dim, 0);
}

bool OptimizedQuadtree::it_insert(Item::Id id, const IntRect& dim)
{
    return _do_it_insert(0, id, dim, 0);
}

void OptimizedQuadtree::remove(Item::Id id)
{
}

void OptimizedQuadtree::it_intersect(const IntRect& dim, std::vector<Item::Id>& out) const
{
    _do_it_intersect(0, dim, out);
}

void OptimizedQuadtree::rq_intersect(const IntRect& dim, std::vector<Item::Id>& out) const
{
    _do_rq_intersect(0, dim, out);
}

void OptimizedQuadtree::dump(bool unused) const
{
    _do_dump(0, unused);
}

bool OptimizedQuadtree::_do_rq_insert(size_t index, Item::Id id, const IntRect& dim, int current_depth)
{
    const Node::Id node_id = nodes_.root.at(index);

    if (node_id == FreeNodeId)
        return false;

    Node& node = nodes_.nodes.at(node_id);
    if (!dim.intersects(node.dim))
        return false;

    if (!node.leaf) {
        for (int i = 1; i <= 4; ++i)
            _do_rq_insert(index * 4 + i, id, dim, current_depth + 1);
        return true;
    }

    node.items.emplace_back(id, dim);
    if (current_depth < MaxDepth && node.items.size() > MaxItemsPerNode)
        // The current node was entirely filled, subdivide it
        _rq_subdivide(index, current_depth);
    return true;
}

bool OptimizedQuadtree::_do_it_insert(size_t index, Item::Id id, const IntRect& dim, int current_depth)
{
    std::queue<size_t> to_process;

    to_process.push(index);
    while (!to_process.empty())
    {
        size_t next = to_process.front(); // inefficient
        to_process.pop();

        const Node::Id node_id = nodes_.root.at(next);

        if (node_id == FreeNodeId)
            continue;

        Node& node = nodes_.nodes.at(node_id);
        if (!dim.intersects(node.dim))
            continue;

        if (!node.leaf) {
            for (int i = 1; i <= 4; ++i)
                to_process.push(next * 4 + i);
            continue;
        }

        node.items.emplace_back(id, dim);
        if (current_depth < MaxDepth && node.items.size() > MaxItemsPerNode)
            // The current node was entirely filled, subdivide it
            _it_subdivide(next, current_depth);
    }

    return true;
}

void OptimizedQuadtree::_do_rq_intersect(size_t index, const IntRect& dim, std::vector<Item::Id>& out) const
{
    //if (index >= nodes_.root.size())
    //    return;

    const Node::Id node_id = nodes_.root.at(index);

    if (node_id == FreeNodeId)
        return;

    const Node& node = nodes_.nodes.at(node_id);
    if (!dim.intersects(node.dim))
        return;

    if (!node.leaf) {
        for (int i = 1; i <= 4; ++i)
            _do_rq_intersect(index * 4 + i, dim, out);
        return;
    }

    for (const auto& item : node.items)
        if (item.dim.intersects(dim))
            out.push_back(item.id);
}


void OptimizedQuadtree::_do_it_intersect(size_t index, const IntRect& dim, std::vector<Item::Id>& out) const
{
    std::queue<size_t> to_process;

    to_process.push(0);
    while (!to_process.empty())
    {
        size_t next = to_process.front();
        to_process.pop();

        const Node::Id node_id = nodes_.root.at(next);
        if (node_id == FreeNodeId)
            continue;

        const Node& node = nodes_.nodes.at(node_id);
        if (!dim.intersects(node.dim))
            continue;

        if (!node.leaf) {
            for (int i = 1; i <= 4; ++i)
                to_process.push(next * 4 + i);
            continue;
        }

        for (const auto& item : node.items)
            if (item.dim.intersects(dim))
                out.push_back(item.id);
    }
}

void OptimizedQuadtree::_do_dump(size_t index, bool unused, std::size_t depth) const
{
    if (index >= nodes_.root.size())
        return;

    const std::string prefix = std::string(depth * 2, ' ');
    const Node::Id id = nodes_.root.at(index);

    if (id == FreeNodeId) {
        if (unused)
            std::clog << prefix << "[" << index << "]" << ": unused" << std::endl;
    }
    else {
        const Node& node = nodes_.nodes.at(id);
        std::clog << prefix << "[" << index << "]" << ": #" << id << "=" << node.dim << " leaf=" << std::boolalpha << node.leaf;
        if (!node.items.empty())
            std::clog << " size=" << node.items.size() << ".." << node.items << std::endl;
        else
            std::clog << std::endl;
    }

    for (int i = 1; i <= 4; ++i)
        _do_dump(index * 4 + i, unused, depth + 1);
}

void OptimizedQuadtree::_rq_subdivide(size_t parent_index, int parent_depth)
{
    const Node::Id parent_id = nodes_.root.at(parent_index);
    Node& parent_node = nodes_.nodes.at(parent_id);
    const IntRect& dim = parent_node.dim;

    const int16_t x = dim.x; const int16_t y = dim.y;
    const int16_t w = dim.w / 2; const int16_t h = dim.h / 2;
    const std::vector<IntRect> dims = {
        { x + w, y, w, h }, { x, y, w, h },
        { x, y + h, w, h }, { x + w, y + h, w, h},
    };

    const size_t needed_size = parent_index * 4 + 4 + 1;
    if (nodes_.root.size() < needed_size)
        nodes_.root.resize(needed_size, FreeNodeId);

    for (int i = 1; i <= 4; ++i) {
        const int index = parent_index * 4 + i;
        Node::Id& v = nodes_.root[index];

        if (v != FreeNodeId) {
            std::clog << "Warning: index " << index << " is not free. Skipping." << std::endl;
            continue;
        }

        const int id = next_node_id_++;
        nodes_.root[index] = id;
        auto inserted = nodes_.nodes.insert({ id, Node(dims[i - 1]) });
        inserted.first->second.leaf = true;

        for (const auto& item : parent_node.items)
            _do_rq_insert(index, item.id, item.dim, parent_depth + 1);
    }
    parent_node.leaf = false;
    parent_node.items.clear();
}

void OptimizedQuadtree::_it_subdivide(size_t parent_index, int parent_depth)
{
    const Node::Id parent_id = nodes_.root.at(parent_index);
    Node& parent_node = nodes_.nodes.at(parent_id);
    const IntRect& dim = parent_node.dim;

    const int16_t x = dim.x; const int16_t y = dim.y;
    const int16_t w = dim.w / 2; const int16_t h = dim.h / 2;
    const std::vector<IntRect> dims = {
        { x + w, y, w, h }, { x, y, w, h },
        { x, y + h, w, h }, { x + w, y + h, w, h},
    };

    const size_t needed_size = parent_index * 4 + 4 + 1;
    if (nodes_.root.size() < needed_size)
        nodes_.root.resize(needed_size, FreeNodeId);

    for (int i = 1; i <= 4; ++i) {
        const int index = parent_index * 4 + i;
        Node::Id& v = nodes_.root[index];

        if (v != FreeNodeId) {
            std::clog << "Warning: index " << index << " is not free. Skipping." << std::endl;
            continue;
        }

        const int id = next_node_id_++;
        nodes_.root[index] = id;
        auto inserted = nodes_.nodes.insert({ id, Node(dims[i - 1]) });
        inserted.first->second.leaf = true;

        for (const auto& item : parent_node.items)
            _do_it_insert(index, item.id, item.dim, parent_depth + 1);
    }
    parent_node.leaf = false;
    parent_node.items.clear();
}

void OptimizedQuadtree::_subdivide_v2()
{

}