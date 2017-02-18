#define CATCH_CONFIG_MAIN

#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>

#include "catch.hpp"
#include "KMeans.hpp"
#include "PacketAnalyser.hpp"
#include "CUdpPair.hpp"
#include "CUdpProtocolLearner.hpp"
#include "CUdpSender.hpp"



using namespace std;

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
	
	const std::uint16_t lHighReceiverPort{4567};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint32_t lBufferSize{1024*1024};
	
	CUdpPair UdpPair_HighToLow(std::string("localhost"), std::string("localhost"), lHighReceiverPort, lBufferSize);
	CUdpPair UdpPair_LowToHigh(std::string("localhost"), std::string("localhost"), lLowReceiverPort, lBufferSize);

    const std::uint32_t lMaxPacketsToObserve{10};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, UdpPair_LowToHigh, lMaxPacketsToObserve, lMaxTimeToObserve_s);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeen() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeen() == 0);
	
	CUdpSender UdpSender(std::string("localhost"), lHighReceiverPort);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	UdpSender.mSend(lFrame);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeen() == 1);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeen() == 2);	
	
}
/*
TEST_CASE("Test that the UDP protocol learner sends through packets unchanged")
{
	bool lPacketReceivedOnLow = false;
	CUdpServerFake lFakeUdpReceiver("localhost", lLowPort, auto [&](){lPacketReceivedOnLow = true});
	
	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, UdpPair_LowToHigh, lMaxPacketsToObserve, lMaxTimeToObserve);
		
	REQUIRE(!lPacketReceivedOnLow);
		
	CUdpSender UdpSender("localhost", lHighPort);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	UdpSender.mSend(lFrame);
	
	REQUIRE(lPacketReceivedOnLow);
	
	const auto lPacketReceived = lFakeUdpReceiver.mPopFrame();
	REQUIRE(lPacketReceived == lFrame);
	
}
*/
/*
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
*/
