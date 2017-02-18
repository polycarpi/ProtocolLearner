#include "KMeans.hpp"

template <class T>
void KMeans<T>::mUpdateMeans()
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

template<class T>
void KMeans<T>::mSubmit(const std::vector<T>& inVector)
{
    m_PointVector.emplace_back(inVector, m_PointIndex++);
}

template<class T>
const DataPoint<T>& KMeans<T>::mExtract(const uint16_t aIndex) const
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

template<class T>
const MeanPoint<T>& KMeans<T>::mExtractMean(const uint16_t aMeanLabel) const
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
  
template<class T>
const T KMeans<T>::mCalculateEuclideanDistance(const DataPoint<T> aPoint, const MeanPoint<T> aMeanPoint)
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
  
template<class T>  
const std::uint16_t KMeans<T>::mFindNearestMean(const DataPoint<T>& aPoint)
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

template<class T> 
void KMeans<T>::mAssignAll()
{	
    for(auto& i : m_PointVector)
    {
		auto oldID = i.m_MeanIndex;
		std::uint16_t lNearestMeanID = mFindNearestMean(i);
		i.m_MeanIndex = lNearestMeanID;
		i.m_EuclideanDistanceFromAssignedMean = mCalculateEuclideanDistance(i, m_CurrentMeans.at(i.m_MeanIndex));
	}
}
  
template<class T>
void KMeans<T>::mInitialise(const std::uint16_t aNumMeans)
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
  
template<class T>  
const std::vector<DataPoint<T> > KMeans<T>::mGetCurrentMeans()
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

  
template<class T> 
const uint16_t KMeans<T>::mGetNumPoints() const
{
    return m_PointVector.size();
}
    
template<class T>
void KMeans<T>::mRunKMeans(const uint16_t aNumMeans, const uint16_t aNumIterations)
{
    std::vector<std::pair<std::vector<T>, uint16_t> > means;
    std::vector<uint16_t> randomIndices;
        
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint16_t> dist(0, m_PointVector.size() - 1);

    while(randomIndices.size() < aNumMeans)
    {
        uint16_t sample = dist(generator);
        if(std::find(randomIndices.begin(), randomIndices.end(), sample) == randomIndices.end())
        {
            randomIndices.push_back(sample);
        }
    }
}


template<class T>
std::ostream& operator<<(std::ostream& os, const KMeans<T>& ca)
{
    std::cerr << "Error: not implemented" << std::endl;
    return os;
}

