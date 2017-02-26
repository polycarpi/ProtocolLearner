#include <iostream>
#include <list>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <chrono>
#include <thread>

#include "KMeans.hpp"
#include "PacketAnalyser.hpp"
#include "CUdpPair.hpp"
#include "CUdpProtocolLearner.hpp"
#include "CUdpClient.hpp"
#include "CUdpReceiver.hpp"

#include <boost/thread.hpp>

using namespace std;

int main(int c, char * argv[])
{

    JsonConfigFileReader configFileReader(configFilePath);

	const std::uint16_t lLowServerPort = 10068;
	const std::uint16_t lDiodePortHighToLow = 10070;
	const std::uint16_t lDiodePortLowToHigh = 10071;		
	
	boost::asio::io_service lMainService;		
	
	CUdpPair UdpPair_HighToLow(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortHighToLow, lLowServerPort, lMainService);
	CUdpPair UdpPair_LowToHigh(std::string("127.0.0.1"), std::string("127.0.0.1"), lDiodePortLowToHigh, 0, lMainService);	
	
    const std::uint32_t lMaxPacketsToObserve{1000};
    const std::uint32_t lMaxTimeToObserve_s{3600};

	CUdpProtocolLearner UdpProtocolLearner(UdpPair_HighToLow, 
	                                       UdpPair_LowToHigh, 
	                                       lMaxPacketsToObserve, 
	                                       lMaxTimeToObserve_s);
	
	UdpProtocolLearner.mStartListening();	
	
	lMainService.run();
    	
    return 0;
}

