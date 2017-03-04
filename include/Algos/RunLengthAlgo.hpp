#ifndef RUNLENGTHALGO_H
#define RUNLENGTHALGO_H

#include "Algos/PacketAnalysisAlgo.hpp"

class RunLengthAlgo : public PacketAnalysisAlgo
{
public:
    void mAnalyse(const std::vector<std::uint8_t> aContents) override
    {
		for(auto c : aContents)
		{
			m_Decoded.push_back(mClassify(c));
		}
	}

	const std::vector<float> mGetAnalysis()
	{
		std::vector<std::pair<eCharType, float> > lRunLengths;
		for(auto c : m_Decoded)
		{
			bool lSet = false;
			if(!lRunLengths.empty())
			{
				if(lRunLengths.back().first == c)
				{
					++lRunLengths.back().second;
					lSet = true;
				}
		    }
			if(!lSet)
			{
				lRunLengths.emplace_back(c, 1.0f);
			}
		}
		
		std::vector<float> lTypelessRunLengths;
		
		std::transform(
		  lRunLengths.begin(), 
		  lRunLengths.end(),
		  std::back_inserter(lTypelessRunLengths),
		  [](std::pair<eCharType, float> aPair){return aPair.second;});
		
		return lTypelessRunLengths;
	}
private:


    std::vector<eCharType> m_Decoded;
};






#endif
