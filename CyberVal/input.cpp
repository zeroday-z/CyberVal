#include "input.h"
#include <stdio.h>

Input input;

void Input::Init(HWND lp)
{
	if (SUCCEEDED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpDIObject, NULL)))
	{
		this->lpDIObject->CreateDevice(GUID_SysMouse, &lpDIMouseDevice, NULL);
		this->lpDIMouseDevice->SetDataFormat(&c_dfDIMouse2);
		this->lpDIMouseDevice->SetCooperativeLevel(lp, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		this->lpDIMouseDevice->Acquire();
	}
}

void Input::Update()
{
	
	if (lpDIMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &MouseState) == DIERR_INPUTLOST)
	{
		this->lpDIMouseDevice->Acquire();
	}
}