#ifndef __keyboard_h__
#define __keyboard_h__

#pragma once

#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// ASCII mapping VK_CODE
#define VK_KEY_L	0x4C
#define VK_KEY_W	0x57

#endif // __keyboard_h__