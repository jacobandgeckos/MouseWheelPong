#include "MWPinput.h"

void ProcessEvents()
{
	MSG event;
	while (PeekMessage(&event, 0, 0, 0, PM_REMOVE))
	{
		switch (event.message)
		{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			unsigned int VKCode = (unsigned int)event.wParam;
			int WasDown = (event.lParam & (1 << 30)) != 0;
			int IsDown = (event.lParam & (1 << 31)) == 0;
			if (WasDown != IsDown)
			{

				if (VKCode == 'W')
				{

				}
				else if (VKCode == 'S')
				{

				}
				else if (VKCode == 'A')
				{

				}
				else if (VKCode == 'D')
				{

				}
				else if (VKCode == 'Q')
				{
					
				}
				else if (VKCode == 'E')
				{
					
				}
				else if (VKCode == VK_UP)
				{
				}
				else if (VKCode == VK_DOWN)
				{

				}
				else if (VKCode == VK_RIGHT)
				{

				}
				else if (VKCode == VK_LEFT)
				{

				}
				else if (VKCode == VK_ESCAPE)
				{
					running = false;
				}
				else if (VKCode == VK_SPACE)
				{

				}
			}
			int AltKeyWasDown = (event.lParam & (1 << 29));
			if (VKCode == VK_F4 && AltKeyWasDown)
			{
				running = false;
			}
			break;
		}
		default:
		{
			TranslateMessage(&event);
			DispatchMessage(&event);
		}
		}
	}
}