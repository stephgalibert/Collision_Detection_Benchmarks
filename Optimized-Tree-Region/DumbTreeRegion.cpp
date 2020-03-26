#include "DumbTreeRegion.hpp"

#include <iostream>
#include <algorithm>

DumbTreeRegion::DumbTreeRegion(const IntRect& rect)
	: rect_(rect)
{
}

DumbTreeRegion::~DumbTreeRegion(void)
{
}

void DumbTreeRegion::insert(int32_t id, const IntRect& rect)
{
	if (!rect_.intersects(rect)) {
		return;
	}

	if (isDivided()) {
		nw_->insert(id, rect);
		ne_->insert(id, rect);
		sw_->insert(id, rect);
		se_->insert(id, rect);
	}
	else {
		elems_.emplace_back(Element({ id, rect }));
		if (elems_.size() > Capacity) {
			subdivide();
		}
	}
}

void DumbTreeRegion::remove(int32_t id, const IntRect& rect)
{
	if (!rect_.intersects(rect)) {
		return;
	}

	if (isDivided()) {
		nw_->remove(id, rect);
		ne_->remove(id, rect);
		sw_->remove(id, rect);
		se_->remove(id, rect);
		join();
	}
	else {
		elems_.erase(std::remove_if(std::begin(elems_), std::end(elems_),
			[&](Element const& elm) -> bool {
				return elm.id == id;
			}), std::end(elems_));
	}
}

void DumbTreeRegion::query(const IntRect& rect, std::vector<int32_t>& results) const
{
	if (!rect_.intersects(rect)) {
		return;
	}
	if (isDivided()) {
		nw_->query(rect, results);
		ne_->query(rect, results);
		sw_->query(rect, results);
		se_->query(rect, results);
	}
	else {
		for (auto&& it : elems_) {
			if (it.dim.intersects(rect)) {
				results.push_back(it.id);
			}
		}
	}
}

void DumbTreeRegion::query(const Point& point, std::vector<int32_t>& results) const
{
	if (!rect_.contains(point)) {
		return;
	}
	if (isDivided()) {
		nw_->query(point, results);
		ne_->query(point, results);
		sw_->query(point, results);
		se_->query(point, results);
	}
	else {
		for (auto&& it : elems_) {
			if (it.dim.contains(point)) {
				results.push_back(it.id);
			}
		}
	}
}

void DumbTreeRegion::clear(void)
{
	nw_.reset();
	ne_.reset();
	sw_.reset();
	se_.reset();
	elems_.clear();
}

bool DumbTreeRegion::isEmpty(void) const
{
	return !isDivided() && elems_.empty();
}

void DumbTreeRegion::subdivide(void)
{
	const int16_t x = rect_.x; const int16_t y = rect_.y;
	const int16_t w = rect_.w >> 1; const int16_t h = rect_.h >> 1;

	nw_ = std::make_shared<DumbTreeRegion>(IntRect({ x + w, y, w, h }));
	ne_ = std::make_shared<DumbTreeRegion>(IntRect({ x, y, w, h }));
	sw_ = std::make_shared<DumbTreeRegion>(IntRect({ x, y + h, w, h }));
	se_ = std::make_shared<DumbTreeRegion>(IntRect({ x + w, y + h, w, h }));

	for (auto&& it : elems_) {
		nw_->insert(it.id, it.dim);
		ne_->insert(it.id, it.dim);
		sw_->insert(it.id, it.dim);
		se_->insert(it.id, it.dim);
	}
	elems_.clear();
}

void DumbTreeRegion::join(void)
{
	if (nw_->isEmpty() && ne_->isEmpty() && sw_->isEmpty() && se_->isEmpty()) {
		clear();
	}
}

bool DumbTreeRegion::isDivided(void) const
{
	return nw_ != nullptr;
}