#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "IntRect.hpp"
#include "v2.hpp"

class DumbTreeRegion
{
private:
	static constexpr int Capacity = 256;

public:
	// DumbTreeRegion(void);
	DumbTreeRegion(const IntRect& rect);
	virtual ~DumbTreeRegion(void);

	void insert(int32_t id, const IntRect& rect);
	void remove(int32_t id, const IntRect& rect);
	void query(const IntRect& rect, std::vector<Element>& results) const;
	void query(const Point& point, std::vector<Element>& results) const;

	void clear(void);
	bool isEmpty(void) const;

private:
	void subdivide(void);
	void join(void);
	bool isDivided(void) const;

private:
	IntRect rect_;

	std::shared_ptr<DumbTreeRegion> nw_;
	std::shared_ptr<DumbTreeRegion> ne_;
	std::shared_ptr<DumbTreeRegion> sw_;
	std::shared_ptr<DumbTreeRegion> se_;

	std::vector<Element> elems_;
};