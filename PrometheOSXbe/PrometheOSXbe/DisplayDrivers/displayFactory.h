#pragma once

#include "displayDriver.h"
#include "displayLCDXXXXLPC.h"
#include "displayLCDXXXXModxo.h"
#include "displayHD44780LPC.h"
#include "displayHD44780Modxo.h"
#include "displaySPI2PARModxo.h"
#include "displayPythiaModxo.h"
#include "displayHD44780Aladdin.h"
#include "displaySPI2PARAladdin.h"
#include "displayHD44780Smartxx.h"
#include "displaySPI2PARSmartxx.h"
#include "displayHD44780Xecuter.h"
#include "displaySPI2PARXenium.h"

enum displayVariant {
    displayVariantLCDXXXXLPC,
    displayVariantLCDXXXXModxo,
    displayVariantHD44780LPC,
    displayVariantHD44780Modxo,
    displayVariantSPI2PARModxo,
    displayVariantPythiaModxo,
    displayVariantHD44780Aladdin,
    displayVariantSPI2PARAladdin,
    displayVariantHD44780Smartxx,
    displayVariantSPI2PARSmartxx,
    displayVariantHD44780Xecuter,
    displayVariantSPI2PARXenium,
};

class displayFactory {
  public:
    static displayDriver* getDisplay(displayVariant variant);
};
