#pragma once

#include <list>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>

class SliceAllocator;

typedef boost::shared_ptr<SliceAllocator> SliceAllocatorPtr;

struct CornerCacheEntry
{
	CornerCacheEntry() {}
	CornerCacheEntry(int _x, int _y, double _fieldValue, bool _cubeDone = false, int _lx = -1, int _ly = -1, int _lz = -1)
		: x(_x), y(_y), fieldValue(_fieldValue), cubeDone(_cubeDone), lx(_lx), ly(_ly), lz(_lz) {}

	CornerCacheEntry(int _lx, int _ly, int _lz)
		: x(0), y(0), fieldValue(-1.0), cubeDone(false), lx(_lx), ly(_ly), lz(_lz) {}

	int x;
	int y;
	double fieldValue;
	bool cubeDone;
	int lx;
	int ly;
	int lz;
	float spaceCoord[3];
};


struct SliceEntry 
{
	SliceEntry() : doneBelow(false), doneAbove(false), cornerCacheIndex(-1) {}
	
	bool doneBelow;
	bool doneAbove;
	int cornerCacheIndex;
};


class SliceAllocator
{
private:
	SliceEntry* _pool;
	int _xSize;
	int _ySize;

	SliceEntry** _chunks;
	int _capacity;
	int _top;

	SliceAllocator(const SliceAllocator& other) {}
	SliceAllocator& operator=(const SliceAllocator& other) {}

	SliceAllocator(int xSize, int ySize, int capacity)
		: _xSize(xSize), _ySize(ySize), _capacity(capacity), _top(0)
	{
		_pool = new SliceEntry[_xSize * _ySize * _capacity];
		_chunks = new SliceEntry*[_capacity];
		for (int i = 0; i < _capacity; i++) {
			_chunks[i] = &_pool[i * _xSize * _ySize];
		}
	}

public:

	static SliceAllocatorPtr getNewAllocator(int xSize, int ySize, int capacity) 
	{
		return boost::shared_ptr<SliceAllocator>(new SliceAllocator(xSize, ySize, capacity));
	}

	~SliceAllocator()
	{
		if (_pool) delete [] _pool;
		if (_chunks) delete [] _chunks;
	}

	SliceEntry* allocate()
	{
		return _chunks[_top++];
	}

	void deallocate(SliceEntry* mem)
	{
		_chunks[--_top] = mem;
	}
};

struct Slice 
{
	Slice() : data(0) {}
	~Slice() 
	{
		deallocate();
	}

	std::vector<CornerCacheEntry> cornerCache;
	SliceEntry* data;
	int sizeX;
	int sizeY;
	SliceAllocatorPtr allocator;

	bool isAllocated() { return data != 0; }

	SliceEntry& operator()(int x, int y)
	{
		return data[x * sizeY + y];
	}

	// value less than zero mean there is no entry in the cache for given x and y
	double getFieldValueAt(int x, int y) {
		SliceEntry& se = this->operator()(x, y);
		if (se.cornerCacheIndex == -1 || !cornerCache[se.cornerCacheIndex].cubeDone) {
			return -1.0;
		} else {
			return cornerCache[se.cornerCacheIndex].fieldValue;
		}
	}

	void ensureAllocated(int sizeX, int sizeY, SliceAllocatorPtr& alloc)
	{
		if (isAllocated()) 
			return;

		allocator = alloc;
		this->sizeX = sizeX;
		this->sizeY = sizeY;
		data = allocator->allocate();
		cornerCache.reserve(sizeX * sizeY);
	}

	void deallocate() 
	{
		if (isAllocated()) {
			for (std::vector<CornerCacheEntry>::iterator it = cornerCache.begin(); it != cornerCache.end(); ++it) {
				this->operator()(it->x, it->y).cornerCacheIndex = -1;
				this->operator()(it->x, it->y).doneAbove = false;
				this->operator()(it->x, it->y).doneBelow = false;
			}
			
			allocator->deallocate(data);
			data = 0;
		}
		cornerCache.clear();
	}
};


