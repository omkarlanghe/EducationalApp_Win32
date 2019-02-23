#include<windows.h>
#include "ExplicitDllForPhysics.h"

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return(TRUE);
}

extern "C" float MagnitudeOfForce(float gravity, float mass1, float mass2, float distance)
{
	float fg = gravity*(mass1*mass2) / (distance*distance);
	return(fg);
}

