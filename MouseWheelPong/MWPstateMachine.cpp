#include "MWPstateMachine.h"

extern Winfo globalWindow;
extern gameInfo GlobalGameInfo;

extern MWP_States MWP_State;

char ExecuteMWPStateMachine(KeyPresses * key)
{
	
	switch (MWP_State) 
	{   // Transitions
		case MWP_REGISTRATION:
			{
				if (key->accept)
				{
					MWP_State = MWP_GAME;
				}
				else if (key->quit)
				{
					return 1;
				}
			}
			break;
		case MWP_GAME:
			if (key->quit)
			{
				return 1;
			}
			break;
		case MWP_PAUSE:
			break;
		default:
			break;
	}
	switch (MWP_State)
	{   // State actions
		case MWP_REGISTRATION:
			{
				RegistrationScreen(key);
			}
			break;
		case MWP_GAME:
			{
				Game(key);
			}
			break;
		case MWP_PAUSE:
			break;
		default:
			break;
	} 

	
	return 0;
}