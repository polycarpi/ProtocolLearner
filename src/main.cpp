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
	std::cout << "Binary main" << std::endl;
	return 0;
}

