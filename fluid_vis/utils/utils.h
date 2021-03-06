#pragma once


#define UTILS_PRINT_VAR(var) \
	std::cout << #var << ": " << var << std::endl;

namespace Utils
{
	template <typename T>
	void printArray(T* a, int components, int count);

	class NonCopyable 
	{
	protected:
		NonCopyable() {}
		~NonCopyable() {}
	private:
		NonCopyable(const NonCopyable& other) {}
		NonCopyable& operator=(const NonCopyable& other) {}
	};

	template <typename T>
	class MemHandle : public NonCopyable
	{
	private:
		T* _mem;
	public:
		MemHandle(T* mem)
			: _mem(mem) {}
		~MemHandle() { if (_mem) delete _mem; }
	};

	template <typename T>
	class ArrayHandle : public NonCopyable
	{
	private:
		T* _mem;
	public:
		ArrayHandle(T* mem)
			: _mem(mem) {}
		~ArrayHandle() { if (_mem) delete [] _mem; }
	};

	template <typename PTR>
	class RoundRobinPtr
	{
		int _size;
		int _capacity;
		int _current;
		int _prev;

		PTR* _buf;
	public:
		RoundRobinPtr(int capacity)
			: _capacity(capacity), _current(0), _size(0), _buf(0), _prev(0)
		{
			if (_capacity > 0)
				_buf = new PTR[_capacity];
		}

		~RoundRobinPtr()
		{
			if (_buf)
				delete [] _buf;
		}

		void add(const PTR& ptr) 
		{
			if (_size < _capacity) {
				_buf[_size++] = ptr;
			}
		}

		PTR& next()
		{
			_prev = _current;
			_current = (_current + 1) % _size;
			return _buf[_current];
		}

		PTR& getCurrent()
		{
			return _buf[_current];
		}

		PTR& getPrev()
		{
			return _buf[_prev];
		}
	};
}; // namespace Utils

template <typename T>
void Utils::printArray(T* a, int components, int count)
{
	for (int i = 0; i < count; i++) {
		std::cout << a[i * components];
		for (int j = 1; j < components; j++) {
			std::cout << ", " << a[i*components + j];
		}
		std::cout << std::endl;
	}
}