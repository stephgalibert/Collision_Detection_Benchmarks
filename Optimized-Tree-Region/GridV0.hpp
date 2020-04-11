#pragma once

#include <vector>

#include "IntRect.hpp"
#include "AvailableArray.hpp"
#include "v2.hpp"

class GridV0
{
public:
    GridV0();
    ~GridV0() = default;

    void setSurfaceRect(const IntRect& rect);

    void insert(int32_t id, const IntRect& rect);
    void remove(int32_t id, const IntRect& rect);

    void query(const IntRect& rect, std::vector<int32_t>& results) const;
    void query(const Point& point, std::vector<int32_t>& results) const;

    void clear();

private:
    void updateSurface();
    [[nodiscard]] Vector2i getNbCellsImpacted(const IntRect& rect) const;
    [[nodiscard]] Vector2i getStartingCell(const IntRect& rect) const;
    [[nodiscard]] Vector2i getStartingCell(const Point& point) const;

private:
    IntRect surfaceRect_;
    Vector2i cellSize_;
    Vector2i dim_;
    std::vector<std::vector<Element>> elements_;
};

