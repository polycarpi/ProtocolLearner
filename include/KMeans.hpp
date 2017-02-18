#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>
#include <algorithm>

template <class T>
struct DataPoint
{
    DataPoint(const std::vector<T>& aVector, const uint16_t aIndex) : 
        m_MeanIndex{0},
        m_EuclideanDistanceFromAssignedMean{0.0},        
        m_Vector{aVector}, 
        m_Index{aIndex}
    {

	}
    uint16_t m_MeanIndex;
    T m_EuclideanDistanceFromAssignedMean;
    std::vector<T> m_Vector;
    uint16_t m_Index;

};

template <class T>
struct MeanPoint
{
    MeanPoint(const std::uint16_t aLabel, const std::vector<T>& aVector) : 
        m_Label{aLabel}, 
        m_Vector{aVector}{}
    std::uint16_t m_Label;
    std::vector<T> m_Vector;
};

template <class T>
class KMeans
{

public:
    KMeans(const uint16_t numArgsIn) : m_NumArgs{numArgsIn}, m_PointIndex{0} {}
    
    void mUpdateMeans()
	{
	    for(auto& lMeanPoint : m_CurrentMeans)
		{
			std::vector<T> lAverageVector(m_NumArgs, 0.0);
			typename std::vector<DataPoint<T> >::iterator lPointIterator = m_PointVector.begin();
			std::uint16_t lNumberInGroup = 0;
			while(lPointIterator != m_PointVector.end())
			{
				lPointIterator = std::find_if(lPointIterator, 
				    m_PointVector.end(), 
				    [&](const DataPoint<T>& aPoint){return aPoint.m_MeanIndex == lMeanPoint.m_Label;});
				
				if(lPointIterator != m_PointVector.end())
				{
				    for(typename std::vector<T>::iterator lIt = lAverageVector.begin();
				        lIt != lAverageVector.end();
				        ++lIt)
				    {
						*lIt += (*lPointIterator).m_Vector.at(std::distance(lAverageVector.begin(), lIt));
				    }
					++lPointIterator;
	                   ++lNumberInGroup;
				}
			}
			for(auto& lElement : lAverageVector)
			{
				lElement /= static_cast<T>(lNumberInGroup);
			}
			lMeanPoint.m_Vector = lAverageVector;
		}
	}

    void mSubmit(const std::vector<T>& inVector)
    {
        m_PointVector.emplace_back(inVector, m_PointIndex++);
    }
    
    const DataPoint<T>& mExtract(const uint16_t aIndex) const
	{
	    auto it = std::find_if(m_PointVector.begin(), 
		                       m_PointVector.end(), 
		                       [&](const DataPoint<T>& a)
		                       {
								   return aIndex == a.m_Index;
							   });
							   
		if(it == m_PointVector.end())
		{
			throw new std::exception;
		}
		return *it;
	}    
    
    
    
    
    const MeanPoint<T>& mExtractMean(const uint16_t aMeanLabel) const
	{		
		auto it = std::find_if(m_CurrentMeans.begin(), 
		                       m_CurrentMeans.end(), 
		                       [&](const MeanPoint<T>& a)
		                       {
								   return aMeanLabel == a.m_Label;
							   });
							   
		if(it == m_CurrentMeans.end())
		{
			throw new std::exception;
		}
		return *it;
	}    
    
    const T mCalculateEuclideanDistance(const DataPoint<T> aPoint, const MeanPoint<T> aMeanPoint)
	{
		T lAcc = 0.0;
		for(typename std::vector<T>::const_iterator lIt = aPoint.m_Vector.begin(); 
		    lIt != aPoint.m_Vector.end(); 
		    ++lIt)
		{
			lAcc += pow(*lIt - aMeanPoint.m_Vector.at(std::distance(aPoint.m_Vector.begin(), lIt)), 2.0);
		}
		return sqrt(lAcc);
	}    
    
    const std::uint16_t mFindNearestMean(const DataPoint<T>& aPoint)
	{
		typename std::vector<MeanPoint<T> >::iterator lMeansIterator = m_CurrentMeans.begin();
		
		T lCurrentMinEuclidean = mCalculateEuclideanDistance(aPoint, *lMeansIterator);
		
		std::uint16_t lCurrentClosestMeanIndex = m_CurrentMeans.at(0).m_Label;
		
		++lMeansIterator;
		
		for(; lMeansIterator != m_CurrentMeans.end(); ++lMeansIterator)
		{
			T lTempEuclidean = mCalculateEuclideanDistance(aPoint, *lMeansIterator);
			if(lTempEuclidean < lCurrentMinEuclidean)
			{
				lCurrentMinEuclidean = lTempEuclidean;
	            lCurrentClosestMeanIndex = lMeansIterator->m_Label;
			}
		}
		return lCurrentClosestMeanIndex;
	}    
    
    void mAssignAll()
	{	
	    for(auto& i : m_PointVector)
	    {
			auto oldID = i.m_MeanIndex;
			std::uint16_t lNearestMeanID = mFindNearestMean(i);
			i.m_MeanIndex = lNearestMeanID;
			i.m_EuclideanDistanceFromAssignedMean = mCalculateEuclideanDistance(i, m_CurrentMeans.at(i.m_MeanIndex));
		}
	}    

    void mInitialise(const std::uint16_t aNumMeans)
	{
		m_CurrentNumMeans = aNumMeans;
		if(m_PointVector.size() < aNumMeans)
		{
			throw new std::exception;
		}
	    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	    std::shuffle (m_PointVector.begin(), m_PointVector.end(), std::default_random_engine(seed));
	    for(uint16_t lMean = 0; lMean < aNumMeans; ++lMean)
	    {
	    	m_CurrentMeans.emplace_back(lMean, m_PointVector.at(lMean).m_Vector);
		}
	}    

    const std::vector<DataPoint<T> > mGetCurrentMeans()
	{
		std::vector<DataPoint<T> > lCurrentMeans;
		
		for(uint16_t lIDIndex = 0; lIDIndex < m_CurrentNumMeans; ++lIDIndex)
		{   
			auto j = std::find_if(m_PointVector.begin(),
			                   m_PointVector.end(),
			                   [&](const DataPoint<T>& a){return lIDIndex == a.m_Index;});
			                   	
			if(j != m_PointVector.end())
			{
				lCurrentMeans.push_back(*j);
			}
		}
		return lCurrentMeans;
	}    
    
    const uint16_t mGetNumPoints() const
	{
	    return m_PointVector.size();
	}    
    
    friend std::ostream& operator<<(std::ostream& os, const KMeans<float>& ca);

private:
    std::vector<DataPoint<T> > m_PointVector;
    const uint16_t m_NumArgs;
    uint16_t m_CurrentNumMeans;  
    std::vector<MeanPoint<T> > m_CurrentMeans;
    std::uint16_t m_PointIndex;
};
#endif
