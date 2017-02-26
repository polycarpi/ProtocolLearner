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
	}

	const std::vector<std::pair<eRunLengthCategories, std::uint32_t> > mGetAnalysis()
	{
		return std::vector<std::pair<eRunLengthCategories, std::uint32_t> >({});
	}
};






#endif
