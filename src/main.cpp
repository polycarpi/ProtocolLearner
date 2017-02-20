#define CATCH_CONFIG_MAIN

#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>
#include <thread>

#include "catch.hpp"
#include "KMeans.hpp"
#include "PacketAnalyser.hpp"
#include "CUdpPair.hpp"
#include "CUdpProtocolLearner.hpp"
#include "CUdpSender.hpp"
#include "CUdpReceiver.hpp"

#include <boost/thread.hpp>

using namespace std;	

TEST_CASE("Dummy")
{
    PacketAnalyser analyser({'a'});
    analyser.mAnalysePacket();
    auto resultsVec = analyser.mGetAnalysis();
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

TEST_CASE("Test basic UDP protocol learner functionality, namely that we can set up a protocol learner and inject packets")
{
	
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint32_t lBufferSize{1024*1024};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, 12001, lBufferSize, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 12002, lBufferSize, lMainService);

    const std::uint32_t lMaxPacketsToObserve{10};
    const std::uint32_t lMaxTimeToObserve_s{3600};

    std::atomic<bool> * lStopFlag = new std::atomic<bool>;
	*lStopFlag = true;
	
	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s,
	                                       lStopFlag);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 0);
	
	CUdpSender UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	UdpSender.mSend(lFrame);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
	t.join();

	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 1);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 2);
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenLowToHigh() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenLowToHigh() == 0);	

    delete lStopFlag;
	
}


TEST_CASE("Test that the protocol learner terminates after observing the max number of packets")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint32_t lBufferSize{1024*1024};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, 12000, lBufferSize, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 12001, lBufferSize, lMainService);

    const std::uint32_t lMaxPacketsToObserve{1};
    const std::uint32_t lMaxTimeToObserve_s{3600};

    std::atomic<bool> * lStopFlag = new std::atomic<bool>;
	*lStopFlag = false;
	
	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s,
	                                       lStopFlag);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 0);
	
	CUdpSender UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	UdpSender.mSend(lFrame);
	UdpSender.mSend(lFrame);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
	t.join();
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 2);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 4);
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenLowToHigh() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenLowToHigh() == 0);	

	delete lStopFlag;
	
}


TEST_CASE("Test that the protocol learner passes the packets through to a receiver")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint16_t lFinalDestinationPort{8888};
	const std::uint32_t lBufferSize{1024*1024};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, lFinalDestinationPort, lBufferSize, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 0, lBufferSize, lMainService);

    const std::uint32_t lMaxPacketsToObserve{10};
    const std::uint32_t lMaxTimeToObserve_s{3600};

    std::atomic<bool> * lStopFlag = new std::atomic<bool>;
	*lStopFlag = true;

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s,
	                                       lStopFlag);
	
	CUdpSender UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), lFinalDestinationPort, lMainService);
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 0);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 0);
	
	for(uint32_t l = 0; l < lMaxPacketsToObserve; ++l)
	{
	    UdpSender.mSend(lFrame);
	}
		
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
	*lStopFlag = true;
	t.join();
	
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 1);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 8);	
	
	delete lStopFlag;

}

TEST_CASE("Test that the protocol learner adds the seen packets to a learning engine")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint16_t lFinalDestinationPort{8888};
	const std::uint32_t lBufferSize{1024*1024};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, lFinalDestinationPort, lBufferSize, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 0, lBufferSize, lMainService);

    const std::uint32_t lMaxPacketsToObserve{4};
    const std::uint32_t lMaxTimeToObserve_s{3600};

    std::atomic<bool> * lStopFlag = new std::atomic<bool>;
	*lStopFlag = true;

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s,
	                                       lStopFlag);
	
	CUdpSender UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), lFinalDestinationPort, lMainService);
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 0);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 0);
	
	for(uint32_t l = 0; l <= lMaxPacketsToObserve; ++l)
	{
	    UdpSender.mSend(lFrame);
	}
	lMainService.run();
	
	REQUIRE(UdpProtocolLearner.mGetNumberPacketsInLearningEngine() == 4);

}

/*
int BinaryMain(int c, char * argv[])
{
  return 0;
}
*/
