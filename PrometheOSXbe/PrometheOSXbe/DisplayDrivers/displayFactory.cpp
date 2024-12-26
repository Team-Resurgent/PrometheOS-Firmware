#include "displayFactory.h"

displayDriver* displayFactory::getDisplay(displayVariant variant)
{
	if (variant == displayVariantLCDXXXXLPC)
	{
		return displayLCDXXXXLPC::getInstance();
	}
	else if (variant == displayVariantLCDXXXXModxo)
	{
		return displayLCDXXXXModxo::getInstance();
	}
	else if (variant == displayVariantHD44780LPC)
	{
		return displayHD44780LPC::getInstance();
	}
	else if (variant == displayVariantHD44780Modxo)
	{
		return displayHD44780Modxo::getInstance();
	}
	else if (variant == displayVariantSPI2PARModxo)
	{
		return displaySPI2PARModxo::getInstance();
	}
	else if (variant == displayVariantHD44780Aladdin)
	{
		return displayHD44780Aladdin::getInstance();
	}
	else if (variant == displayVariantSPI2PARAladdin)
	{
		return displaySPI2PARAladdin::getInstance();
	}
	else if (variant == displayVariantSPI2PARSmartxx)
	{
		return displaySPI2PARSmartxx::getInstance();
	}
	else if (variant == displayVariantHD44780Xecuter)
	{
		return displayHD44780Xecuter::getInstance();
	}
	else if (variant == displayVariantSPI2PARXenium)
	{
		return displaySPI2PARXenium::getInstance();
	}
	else if (variant == displayVariantSPI2PARXtremium)
	{
		return displaySPI2PARXtremium::getInstance();
	}
	return NULL;
}