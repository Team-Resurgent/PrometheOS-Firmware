#pragma once

#include "xenium.h"

#include <string>
#include <vector>

class xeniumManager
{
public:

	//int bank, int bank count

	static bool uploadBios(std::vector<char> data, std::string name);
	static std::string getBiosDetails();
	static bool bootbank(xenium::bankEnum bank);
};
