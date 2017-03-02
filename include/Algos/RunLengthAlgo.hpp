#ifndef RUNLENGTHALGO_H
#define RUNLENGTHALGO_H

#include "Algos/PacketAnalysisAlgo.hpp"

typedef enum eRunLengthCategories_tag
{
	CHAR = 0,
	WHITESPACE = 1,
	NUMERAL = 2,
	PUNCTUATION = 3,
	CONTROL = 4,
	NUMTYPES = 5
} eRunLengthCategories;


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

	const std::vector<std::pair<eRunLengthCategories, std::uint32_t> > mGetAnalysis()
	{
		std::vector<std::pair<eRunLengthCategories, std::uint32_t> > lRunLengths;
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
				lRunLengths.emplace_back(c, 1);
			}
		}
		return lRunLengths;
	}
private:
    eRunLengthCategories mClassify(const std::uint8_t aChar)
    {
		eRunLengthCategories lRet;
		if(aChar < 32)
		{
			lRet = CONTROL;
		}
		else if(aChar  < 33)
		{
			lRet = WHITESPACE;
		}
		else if(aChar < 48)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 58)
		{
			lRet = NUMERAL;
		}
		else if(aChar < 65)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 91)
		{
			lRet = CHAR;
		}
		else if(aChar < 97)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 123)
		{
			lRet = CHAR;
		}
		else if(aChar < 127)
		{
			lRet = PUNCTUATION;
		}
		else
		{
			lRet = CONTROL;
		}
		return lRet;
	}

    std::vector<eRunLengthCategories> m_Decoded;
};






#endif
