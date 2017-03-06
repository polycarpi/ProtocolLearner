#ifndef CHARDISTRIBUTIONALGO_H
#define CHARDISTRIBUTIONALGO_H

#include <iostream>

#include "Algos/PacketAnalysisAlgo.hpp"

class CharDistributionAlgo : public PacketAnalysisAlgo
{
public:
    void mAnalyse(const std::vector<std::uint8_t> aContents) override
    {
		for(auto c : aContents)
		{
			m_Decoded.push_back(mClassify(c));
		}
	}

	const std::vector<float> mGetAnalysis() override
	{
		std::vector<float> lCharTypeDistribution;
        for(std::uint32_t lType = 0; lType < NUMTYPES; ++lType)
        {
			lCharTypeDistribution.push_back(0.0f);
		}
		
		for(auto c : m_Decoded)
		{
			++lCharTypeDistribution.at(c);
		}
		
		return lCharTypeDistribution;
	}
private:
    std::vector<eCharType> m_Decoded;
};






#endif
