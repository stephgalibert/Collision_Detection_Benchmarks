#include <iostream>
#include <chrono>
#include <functional>

#include <random>
#include <chrono>

#include "Optimized-Tree-Region.hpp"
#include "TreeRegionV2.hpp"

double timed_code(std::function<void(void)> code)
{
    auto start = std::chrono::high_resolution_clock::now();
    code();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

void monitorize_v1_insertion(OptimizedQuadtree&tree, std::vector<int16_t> x, std::vector<int16_t> y)
{
    if (x.size() != y.size())
        return;

    auto ctor_duration = timed_code([&]() {
        for (size_t i = 0; i < x.size(); ++i)
            tree.rq_insert(i, { x[i], y[i], 2, 2 });
    });
    std::clog << "Insertion v1 of " << x.size() << " objects: total duration: " << ctor_duration << " second(s)" << std::endl;
}

void monitorize_v1_intersection(const OptimizedQuadtree& tree, const IntRect& rect)
{
    std::vector<int32_t> results;
    double intersect_duration = timed_code([&]() { tree.rq_intersect(rect, results); });
    std::clog << "Intersect v1 duration: " << intersect_duration << " second(s), "
              << "zone: " << rect << ", "
              << "nb of result(s):" << results.size() << std::endl;
}

void monitorize_v2_insertion(TreeRegionV2&tree, std::vector<int16_t> x, std::vector<int16_t> y)
{
    if (x.size() != y.size())
        return;

    auto ctor_duration = timed_code([&]() {
        for (size_t i = 0; i < x.size(); ++i)
            tree.insert(i, { x[i], y[i], 2, 2 });
    });
    std::clog << "Insertion v2 of " << x.size() << " objects: total duration: " << ctor_duration << " second(s)" << std::endl;
}

void monitorize_v2_intersection(const TreeRegionV2& tree, const IntRect& rect)
{
    std::vector<int32_t> results;
    double intersect_duration = timed_code([&]() { tree.intersect(rect, results); });
    std::clog << "Intersect v2 duration: " << intersect_duration << " second(s), "
              << "zone: " << rect << ", "
              << "nb of result(s):" << results.size() << std::endl;
}

int main() {
    std::clog << "v1 sizeof(IntRect)=" << sizeof(IntRect) << " "
        << "sizeof(Node)=" << sizeof(Node) << " "
        << "sizeof(Node::Id)=" << sizeof(Node::Id) << " "
        << "sizeof(Nodes)=" << sizeof(Nodes) << " "
        << std::endl;

    std::clog << "v2 sizeof(QuadNode)=" << sizeof(QuadNode) << " "
              << "sizeof(ElementNode)=" << sizeof(ElementNode) << " "
              << "sizeof(Element)=" << sizeof(Element) << " "
              << std::endl;

    int64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rd_engine(seed);

    // generating fake coords
    // TODO: generate fake weight/height
    std::vector<int16_t> x, y;
    for (int i = 0; i < 10000000; ++i)
    {
        x.push_back(static_cast<int16_t>(rd_engine() % 1919));
        y.push_back(static_cast<int16_t>(rd_engine() % 1079));
    }

    OptimizedQuadtree treev1;
    monitorize_v1_insertion(treev1, x, y);
    monitorize_v1_intersection(treev1, { 0, 0, 1920/2, 1080/2 });
    monitorize_v1_intersection(treev1, { 0, 0, 1920, 1080 });

    rd_engine = std::mt19937(seed);
    TreeRegionV2 treev2;
    monitorize_v2_insertion(treev2, x, y);
    monitorize_v2_intersection(treev2, { 0, 0, 1920/2, 1080/2 });
    monitorize_v2_intersection(treev2, { 0, 0, 1920, 1080 });


    // treev2.dump();

    //std::vector<int32_t> results;
    //treev2.intersect({ 1485,995,10,10 }, results);
    //std::clog << "results: ";
    //for (const auto& result : results)
    //    std::clog << result << ", ";
    //std::clog << std::endl;

    return 0;
}
