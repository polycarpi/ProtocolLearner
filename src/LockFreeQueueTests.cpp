#include "catch.hpp"
#include "LockFreeQueue.hpp"

TEST_CASE("Basic lock free queue functionality")
{
	LockFreeQueue<std::uint64_t> lQueue(10);
	
	REQUIRE(lQueue.mGetCurrentContentsSize() == 0);
	
	lQueue.push(0);
	
	REQUIRE(lQueue.mGetCurrentContentsSize() == 1);
	
    REQUIRE(lQueue.pop() == 0);
    
    REQUIRE(lQueue.mGetCurrentContentsSize() == 0);

}

TEST_CASE("Test that we can fill the queue with  one extra")
{
	LockFreeQueue<std::uint64_t> lQueue(10);
	
	for(int i = 0; i < 11; ++i)	
	{
    	lQueue.push(i);
    }
	
	REQUIRE(lQueue.mGetCurrentContentsSize() == 10);
	
	REQUIRE(lQueue.pop() == 1);
}

TEST_CASE("Test that we can fill the queue many times over")
{
	LockFreeQueue<std::uint64_t> lQueue(10);
	
	for(int i = 0; i < 105; ++i)	
	{
    	lQueue.push(i);
    }
	
	REQUIRE(lQueue.mGetCurrentContentsSize() == 10);
	
	REQUIRE(lQueue.pop() == 95);
}

TEST_CASE("Test that calls to pop decrease the current contents size")
{
	LockFreeQueue<std::uint64_t> lQueue(10);
	
	for(int i = 0; i < 105; ++i)	
	{
    	lQueue.push(i);
    }

	REQUIRE(lQueue.mGetCurrentContentsSize() == 10);

    lQueue.pop();
    REQUIRE(lQueue.mGetCurrentContentsSize() == 9);
    REQUIRE(lQueue.pop() == 96);
    REQUIRE(lQueue.pop() == 97);
    REQUIRE(lQueue.pop() == 98);
    REQUIRE(lQueue.pop() == 99);
    REQUIRE(lQueue.pop() == 100);
    REQUIRE(lQueue.pop() == 101);
    REQUIRE(lQueue.pop() == 102);
    REQUIRE(lQueue.pop() == 103);
    REQUIRE(lQueue.pop() == 104);
    
    
    REQUIRE(lQueue.mGetCurrentContentsSize() == 0);
    REQUIRE_THROWS_AS(lQueue.pop(), std::runtime_error);
}
