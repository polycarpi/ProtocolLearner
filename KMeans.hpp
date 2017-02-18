#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>
#include <algorithm>


template <class T>
class DataPoint
{
public:
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
class MeanPoint
{
public:
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
    void mUpdateMeans();
    void mSubmit(const std::vector<T>& inVector);
    const DataPoint<T>& mExtract(const uint16_t aIndex) const;
    const MeanPoint<T>& mExtractMean(const uint16_t aMeanLabel) const;
    const T mCalculateEuclideanDistance(const DataPoint<T> aPoint, const MeanPoint<T> aMeanPoint);
    const std::uint16_t mFindNearestMean(const DataPoint<T>& aPoint);
    void mAssignAll();
    void mInitialise(const std::uint16_t aNumMeans);
    const std::vector<DataPoint<T> > mGetCurrentMeans();
    const uint16_t mGetNumPoints() const;
    friend std::ostream& operator<<(std::ostream& os, const KMeans<float>& ca);
    void mRunKMeans(const uint16_t aNumMeans, const uint16_t aNumIterations);
private:
    std::vector<DataPoint<T> > m_PointVector;
    const uint16_t m_NumArgs;
    uint16_t m_CurrentNumMeans;  
    std::vector<MeanPoint<T> > m_CurrentMeans;
    std::uint16_t m_PointIndex;

};
