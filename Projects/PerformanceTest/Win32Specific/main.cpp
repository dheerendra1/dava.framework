#include "DAVAEngine.h"

int APIENTRY WinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	return DAVA::Core::Run(0, 0, hInstance);
}

/*int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
{
	// find leaks mega-string: {,,msvcr71d.dll}_crtBreakAlloc
	//
	// POSSIBLE LEAKS LIST:
	// remember -- always clear all allocated data for static STL containers
	// 
	return 0;
}*/