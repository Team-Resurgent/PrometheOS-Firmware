#include "xeniumManager.h"
#include "xenium.h"
#include "httpServer.h"

bool xeniumManager::uploadBios(std::vector<char> data, std::string name)
{
	if (data.size() != 256 * 1024)
	{
		return false;
	}
	

	//save to bank 4
	return true;
}

std::string xeniumManager::getBiosDetails()
{
	return "";
}

bool xeniumManager::bootbank(xenium::bankEnum bank)
{
	return true;
}