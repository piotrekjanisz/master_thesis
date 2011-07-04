#pragma once

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