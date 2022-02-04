#pragma once
#include "pch.h"

const int STATE_BLACKFOREST = 0;




class StateClass
{
public:
	StateClass();
	StateClass(const StateClass&);
	~StateClass();

	int GetCurrentState();

private:
	int m_currentState;
};