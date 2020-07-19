#pragma once
#include "MWPheader.h"
#include "MWPinput.h"
#include "MWPgameInfo.h"
#include "MWPdraw.h"

struct RegistrationScreenAttributes
{
	struct rgb backgroundColor;
	struct rgb verticalLines;
};

void initRegistrationAttributes(RegistrationScreenAttributes * RSAttributes);
void RegistrationScreen(KeyPresses * key);