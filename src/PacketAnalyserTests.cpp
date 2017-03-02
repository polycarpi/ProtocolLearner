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
	std::vector<std::pair<eRunLengthCategories, std::uint32_t> > lResults = lRunLengthAlgo.mGetAnalysis();
	
	const std::vector<std::pair<eRunLengthCategories, std::uint32_t> > lExpectedResults = 
	  {
		  {eRunLengthCategories::CHAR, 3}, 
		  {eRunLengthCategories::WHITESPACE, 1}, 
		  {eRunLengthCategories::NUMERAL, 3}, 
		  {eRunLengthCategories::PUNCTUATION, 1}, 
		  {eRunLengthCategories::NUMERAL, 3}, 
		  {eRunLengthCategories::CONTROL, 1}
    };

	REQUIRE(lResults == lExpectedResults);
}


TEST_CASE("Basic test for the run-length analyser - calling from PacketAnalyser")
{
	const std::vector<std::uint8_t> lPacketContents({'R', 'F', 'B',
		 ' ', '0', '0', '3', '.', '0', '0', '8', '\n'});
	
	//std::list<PacketAnalysisAlgo*> lAlgoList;
	//lAlgoList.push_back(new RunLengthAlgo());
	
	PacketAnalyser lAnalyser(lPacketContents);//, lAlgoList);
	lAnalyser.mAnalysePacket();
	
	auto lResults = lAnalyser.mGetAnalysis();
	
	
	//REQUIRE(2 == 3);
}

