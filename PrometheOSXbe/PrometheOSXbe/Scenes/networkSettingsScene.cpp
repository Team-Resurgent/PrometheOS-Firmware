#include "networkSettingsScene.h"
#include "sceneManager.h"
#include "removeScene.h"
#include "flashFlowScene.h"
#include "editFlowScene.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "keypadScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\network.h"

networkSettingsScene::networkSettingsScene()
{
	mSelectedControl = 0;
	mKeypadScene = NULL;
	mNetworkMode = settingsManager::getNetworkMode();
	mAddress = settingsManager::getNetworkAddress();
	mSubnet = settingsManager::getNetworkSubnet();
	mGateway = settingsManager::getNetworkGateway();
	mPrimaryDns = settingsManager::getNetworkPrimaryDns();
	mSecondaryDns = settingsManager::getNetworkSecondaryDns();
	mRefreshNetwork = mNetworkMode != networkModeStatic;

	if (mNetworkMode == networkModeStatic)
	{
		if (mAddress == 0)
		{
			mAddress = network::getAddress();
		}
		if (mSubnet == 0)
		{
			mSubnet = network::getSubnet();
		}
		if (mGateway == 0)
		{
			mGateway = network::getGateway();
		}
	}

	if (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns)
	{
		if (mPrimaryDns == 0)
		{
			mPrimaryDns = network::getPrimaryDns();
		}
		if (mSecondaryDns == 0)
		{
			mSecondaryDns = network::getSecondaryDns();
		}
	}

}

networkSettingsScene::~networkSettingsScene()
{
	delete(mKeypadScene);
}

void networkSettingsScene::update()
{
	if (mRefreshNetwork == true && network::isReady())
	{
		if (mNetworkMode != networkModeStatic)
		{
			mAddress = network::getAddress();
			mSubnet = network::getSubnet();
			mGateway = network::getGateway();
		}
		if (mNetworkMode == networkModeDynamicAutoDns)
		{
			mPrimaryDns = network::getPrimaryDns();
			mSecondaryDns = network::getSecondaryDns();
		}
		mRefreshNetwork = false;
	}

	if (mKeypadScene != NULL)
	{
		mKeypadScene->update();
		if (mKeypadScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mKeypadScene);
			mKeypadScene = NULL;
			return;
		}
		else if (mKeypadScene->getSceneResult() == sceneResultDone)
		{
			if (mSelectedControl == 1)
			{
				mAddress = mKeypadScene->getValue();
			}
			else if (mSelectedControl == 2)
			{
				mSubnet = mKeypadScene->getValue();
			}
			else if (mSelectedControl == 3)
			{
				mGateway = mKeypadScene->getValue();
			}
			else if (mSelectedControl == 4)
			{
				mPrimaryDns = mKeypadScene->getValue();
			}
			else if (mSelectedControl == 5)
			{
				mSecondaryDns = mKeypadScene->getValue();
			}
			delete(mKeypadScene);
			mKeypadScene = NULL;
		}
		return;
	}

	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(sceneItemSystemSettingsScene);
	}

	// Apply Action

	if (hasNetworkChanges() && inputManager::buttonPressed(ButtonY))
	{
		settingsManager::setNetwork(mNetworkMode, mAddress, mSubnet, mGateway, mPrimaryDns, mSecondaryDns);
		network::restart();
		mRefreshNetwork = true;
	}

	// Select Action

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mSelectedControl == 0)
		{
			if (mNetworkMode == networkModeDynamicAutoDns)
			{
				mNetworkMode = networkModeDynamicManualDns;
			}
			else if (mNetworkMode == networkModeDynamicManualDns)
			{
				mNetworkMode = networkModeStatic;
			}
			else if (mNetworkMode == networkModeStatic)
			{
				mNetworkMode = networkModeDynamicAutoDns;
			}
		}
		else if (mSelectedControl == 1 && mNetworkMode == networkModeStatic)
		{
			mKeypadScene = new keypadScene("Please enter IP...", mAddress);
		}
		else if (mSelectedControl == 2 && mNetworkMode == networkModeStatic)
		{
			mKeypadScene = new keypadScene("Please enter Subnet...", mSubnet);
		}
		else if (mSelectedControl == 3 && mNetworkMode == networkModeStatic)
		{
			mKeypadScene = new keypadScene("Please enter Gateway...", mGateway);
		}
		else if (mSelectedControl == 4 && (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns))
		{
			mKeypadScene = new keypadScene("Please enter Primary DNS...", mPrimaryDns);
		}
		else if (mSelectedControl == 5 && (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns))
		{
			mKeypadScene = new keypadScene("Please enter Secondary DNS...", mSecondaryDns);
		}
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 5; 
	}
}

void networkSettingsScene::render()
{
	if (mKeypadScene != NULL)
	{
		mKeypadScene->render();
		return;
	}

	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Network Settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	uint32_t yPos = (context::getBufferHeight() - ((6 * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	if (mNetworkMode == networkModeDynamicAutoDns)
	{
		component::button(mSelectedControl == 0, false, "Mode: Dynamic IP (Automatic DNS)", 193, yPos, 322, 30);
	}
	else if (mNetworkMode == networkModeDynamicManualDns)
	{
		component::button(mSelectedControl == 0, false, "Mode: Dynamic IP (Manual DNS)", 193, yPos, 322, 30);
	}
	else if (mNetworkMode == networkModeStatic)
	{
		component::button(mSelectedControl == 0, false, "Mode: Static IP", 193, yPos, 322, 30);
	}

	yPos += 40;
	
	char* ip = stringUtility::formatIp(mAddress);
	char* ipString = stringUtility::formatString("IP Address: %s", ip);
	if (mNetworkMode == networkModeStatic)
	{
		component::button(mSelectedControl == 1, false, ipString, 193, yPos, 322, 30);
	}
	else
	{
		component::textBox(ipString, mSelectedControl == 1, false, horizAlignmentCenter, 193, yPos, 322, 30);
	}
	free(ipString);
	free(ip);

	yPos += 40;

	char* subnet = stringUtility::formatIp(mSubnet);
	char* subnetString = stringUtility::formatString("Subnet Mask: %s", subnet);
	if (mNetworkMode == networkModeStatic)
	{
		component::button(mSelectedControl == 2, false, subnetString, 193, yPos, 322, 30);
	}
	else
	{
		component::textBox(subnetString, mSelectedControl == 2, false, horizAlignmentCenter, 193, yPos, 322, 30);
	}
	free(subnetString);
	free(subnet);

	yPos += 40;

	char* gateway = stringUtility::formatIp(mGateway);
	char* gatewayString = stringUtility::formatString("Gateway: %s", gateway);
	if (mNetworkMode == networkModeStatic)
	{
		component::button(mSelectedControl == 3, false, gatewayString, 193, yPos, 322, 30);
	}
	else
	{
		component::textBox(gatewayString, mSelectedControl == 3, false, horizAlignmentCenter, 193, yPos, 322, 30);
	}
	free(gatewayString);
	free(gateway);

	yPos += 40;

	char* primaryDns = stringUtility::formatIp(mPrimaryDns);
	char* primaryDnsString = stringUtility::formatString("Primary DNS: %s", primaryDns);
	if (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns)
	{
		component::button(mSelectedControl == 4, false, primaryDnsString, 193, yPos, 322, 30);
	}
	else
	{
		component::textBox(primaryDnsString, mSelectedControl == 4, false, horizAlignmentCenter, 193, yPos, 322, 30);
	}
	free(primaryDnsString);
	free(primaryDns);

	yPos += 40;
	
	char* secondaryDns = stringUtility::formatIp(mSecondaryDns);
	char* secondaryDnsString = stringUtility::formatString("Secondary DNS: %s", secondaryDns);
	if (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns)
	{
		component::button(mSelectedControl == 5, false, secondaryDnsString, 193, yPos, 322, 30);
	}
	else
	{
		component::textBox(secondaryDnsString, mSelectedControl == 5, false, horizAlignmentCenter, 193, yPos, 322, 30);
	}
	free(secondaryDnsString);
	free(secondaryDns);

	drawing::drawBitmapString(context::getBitmapFontSmall(), hasNetworkChanges() ? "\xC2\xA1 Select \xC2\xA4 Apply" : "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

bool networkSettingsScene::hasNetworkChanges()
{
	bool result = mNetworkMode != settingsManager::getNetworkMode();
	if (mNetworkMode == networkModeStatic)
	{
		result |=
			(mAddress != settingsManager::getNetworkAddress()) ||
			(mSubnet != settingsManager::getNetworkSubnet()) ||
			(mGateway != settingsManager::getNetworkGateway());
	}
	if (mNetworkMode == networkModeStatic || mNetworkMode == networkModeDynamicManualDns)
	{
		result |=
			(mPrimaryDns != settingsManager::getNetworkPrimaryDns()) ||
			(mSecondaryDns != settingsManager::getNetworkSecondaryDns());
	}
	return result;
}
