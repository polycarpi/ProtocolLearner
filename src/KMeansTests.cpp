#include "catch.hpp"
#include "PacketAnalyser.hpp"
#include "CUdpProtocolLearner.hpp"
#include "Algos/CharDistributionAlgo.hpp"


TEST_CASE("Dummy")
{
    PacketAnalyser analyser({'a'}, std::make_shared<CharDistributionAlgo>());
    analyser.mAnalysePacket();
    
    auto resultsVec = analyser.mGetAnalysis();
    
    REQUIRE(1.0f == resultsVec.at(CHAR));
    REQUIRE(0.0f == resultsVec.at(WHITESPACE));
    REQUIRE(0.0f == resultsVec.at(NUMERAL));
    REQUIRE(0.0f == resultsVec.at(PUNCTUATION));
    REQUIRE(0.0f == resultsVec.at(CONTROL));
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
	REQUIRE(abs(extractedPacket.m_EuclideanDistanceFromAssignedMean - sqrt(0.125f)) < 0.001);
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
    REQUIRE(0.875 == lExtractedMean.m_Vector.at(0));
    REQUIRE(0.125 == lExtractedMean.m_Vector.at(1));
    
}


TEST_CASE("Generate 1 packet and initialise. Add a second packet and \
run the assignment followed by the update. Verify that the mean is \
the mean of the two packets")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 1;
    KMeans<float> packetLearner(numArgs);

    std::vector<float> tempPacket;
    tempPacket.push_back(0.25f);
    tempPacket.push_back(0.75f);
    packetLearner.mSubmit(tempPacket);
    
    packetLearner.mInitialise(numMeans);    
    
    tempPacket.clear();
    tempPacket.push_back(0.75f);
    tempPacket.push_back(0.25f);
    packetLearner.mSubmit(tempPacket);        
	
	packetLearner.mAssignAll();
	packetLearner.mUpdateMeans();
	
    const auto lExtractedMean = packetLearner.mExtractMean(0);
    REQUIRE(0.5 == lExtractedMean.m_Vector.at(0));
    REQUIRE(0.5 == lExtractedMean.m_Vector.at(1));
    
    tempPacket.clear();
    tempPacket.push_back(0.125f);
    tempPacket.push_back(0.5f);
    packetLearner.mSubmit(tempPacket);  
    
	packetLearner.mAssignAll();
	packetLearner.mUpdateMeans();      
	    
    const auto lAnotherExtractedMean = packetLearner.mExtractMean(0);
    REQUIRE(0.375 == lAnotherExtractedMean.m_Vector.at(0));
    REQUIRE(0.5 == lAnotherExtractedMean.m_Vector.at(1));	    
    
}

TEST_CASE("Generate 1000 packets and run clustering")
{
	const uint16_t numArgs = 2;
	const uint16_t numMeans = 4;
	const uint16_t numPoints = 1000;
	const uint32_t numClusteringRuns = 1000;
	
    KMeans<float> packetLearner(numArgs);
    
    const float_t corners[] = {0.2, 0.3, 0.7, 0.8};

    std::vector<float> tempPacket;
    unsigned seed1 = 42;  
    std::default_random_engine generator(seed1);
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    
    for(uint16_t lSample = 0; lSample < numPoints; ++lSample)
    {
		tempPacket.clear();
		for(uint16_t lArg = 0; lArg < numArgs; ++lArg)
		{
		    tempPacket.push_back(distribution(generator));
	    }
        packetLearner.mSubmit(tempPacket);
	}
		
    packetLearner.mInitialise(numMeans);
      
    for(uint32_t i = 0; i < numClusteringRuns; ++i)
    {
	    packetLearner.mAssignAll();	
	    packetLearner.mUpdateMeans();
    }
    
	const auto extractedLastPacket = packetLearner.mExtract(0);
	const std::uint16_t lMeanOfInterest = extractedLastPacket.m_MeanIndex;
    const auto lExtractedMean = packetLearner.mExtractMean(lMeanOfInterest);
    
    // Ensure that the extracted mean falls into one of the four 'corners'
    if((corners[3] >= lExtractedMean.m_Vector.at(0)) && (corners[2] <= lExtractedMean.m_Vector.at(0)) && 
       (corners[3] >= lExtractedMean.m_Vector.at(1)) && (corners[2] <= lExtractedMean.m_Vector.at(1)))
    {
	}
	else if((corners[3] >= lExtractedMean.m_Vector.at(0)) && (corners[2] <= lExtractedMean.m_Vector.at(0)) && 
       (corners[1] >= lExtractedMean.m_Vector.at(1)) && (corners[0] <= lExtractedMean.m_Vector.at(1)))
    {
	}
	else if((corners[1] >= lExtractedMean.m_Vector.at(0)) && (corners[0] <= lExtractedMean.m_Vector.at(0)) && 
       (corners[3] >= lExtractedMean.m_Vector.at(1)) && (corners[2] <= lExtractedMean.m_Vector.at(1)))
    {
	}
	else if((corners[1] >= lExtractedMean.m_Vector.at(0)) && (corners[0] <= lExtractedMean.m_Vector.at(0)) && 
       (corners[1] >= lExtractedMean.m_Vector.at(1)) && (corners[0] <= lExtractedMean.m_Vector.at(1)))
    {
	}
	else
	{
		REQUIRE(1 == 0);
	}
}
