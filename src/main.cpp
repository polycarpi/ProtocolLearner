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

/*
int BinaryMain(int c, char * argv[])
{
  return 0;
}
*/
