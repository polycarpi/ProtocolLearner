#define CATCH_CONFIG_MAIN
#include "../TapTapd/include/catch.hpp"

#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>

using namespace std;

class PacketAnalyser
{
public:
	PacketAnalyser(const std::vector<uint16_t>& inVector) : vec{inVector}, m_PacketSize{0}, m_Symbolics{0}, m_Numeric{0}, m_Character{0}
	{		
	};
	void mAnalysePacket()
	{
	    for(auto i : vec)
	    {
    	        if(i < 32)
    	        {
		    // Control
		    m_Control += 1.0f;
		}
		else if(i < 48)
		{
		    // Symbolic
		    m_Symbolics += 1.0f;
		}
		else if(i < 58)
		{
		    // Numeric
		    m_Numeric += 1.0f;
		}
		else
		{
		    // Character
		    m_Character += 1.0f;
		}
            }
            m_PacketSize = static_cast<float>(vec.size());
            m_Symbolics /= static_cast<float>(vec.size());
            m_Numeric /= static_cast<float>(vec.size());
            m_Character /= static_cast<float>(vec.size());
	};

    
        const std::vector<float> getAnalysis()
        {
            return std::vector<float>({m_PacketSize, m_Symbolics, m_Numeric, m_Character});
        }
private:
    const std::vector<uint16_t> vec;
    float m_PacketSize;
    float m_Symbolics;
    float m_Numeric;
    float m_Character;
    float m_Control;
};


template <typename T>
class DataPoint
{
public:
    DataPoint(const std::vector<T>& aVector, const uint16_t aIndex) : 
        m_Vector{aVector}, 
        m_Index{aIndex}, 
        m_EuclideanDistanceFromAssignedMean{0.0},
        m_MeanIndex{0}
    {

	}
    uint16_t m_MeanIndex;
    T m_EuclideanDistanceFromAssignedMean;
    std::vector<T> m_Vector;
    uint16_t m_Index;
};

template <typename T>
class MeanPoint
{
public:
    MeanPoint(const std::uint16_t aLabel, const std::vector<T>& aVector) : 
        m_Label{aLabel}, 
        m_Vector{aVector}{}
    std::uint16_t m_Label;
    std::vector<T> m_Vector;
};

template <typename T>
class KMeans
{
public:
    KMeans(const uint16_t numArgsIn) : m_NumArgs{numArgsIn}, m_PointIndex{0} {}
    
    void mUpdateMeans()
    {
		for(auto& lMeanPoint : m_CurrentMeans)
		{
			std::vector<T> lAverageVector(m_CurrentNumMeans, 0.0);
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
			std::copy(lAverageVector.begin(), lAverageVector.end(), lMeanPoint.m_Vector.begin());
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
    
    T mCalculateEuclideanDistance(const DataPoint<T> aPoint, const MeanPoint<T> aMeanPoint)
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
	    //For each id in the main list, assign to the nearest mean.
	    for(auto& i : m_PointVector)
	    {
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
    
    void mRunKMeans(const uint16_t aNumMeans, const uint16_t aNumIterations)
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
private:
    std::vector<DataPoint<T> > m_PointVector;
    const uint16_t m_NumArgs;
    uint16_t m_CurrentNumMeans;  
    std::vector<MeanPoint<T> > m_CurrentMeans;
    std::uint16_t m_PointIndex;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const KMeans<T>& ca)
{

    return os;
}

void packetHandler(u_char * userData, const struct pcap_pkthdr * pkthdr, const u_char * packet)
{
    // Update the learning engine
    std::cout << pkthdr->ts.tv_sec << "." << pkthdr->ts.tv_usec << std::endl;
    uint length = pkthdr->len;
    std::vector<uint16_t> lByteVec;
    for(uint lIt = 0; lIt < length; ++lIt)
    {
    	lByteVec.push_back(static_cast<uint16_t>(packet[lIt]));
    }
    PacketAnalyser analyser(lByteVec);
    analyser.mAnalysePacket();
}

TEST_CASE("Dummy")
{
    PacketAnalyser analyser({'a'});
    analyser.mAnalysePacket();
    auto resultsVec = analyser.getAnalysis();
    REQUIRE(1.0f == resultsVec.at(0));
    REQUIRE(0.0f == resultsVec.at(1));
    REQUIRE(0.0f == resultsVec.at(2));
    REQUIRE(1.0f == resultsVec.at(3));
}

TEST_CASE("Generate 2 packets and initialise. Check that the initial means are unchanged")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 2;
    KMeans<float> packetLearner(numArgs);

    std::vector<float> tempPacket;
    tempPacket.push_back(1.0f);
    tempPacket.push_back(0.0f);
    packetLearner.mSubmit(tempPacket);
    
    tempPacket.clear();
    tempPacket.push_back(0.0f);
    tempPacket.push_back(1.0f);
    packetLearner.mSubmit(tempPacket);    
    
    packetLearner.mInitialise(numMeans);

    const std::vector<DataPoint<float> > & extractedMeans = packetLearner.mGetCurrentMeans();
        
    if(1.0f == extractedMeans.at(0).m_Vector.at(0))
    {
		REQUIRE(0.0f == extractedMeans.at(0).m_Vector.at(1));
		REQUIRE(0.0f == extractedMeans.at(1).m_Vector.at(0));
		REQUIRE(1.0f == extractedMeans.at(1).m_Vector.at(1));	
	}
	else
	{
		REQUIRE(0.0f == extractedMeans.at(0).m_Vector.at(0));
		REQUIRE(1.0f == extractedMeans.at(0).m_Vector.at(1));
		REQUIRE(1.0f == extractedMeans.at(1).m_Vector.at(0));
		REQUIRE(0.0f == extractedMeans.at(1).m_Vector.at(1));
	}

}

TEST_CASE("Generate 2 packets and initialise. Add a third packet and \
run the assignment. Check that the initial means don't change")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 2;
    KMeans<float> packetLearner(numArgs);

    std::vector<float> tempPacket;
    tempPacket.push_back(1.0f);
    tempPacket.push_back(0.0f);
    packetLearner.mSubmit(tempPacket);
    
    tempPacket.clear();
    tempPacket.push_back(0.0f);
    tempPacket.push_back(1.0f);
    packetLearner.mSubmit(tempPacket);    
    
    packetLearner.mInitialise(numMeans);

    tempPacket.clear();
    tempPacket.push_back(0.75f);
    tempPacket.push_back(0.25f);
    packetLearner.mSubmit(tempPacket);      
	
	packetLearner.mAssignAll();
	
	// Check that the initial means are unchanged
    const std::vector<DataPoint<float> > & extractedMeans = packetLearner.mGetCurrentMeans();
    if(1.0f == extractedMeans.at(0).m_Vector.at(0))
    {
		REQUIRE(0.0f == extractedMeans.at(0).m_Vector.at(1));
		REQUIRE(0.0f == extractedMeans.at(1).m_Vector.at(0));
		REQUIRE(1.0f == extractedMeans.at(1).m_Vector.at(1));
		
	}
	else
	{
		REQUIRE(0.0f == extractedMeans.at(0).m_Vector.at(0));
		REQUIRE(1.0f == extractedMeans.at(0).m_Vector.at(1));
		REQUIRE(1.0f == extractedMeans.at(1).m_Vector.at(0));
		REQUIRE(0.0f == extractedMeans.at(1).m_Vector.at(1));
	}	
	
}



TEST_CASE("Generate 2 packets and initialise. Add a third packet and \
run the assignment. Check that the third packet has been assigned to \
the (1.0, 0.0) cluster")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 2;
    KMeans<float> packetLearner(numArgs);

    std::vector<float> tempPacket;
    tempPacket.push_back(1.0f);
    tempPacket.push_back(0.0f);
    packetLearner.mSubmit(tempPacket);
    
    tempPacket.clear();
    tempPacket.push_back(0.0f);
    tempPacket.push_back(1.0f);
    packetLearner.mSubmit(tempPacket);    
    
    packetLearner.mInitialise(numMeans);

    tempPacket.clear();
    tempPacket.push_back(0.75f);
    tempPacket.push_back(0.25f);
    packetLearner.mSubmit(tempPacket);      
	
	packetLearner.mAssignAll();
	
	// Extract the packet with ID
	const auto extractedPacket = packetLearner.mExtract(2);
	REQUIRE(extractedPacket.m_EuclideanDistanceFromAssignedMean == sqrt(0.125f));
    const auto lExtractedMean = packetLearner.mExtractMean(extractedPacket.m_MeanIndex);
    REQUIRE(1.0f == lExtractedMean.m_Vector.at(0));
    REQUIRE(0.0f == lExtractedMean.m_Vector.at(1));
	
}

TEST_CASE("Generate 2 packets and initialise. Add a third packet and \
run the assignment followed by the update. Verify that the first mean \
is unchanged but that the second mean is the mid-point of the two")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 2;
    KMeans<float> packetLearner(numArgs);

    std::vector<float> tempPacket;
    tempPacket.push_back(1.0f);
    tempPacket.push_back(0.0f);
    packetLearner.mSubmit(tempPacket);
    
    tempPacket.clear();
    tempPacket.push_back(0.0f);
    tempPacket.push_back(1.0f);
    packetLearner.mSubmit(tempPacket);    
    
    packetLearner.mInitialise(numMeans);

    tempPacket.clear();
    tempPacket.push_back(0.75f);
    tempPacket.push_back(0.25f);
    packetLearner.mSubmit(tempPacket);      
	
	packetLearner.mAssignAll();
	
	packetLearner.mUpdateMeans();
	
	const auto extractedLastPacket = packetLearner.mExtract(2);
	const std::uint16_t lMeanOfInterest = extractedLastPacket.m_MeanIndex;
    const auto lExtractedMean = packetLearner.mExtractMean(lMeanOfInterest);
    std::cout << "(" << lExtractedMean.m_Vector.at(0) << ", " << lExtractedMean.m_Vector.at(1) << ")" << std::endl;
    REQUIRE(0.875 == lExtractedMean.m_Vector.at(0));
    REQUIRE(0.125 == lExtractedMean.m_Vector.at(1));
    
}


int BinaryMain(int c, char * argv[])
{
	
  std::string deviceString;
  pcap_t * descr;
  char errorBuffer[PCAP_ERRBUF_SIZE];

  pcap_if_t * device;
  
  if(-1 == pcap_findalldevs(&device, errorBuffer))
  {
	  std::cerr << "Unable to open any devices" << std::endl;
  }
  
  while(device)
  {
	  if(0 == std::string(argv[1]).compare(std::string(device->name)))
	  {
		  deviceString = std::string(device->name);
		  std::cout << "Got " << deviceString << std::endl;
		  break;
	  }
      device = device->next;
  }
  
  descr = pcap_open_live(deviceString.c_str(), BUFSIZ, 0, -1, errorBuffer);
  if (descr == NULL)
  {
      cout << "pcap_open_live() failed: " << errorBuffer << endl;
      return 1;
  }

  if (pcap_loop(descr, 0, packetHandler, NULL) < 0)
  {
      cout << "pcap_loop() failed: " << pcap_geterr(descr);
      return 1;
  }
  


  return 0;
}
