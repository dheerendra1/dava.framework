/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Base/BaseTypes.h"

#if defined(__DAVAENGINE_MACOS__)

#include "CorePlatformMacOS.h"
#include <ApplicationServices/ApplicationServices.h>

namespace DAVA 
{
	
	// some macros to make code more readable.
#define GetModeWidth(mode) GetDictionaryLong((mode), kCGDisplayWidth)
#define GetModeHeight(mode) GetDictionaryLong((mode), kCGDisplayHeight)
#define GetModeRefreshRate(mode) GetDictionaryLong((mode), kCGDisplayRefreshRate)
#define GetModeBitsPerPixel(mode) GetDictionaryLong((mode), kCGDisplayBitsPerPixel)
	
//------------------------------------------------------------------------------------------
long GetDictionaryLong(CFDictionaryRef theDict, const void* key) 
{
	// get a long from the dictionary
	long value = 0;
	CFNumberRef numRef;
	numRef = (CFNumberRef)CFDictionaryGetValue(theDict, key); 
	if (numRef != NULL)
		CFNumberGetValue(numRef, kCFNumberLongType, &value); 	
	return value;
}
	
DisplayMode CoreMacOSPlatform::GetCurrentDisplayMode()
{
	CFDictionaryRef currentMode = CGDisplayCurrentMode(kCGDirectMainDisplay);
	
	// look at each mode in the available list
	//CFDictionaryRef modeSystem = (CFDictionaryRef)CFArrayGetValueAtIndex(currentMode, mode);

	DisplayMode mode;
	mode.width = GetModeWidth(currentMode);
	mode.height = GetModeHeight(currentMode);
	mode.refreshRate = GetModeRefreshRate(currentMode);
	mode.bpp = GetModeBitsPerPixel(currentMode);
	
	return mode;
}
	
void CoreMacOSPlatform::GetAvailableDisplayModes(List<DisplayMode> & availableModes)
{
	CFArrayRef availableModesSystem = CGDisplayAvailableModes(kCGDirectMainDisplay);
	int32 numberOfAvailableModes = CFArrayGetCount(availableModesSystem);
	
	for (int modeIndex = 0; modeIndex < numberOfAvailableModes; ++modeIndex)
	{
		// look at each mode in the available list
		CFDictionaryRef modeSystem = (CFDictionaryRef)CFArrayGetValueAtIndex(availableModesSystem, modeIndex);
	
		DisplayMode mode;
		mode.width = GetModeWidth(modeSystem);
		mode.height = GetModeHeight(modeSystem);
		mode.refreshRate = GetModeRefreshRate(modeSystem);
		mode.bpp = GetModeBitsPerPixel(modeSystem);
		availableModes.push_back(mode);
	}
	
//		LPDIRECT3D9 direct3D = RenderManager::Instance()->GetD3D();
//		availableDisplayModes.clear();
//		
//		D3DFORMAT formats[] = {D3DFMT_R5G6B5, D3DFMT_X8R8G8B8}; 
//		
//		for (int format = 0; format < sizeof(formats) / sizeof(D3DFORMAT); ++format)
//		{
//			for (int32 mode = 0; mode < direct3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, formats[format]); ++mode)
//			{
//				D3DDISPLAYMODE displayMode;
//				HRESULT hr = direct3D->EnumAdapterModes(D3DADAPTER_DEFAULT, formats[format], mode, &displayMode);	
//				if (!FAILED(hr))
//				{
//					DisplayMode mode;
//					mode.width = displayMode.Width;
//					mode.height = displayMode.Height;
//					if (displayMode.Format == D3DFMT_R5G6B5)mode.bpp = 16;
//					else if (displayMode.Format == D3DFMT_X8R8G8B8) mode.bpp = 32;
//					else if (displayMode.Format == D3DFMT_R8G8B8) mode.bpp = 24;
//					mode.refreshRate = displayMode.RefreshRate;
//					availableDisplayModes.push_back(mode);
//					
//					Logger::Debug("[RenderManagerDX9::GetAvailableDisplayModes] mode found: %d x %d x %d", 
//								  mode.width,
//								  mode.height,
//								  mode.bpp);
//				}
//			}
//		}
	
	
}

	
}


#endif // #if defined(__DAVAENGINE_MACOS__)
