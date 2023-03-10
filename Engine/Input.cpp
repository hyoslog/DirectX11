#include "Input.h"

void CInput::Initialize()
{
	for (bool& ref_key : Keys)
	{
		ref_key = false;
	}
}

void  CInput::KeyDown(unsigned int InInput)
{
	Keys[InInput] = true;
}

void  CInput::KeyUp(unsigned int InInput)
{
	Keys[InInput] = false;
}

bool CInput::IsKeyDown(unsigned int Inkey)
{
	return Keys[Inkey];
}