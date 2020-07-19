#pragma once
#include "MWPheader.h"
#include "MWPinput.h"
#include "MWPgameInfo.h"
#include "MWPdraw.h"
#include "MWPregistration.h"
#include "MWPgame.h"

enum MWP_States { MWP_MAIN_MENU, MWP_REGISTRATION, MWP_GAME, MWP_PAUSE};

char ExecuteMWPStateMachine(KeyPresses * key);