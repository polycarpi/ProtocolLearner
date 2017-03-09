#ifndef LOCKFREEQUEUE_HPP
#define LOCKFREEQUEUE_HPP

#include <cstdint>
#include <vector>
#include <iostream>
#include <stdexcept>

template <class T>
class LockFreeQueue
{
public:
    LockFreeQueue(std::uint64_t aInitialSize) :
    m_UnderlyingReservedSize(aInitialSize),
    m_Back(-1),
    m_Front(0),
    m_CurrentSize(0)
    {
	    m_Underlying.reserve(m_UnderlyingReservedSize);
	}
	std::uint64_t mGetCurrentContentsSize()
	{
        return m_CurrentSize;
	}
	
	void push(const T& input)
	{
		m_Back = (m_Back + 1) % m_UnderlyingReservedSize;		
		if(m_Underlying.size() < m_UnderlyingReservedSize)
		{
			m_Underlying.push_back(input);
			++m_CurrentSize;
		}
		else
		{
			m_Underlying.at(m_Back) = input;
		}
		if(m_Front == m_Back && m_Underlying.size() == m_UnderlyingReservedSize)
		{
			m_Front = (m_Front + 1) % m_UnderlyingReservedSize;
		}
	}
	
	T pop()
	{
		if(m_CurrentSize == 0)
		{
			throw std::runtime_error("Empty queue");
		}
		T lReturnValue = m_Underlying.at(m_Front);
		m_Front = (m_Front + 1) % m_UnderlyingReservedSize;
		--m_CurrentSize;
		return lReturnValue;
	}
private:
    std::uint64_t m_UnderlyingReservedSize;
    std::vector<T> m_Underlying;
    std::int64_t m_Back;
    std::int64_t m_Front;
    std::int64_t m_CurrentSize;
};

#endif
