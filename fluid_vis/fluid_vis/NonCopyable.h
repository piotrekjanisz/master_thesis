#pragma once

/**
 * Mixin class preventing derived classes objects from beeing copied
 */
class NonCopyable 
{
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable& other) {}
	NonCopyable& operator=(const NonCopyable& other) {}
};