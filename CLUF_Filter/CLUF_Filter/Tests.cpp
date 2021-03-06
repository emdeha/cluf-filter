#include "Tests.h"

const char *TEST_FILE_NAME = "test.txt";


//////////////////////////
//  Filter Chain tests  //
//////////////////////////

bool Test_FilterChainCreation(const std::string &inputFile, const std::string &outputFile)
{
	FilterChain testChain(inputFile, outputFile);
	return testChain.inputFile.is_open() && testChain.outputFile.is_open();
}

bool Test_FilterChainAddFilter(Filter *filter, int filterID)
{
	FilterChain testChain(TEST_FILE_NAME, TEST_FILE_NAME);
	testChain.AddFilter(filter, filterID);	

	const auto &addedFilters = testChain.GetFilters();
	auto foundFilter = addedFilters.find(filterID);

	return foundFilter != addedFilters.end();
}

bool Test_FilterChainRemoveFilter(FilterChain *chain, int index)
{
	chain->RemoveFilter(index);

	const auto &chainFilters = chain->GetFilters();
	auto foundFilter = chainFilters.find(index);

	return foundFilter == chainFilters.end();
}

bool Test_FilterChainSerialization(const FilterChain &chain, const std::string &fileName)
{
	chain.Serialize(fileName);

	FilterChain newChain;
	newChain.Deserialize(fileName);

	if (chain.inputFileName == newChain.inputFileName &&
		chain.outputFileName == newChain.outputFileName &&
		chain.filters == newChain.filters)
	{
		return true;
	}
	return false;
}

bool Test_FilterChainProcessThroughFilters(FilterChain &chain, const std::string &expectedOutput)
{
	chain.ProcessThroughFilters();
	std::ifstream filteredFile(chain.outputFileName);
	std::stringstream fileStream;
	fileStream << filteredFile.rdbuf(); 
	std::string stringedStream = fileStream.str();
	filteredFile.close();
	return stringedStream == expectedOutput;
}