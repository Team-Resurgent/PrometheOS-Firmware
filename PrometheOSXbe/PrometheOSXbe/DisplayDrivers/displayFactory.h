#pragma once

#include "displayDriver.h"
#include "displayLCDXXXXLPC.h"
#include "displayLCDXXXXModxo.h"
#include "displayHD44780LPC.h"
#include "displayHD44780Modxo.h"
#include "displaySPI2PARModxo.h"
#include "displayHD44780Aladdin.h"
#include "displaySPI2PARAladdin.h"
#include "displaySPI2PARSmartxx.h"
#include "displayHD44780Xecuter.h"
#include "displaySPI2PARXenium.h"
#include "displaySPI2PARXtremium.h"

enum displayVariant
{ 
	displayVariantLCDXXXXLPC,
	displayVariantLCDXXXXModxo,
	displayVariantHD44780LPC,
	displayVariantHD44780Modxo,
	displayVariantSPI2PARModxo,
	displayVariantHD44780Aladdin,
	displayVariantSPI2PARAladdin,
	displayVariantSPI2PARSmartxx,
	displayVariantHD44780Xecuter,
	displayVariantSPI2PARXenium,
	displayVariantSPI2PARXtremium,
}; 

class displayFactory
{
public:
	static displayDriver* getDisplay(displayVariant variant);
};