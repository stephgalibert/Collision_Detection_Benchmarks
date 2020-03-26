#pragma once

#include <unordered_map>
#include <ostream>
#include <vector>

#include "IntRect.hpp"
#include "Item.hpp"


struct Node {
    using Id = int;
    explicit Node(const IntRect& dim) : dim(dim) {}
    IntRect dim;
    std::vector<Item> items;
    bool leaf = true;
};

struct Nodes {
    // Continuous memory block containing the whole tree.
    // Childs can be accessed with [parent_index * N + k] where N represents the tree dimensions and K the child number.
    // Could be more optimized by using handmade container.
    // Important Note: The contained type must be small as possible to avoid cache misses.
    std::vector<Node::Id> root;

    // Each node id contained in `root` point to a Node stored in the following container.
    // The idea behind this is that we want to retrieve the corresponding Node faster as possible.
    // A Hash map indexed by a `Node::Id` seems to be a good solution since there is an O(1) complexity.
    std::unordered_map<Node::Id, Node> nodes;

    // Get node id from user id
    // std::unordered_map<int, Node::Id> user_id_to_node_id;
};


inline std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "(" << node.dim << ")" << "[" << node.items << "]";
    return os;
}