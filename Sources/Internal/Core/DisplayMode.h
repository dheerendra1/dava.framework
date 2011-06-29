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
#ifndef __DAVAENGINE_DISPLAYMODE_H__
#define __DAVAENGINE_DISPLAYMODE_H__


namespace DAVA
{
/**
	\ingroup core
	\brief Class that describe display mode supported by device
*/
class DisplayMode
{
public:
	DisplayMode()
	{
		width = 0;
		height = 0;
		bpp = 0;
		refreshRate = -1;
	}
	DisplayMode(int32 _width, int32 _height, int32 _bpp, int32 _refreshRate)
	{
		width = _width;
		height = _height;
		bpp = _bpp;
		refreshRate = _refreshRate;
	}
	
	bool IsValid()
	{
		return (width > 0 && height > 0 && refreshRate != -1);
	}
	
	int32 width;	//! width of the display mode
	int32 height;	//! height of the display mode
	int32 bpp;		//! bits per pixel 
	int32 refreshRate;	//! refresh rate of the display mode, 0 if default
};

};

#endif // __DAVAENGINE_DISPLAYMODE_H__