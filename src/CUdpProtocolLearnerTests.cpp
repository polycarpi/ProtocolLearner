#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE

#include <boost/thread.hpp>
#include <chrono>
#include <thread>
#include <algorithm>

#include "catch.hpp"
#include "KMeans.hpp"
#include "PacketAnalyser.hpp"
#include "CUdpPair.hpp"
#include "CUdpProtocolLearner.hpp"
#include "CUdpClient.hpp"
#include "CUdpReceiver.hpp"
#include "CUdpTestEchoServer.hpp"

static const std::uint32_t milliseccondSleepTimeForTermination = 10;


TEST_CASE("Test basic UDP protocol learner functionality, " 
          "namely that we can set up a protocol learner and inject packets")
{
	
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, 12001, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 12002, lMainService);

    const std::uint32_t lMaxPacketsToObserve{10};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 0);
	
	CUdpClient UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	UdpSender.mSend(lFrame);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
	std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));
	lMainService.stop();
	t.join();

	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 1);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 2);
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenLowToHigh() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenLowToHigh() == 0);	

}

TEST_CASE("Test that the protocol learner terminates after observing the max number of packets")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, 12000, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 12001, lMainService);

    const std::uint32_t lMaxPacketsToObserve{1};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 0);
	
	CUdpClient UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	UdpSender.mSend(lFrame);
	UdpSender.mSend(lFrame);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));	
	lMainService.stop();	
	t.join();
	
	
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenHighToLow() == 2);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenHighToLow() == 4);
	REQUIRE(UdpProtocolLearner.mGetPacketsSeenLowToHigh() == 0);
	REQUIRE(UdpProtocolLearner.mGetTotalBytesSeenLowToHigh() == 0);	
	
}


TEST_CASE("Test that the UDP receiver works", "[udpblast]")
{
	boost::asio::io_service lSenderService;	
	boost::asio::io_service lReceiverService;	
		
	CUdpClient UdpSender(std::string("127.0.0.1"), 10042, lSenderService);
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), 10042, lReceiverService);
	
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 0);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 0);
	
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C, 0x1A, 0x4C, 0x1A, 0x4C, 0x1A, 0x4C, 0x1A, 0x4C, 0x1A, 0x4C});
	
	const std::uint32_t lNumFramesToSend = 4810;
	
	UdpSender.mSend(lFrame);	
	boost::thread tSender(boost::bind(&boost::asio::io_service::run, &lSenderService));
	boost::thread tReceiver(boost::bind(&boost::asio::io_service::run, &lReceiverService));
	
	for(std::uint32_t l = 0; l < lNumFramesToSend - 1; ++l)
	{
		if(0 == l % 50)
		{
	        std::this_thread::sleep_for (std::chrono::microseconds(10));
		}
	    UdpSender.mSend(lFrame);
    }
    
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));
	lSenderService.stop();
	lReceiverService.stop();
	
	tSender.join();
	tReceiver.join();
		
	REQUIRE(UdpReceiver.mGetPacketsSeen() == lNumFramesToSend);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == lFrame.size() * lNumFramesToSend);
}


TEST_CASE("Test that the protocol learner passes the packets through to a receiver")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint16_t lFinalDestinationPort{10582};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, lFinalDestinationPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 0, lMainService);

    const std::uint32_t lMaxPacketsToObserve{10};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	
	CUdpClient UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), lFinalDestinationPort, lMainService);
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 0);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 0);
	
	UdpSender.mSend(lFrame);	
		
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
	
	for(uint32_t l = 1; l < lMaxPacketsToObserve; ++l)
	{
	    UdpSender.mSend(lFrame);
	}	
	
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));	
    lMainService.stop();
	t.join();
	
	REQUIRE(UdpReceiver.mGetPacketsSeen() == lMaxPacketsToObserve);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == lMaxPacketsToObserve * lFrame.size());	

}



TEST_CASE("Test that the protocol learner adds the seen packets to a learning engine")
{
	const std::uint16_t lHighReceiverPort{10146};
	const std::uint16_t lLowReceiverPort{10020};
	const std::uint16_t lFinalDestinationPort{8888};
	
	boost::asio::io_service lMainService;	
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lHighReceiverPort, lFinalDestinationPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowReceiverPort, 0, lMainService);

    const std::uint32_t lMaxPacketsToObserve{4};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	
	CUdpClient UdpSender(std::string("127.0.0.1"), lHighReceiverPort, lMainService);
	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	
	UdpProtocolLearner.mStartListening();
	
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), lFinalDestinationPort, lMainService);
	REQUIRE(UdpReceiver.mGetPacketsSeen() == 0);
	REQUIRE(UdpReceiver.mGetTotalBytesSeen() == 0);
	
	REQUIRE(UdpProtocolLearner.mGetNumberPacketsInLearningEngine() == 0);	
		
	for(uint32_t l = 0; l < lMaxPacketsToObserve; ++l)
	{
	    UdpSender.mSend(lFrame);
	}
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));	
    lMainService.stop();    
    t.join();
	
	REQUIRE(UdpReceiver.mGetPacketsSeen() == lMaxPacketsToObserve);	
	REQUIRE(UdpProtocolLearner.mGetNumberPacketsInLearningEngine() == lMaxPacketsToObserve);

}

TEST_CASE("Set up an echo-increment server and a UDP client. Check that the protocol learner "
           "fires the echo server callback")
{
	const std::uint16_t lLowServerPort = 10068;
	const std::uint16_t lDiodePortHighToLow = 10070;
	const std::uint16_t lDiodePortLowToHigh = 10071;
	const std::uint16_t lHighDestinationDummy = 10072;
	
	bool callBackWasFired = false;
	
	auto lEchoFunctionIncrementAllBytes = [&](const std::vector<std::uint8_t> inVec)
	{	
		callBackWasFired = true;
	};
	
	boost::asio::io_service lMainService;	
	
	CUdpTestEchoServer UdpTestServer(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowServerPort, lDiodePortLowToHigh, lMainService, lEchoFunctionIncrementAllBytes);
		
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortHighToLow, lLowServerPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortLowToHigh, lHighDestinationDummy, lMainService);	
	
    const std::uint32_t lMaxPacketsToObserve{1000};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	                                       
	UdpProtocolLearner.mStartListening();

	CUdpClient UdpSender(std::string("127.0.0.1"), lDiodePortHighToLow, lMainService);

	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});

	UdpSender.mSend(lFrame);
	
	REQUIRE(!callBackWasFired);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));	
    lMainService.stop();    
    t.join();
	
    REQUIRE(callBackWasFired);
}

TEST_CASE("Tests that the echo-increment server replies and that the protocol learner sees the reply")
{
	const std::uint16_t lLowServerPort = 10068;
	const std::uint16_t lDiodePortHighToLow = 10070;
	const std::uint16_t lDiodePortLowToHigh = 10071;
	const std::uint16_t lHighDestinationDummy = 10072;
	
	
	auto lEchoFunctionIncrementAllBytes = [&](std::vector<std::uint8_t>& inVec)
	{
		for (auto& i : inVec) ++i;
	};
	
	boost::asio::io_service lMainService;	
	
	CUdpTestEchoServer UdpTestServer(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowServerPort, lDiodePortLowToHigh, lMainService, lEchoFunctionIncrementAllBytes);
		
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortHighToLow, lLowServerPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortLowToHigh, lHighDestinationDummy, lMainService);	
	
    const std::uint32_t lMaxPacketsToObserve{1000};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	                                       
	UdpProtocolLearner.mStartListening();

	CUdpClient UdpSender(std::string("127.0.0.1"), lDiodePortHighToLow, lMainService);

	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});

	UdpSender.mSend(lFrame);
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));
    lMainService.stop();    
    t.join();
	
	REQUIRE(UdpProtocolLearner.mGetNumberPacketsInLearningEngine() == 2);
}

TEST_CASE("Test an entire round trip", "[roundtrip]")
{
	const std::uint16_t lLowServerPort = 10068;
	const std::uint16_t lDiodePortHighToLow = 10070;
	const std::uint16_t lDiodePortLowToHigh = 10071;	
	
	auto lEchoFunctionIncrementAllBytes = [&](std::vector<std::uint8_t>& inVec)
	{
		for (auto& i : inVec) ++i;
	};
	
	boost::asio::io_service lMainService;	
	
	CUdpTestEchoServer UdpTestServer(std::string("127.0.0.1"), std::string("127.0.0.1"), lLowServerPort, lDiodePortLowToHigh, lMainService, lEchoFunctionIncrementAllBytes);
		
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortHighToLow, lLowServerPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortLowToHigh, 0, lMainService);	
	
    const std::uint32_t lMaxPacketsToObserve{1000};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	                                       
	UdpProtocolLearner.mStartListening();

    std::uint32_t packetsReceived = 0;
    auto RoundTripCallback = [&](){++packetsReceived;};

	CUdpClient UdpClient(std::string("127.0.0.1"), lDiodePortHighToLow, lMainService);
	UdpClient.mSetFrameReceivedCallback(RoundTripCallback);

	const std::vector<std::uint8_t> lFrame({0x1A, 0x4C});

    REQUIRE(packetsReceived == 0);

    const std::uint32_t numPacketsToSend = 148;

    for(std::uint32_t lF = 0; lF < numPacketsToSend; ++lF)
    {
		UdpClient.mSend(lFrame);
    }
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));
    lMainService.stop();    
    t.join();

    REQUIRE(packetsReceived == numPacketsToSend);
	
}

TEST_CASE("Test that the UDP receiver clears buffer memory appropriately "
           "after receiving a longer frame")
{
	boost::asio::io_service lMainService;
	
	bool areSame = false;
	std::vector<std::uint8_t> aExpected;
	
	auto bytesExtractor = [&](const std::vector<std::uint8_t>& aIn)
	{
		areSame = std::equal(aIn.begin(), aIn.end(), aExpected.begin());
	};
		
	CUdpClient UdpSender(std::string("127.0.0.1"), 10042, lMainService);
	CUdpReceiver UdpReceiver(std::string("127.0.0.1"), 10042, lMainService);
	UdpReceiver.mSetFrameReceivedCallback(bytesExtractor);
	
	std::vector<std::uint8_t> lFrame({0x1A, 0x4C});
	// Send a short frame
	UdpSender.mSend(lFrame);	
	
	lFrame.clear();
	for(int i = 0; i < 24; ++i)
	{
	    lFrame.push_back(i);
    }
    // Now send a long frame
    
    UdpSender.mSend(lFrame);
    
    lFrame.clear();
	for(int i = 0; i < 3; ++i)
	{
	    lFrame.push_back(i);
    }    
    
    // Now send a short frame again
    UdpSender.mSend(lFrame);
    aExpected = lFrame;
	
	boost::thread t(boost::bind(&boost::asio::io_service::run, &lMainService));
    std::this_thread::sleep_for (std::chrono::milliseconds(milliseccondSleepTimeForTermination));
	lMainService.stop();
	t.join();
		
	REQUIRE(areSame);
}
