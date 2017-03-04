#include <list>
#include <vector>
#include <iostream>

#include "catch.hpp"
#include "PacketAnalyser.hpp"
#include "Algos/RunLengthAlgo.hpp"

TEST_CASE("Basic test for the run-length analyser")
{
	const std::vector<std::uint8_t> lPacketContents({'R', 'F', 'B',
		 ' ', '0', '0', '3', '.', '0', '0', '8', '\n'});
	
    RunLengthAlgo lRunLengthAlgo;
	
	lRunLengthAlgo.mAnalyse(lPacketContents);
	std::vector<float> lResults = lRunLengthAlgo.mGetAnalysis();
		
	const std::vector<float> lExpectedResults = 
	  {3.0f, 1.0f, 3.0f, 1.0f, 3.0f, 1.0f};

	REQUIRE(lResults == lExpectedResults);
	
	SECTION("Basic test for the run-length analyser - calling from PacketAnalyser")
	{
		PacketAnalyser lAnalyser(lPacketContents, std::make_shared<RunLengthAlgo>());
		lAnalyser.mAnalysePacket();
		
		auto lResults = lAnalyser.mGetAnalysis();
		
		REQUIRE(lResults == lExpectedResults);
	}
}

