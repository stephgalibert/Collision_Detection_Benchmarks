#include "GridV0.hpp"

#include <cmath>
#include <iostream>

GridV0::GridV0()
{
    setSurfaceRect({ 0, 0, 1920, 1080 });
}

void GridV0::setSurfaceRect(const IntRect& rect)
{
    surfaceRect_ = rect;
    updateSurface();
}

void GridV0::insert(int32_t id, const IntRect& rect)
{
    if (!surfaceRect_.intersects(rect))
        return;

    const Vector2i nbCells = getNbCellsImpacted(rect);
    const Vector2i cell = getStartingCell(rect);

    for (int y = cell.y; y <= cell.y + nbCells.y; ++y)
        for (int x = cell.x; x <= cell.x + nbCells.x; ++x) {
            const int coord = y * dim_.x + x;
            elements_[coord].push_back({ id, rect });
        }
}

void GridV0::remove(int32_t id, const IntRect& rect)
{
    if (!surfaceRect_.intersects(rect))
        return;

    const Vector2i nbCells = getNbCellsImpacted(rect);
    const Vector2i cell = getStartingCell(rect);

    for (int y = cell.y; y <= cell.y + nbCells.y; ++y)
        for (int x = cell.x; x <= cell.x + nbCells.x; ++x) {
            const int coord = y * dim_.x + x;
            std::vector<Element>& v = elements_.at(coord);
            v.erase(std::remove_if(std::begin(v), std::end(v),
                [&](const Element& elm) -> bool {
                    return elm.id == id;
                }), std::end(v));
        }
}

void GridV0::query(const IntRect& rect, std::vector<int32_t>& results) const
{
    if (!surfaceRect_.intersects(rect))
        return;

    const Vector2i nbCells = getNbCellsImpacted(rect);
    const Vector2i cell = getStartingCell(rect);

    for (int y = cell.y; y <= cell.y + nbCells.y; ++y)
        for (int x = cell.x; x <= cell.x + nbCells.x; ++x) {
            const int coord = y * dim_.x + x;
            for (const auto& it : elements_.at(coord))
                if (it.dim.intersects(rect))
                    results.emplace_back(it.id);
        }
}

void GridV0::query(const Point& point, std::vector<int32_t>& results) const
{
    if (!surfaceRect_.contains(point))
        return;

    const Vector2i cell = getStartingCell(point);
    const int coord = cell.y * dim_.x + cell.x;

    for (const auto& it : elements_.at(coord))
        if (it.dim.contains(point))
            results.emplace_back(it.id);
}

void GridV0::clear()
{
    elements_.clear();
    elements_.resize((dim_.x + 1) * (dim_.y + 1));
}

void GridV0::updateSurface()
{
    cellSize_ = { surfaceRect_.w / 100 , surfaceRect_.h / 100 };
    dim_ = { surfaceRect_.w / cellSize_.x, surfaceRect_.h / cellSize_.y };
    clear();
}

Vector2i GridV0::getNbCellsImpacted(const IntRect& rect) const
{
    return {
        static_cast<int>(std::ceil(((rect.x % cellSize_.x) + rect.w) / cellSize_.x)),
        static_cast<int>(std::ceil(((rect.y % cellSize_.y) + rect.h) / cellSize_.y)),
    };
}

Vector2i GridV0::getStartingCell(const IntRect& rect) const
{
    return {
        rect.x / cellSize_.x,
        rect.y / cellSize_.y
    };
}

Vector2i GridV0::getStartingCell(const Point& point) const
{
    return {
        point.x / cellSize_.x,
        point.y / cellSize_.y
    };
}