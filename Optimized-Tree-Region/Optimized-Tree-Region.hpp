#pragma once

#include <unordered_set>

#include "Nodes.hpp"
#include "Item.hpp"

class OptimizedQuadtree
{
private:
    // Current configuration...
    static const size_t FreeNodeId = 0;
    static const size_t RootNodeId = FreeNodeId + 1;

    static const size_t MaxItemsPerNode = 256;
    static const size_t MaxDepth = 9;

public:
    OptimizedQuadtree();
    ~OptimizedQuadtree() = default;

    bool it_insert(Item::Id id, const IntRect& dim);
    bool rq_insert(Item::Id id, const IntRect& dim);
    void remove(Item::Id id);

    void rq_intersect(const IntRect& dim, std::vector<Item::Id>& out) const;
    void it_intersect(const IntRect& dim, std::vector<Item::Id>& out) const;
    void dump(bool unused = false) const;

private:
    bool _do_it_insert(size_t index, Item::Id id, const IntRect& dim, int current_depth);
    bool _do_rq_insert(size_t index, Item::Id id, const IntRect& dim, int current_depth);
    void _do_it_intersect(size_t index, const IntRect& dim, std::vector<Item::Id>& out) const;
    void _do_rq_intersect(size_t index, const IntRect& dim, std::vector<Item::Id>& out) const;
    void _do_dump(size_t index, bool unused = false, std::size_t depth = 0) const;

    void _it_subdivide(size_t parent_index, int parent_depth);
    void _rq_subdivide(size_t parent_index, int parent_depth);
    void _subdivide_v2();

private:
    Nodes nodes_;
    size_t next_node_id_ = RootNodeId + 1; // Starting at 2;
};

