#include "MWPregistration.h"

extern Winfo globalWindow;
extern gameInfo GlobalGameInfo;

extern RegistrationScreenAttributes regScreen;

void initRegistrationAttributes(RegistrationScreenAttributes * RSAttributes)
{
	RSAttributes->backgroundColor.red = 255;
	RSAttributes->backgroundColor.green = 0;
	RSAttributes->backgroundColor.blue = 0;


	RSAttributes->verticalLines.red = 255;
	RSAttributes->verticalLines.green = 255;
	RSAttributes->verticalLines.blue = 0;
}

void RegistrationScreen(KeyPresses * key)
{
	if (key->device != NULL)
	{
		MWPdevice& device = GlobalGameInfo.deviceList[key->device];
		if (key->leftButtonUp)
			device.mouse.leftButtonPressed = false;
		else if (key->leftButtonDown)
			device.mouse.leftButtonPressed = true;
		else if (key->rightButtonUp)
			device.mouse.rightButtonPressed = false;
		else if (key->rightButtonDown)
			device.mouse.rightButtonPressed = true;

		if (device.mouse.rightButtonPressed && device.mouse.leftButtonPressed && device.assignedPlayer > MAX_PLAYERS && GlobalGameInfo.playerCount < MAX_PLAYERS)
		{
			registerDevice(GlobalGameInfo.deviceList, GlobalGameInfo.playerCount, key->device);
			Player player = { 0 };
			player.device = key->device;
			GlobalGameInfo.players[GlobalGameInfo.playerCount] = player;
			++GlobalGameInfo.playerCount;
			background(&globalWindow, regScreen.backgroundColor);
			drawSpreadVerticalLines(&globalWindow, GlobalGameInfo.playerCount - 1, regScreen.verticalLines);
			drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0,0,0));

		}

		if (device.assignedPlayer < MAX_PLAYERS)
		{
			if (key->scrolledUp)
			{
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, regScreen.backgroundColor);
				if ((GlobalGameInfo.players[device.assignedPlayer].position - 30 + (globalWindow.winHeight / 2)) > 0)
					GlobalGameInfo.players[device.assignedPlayer].position -= 30;
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
			}
			else if (key->scrolledDown)
			{
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, regScreen.backgroundColor);
				if (globalWindow.winHeight > (GlobalGameInfo.players[device.assignedPlayer].position + 30 + GlobalGameInfo.paddleHeight + (globalWindow.winHeight / 2)))
					GlobalGameInfo.players[device.assignedPlayer].position += 30;
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
			}
		}

	}
}