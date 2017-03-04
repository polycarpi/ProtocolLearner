#ifndef PACKETANALYSER_H
#define PACKETANALYSER_H

#include <vector>
#include <list>
#include <memory>
#include <cstdint>

#include "Algos/PacketAnalysisAlgo.hpp"

class PacketAnalyser
{
public:

	PacketAnalyser(const std::vector<uint8_t>& inVector, const std::shared_ptr<PacketAnalysisAlgo>& aAlgoToRun) : 
	  m_Vec{inVector}, 
	  m_AlgoToRun(aAlgoToRun),
	  m_PacketSize{0}, 
	  m_Symbolics{0}, 
	  m_Numeric{0}, 
	  m_Character{0}
	{		
	};	
	
	void mAnalysePacket();
    
	const std::vector<float> mGetAnalysis()
	{
		return m_AlgoToRun->mGetAnalysis();
	}
private:
    const std::vector<uint8_t> m_Vec;
    std::shared_ptr<PacketAnalysisAlgo> m_AlgoToRun;
    float m_PacketSize;
    float m_Symbolics;
    float m_Numeric;
    float m_Character;
    float m_Control;
};

#endif
