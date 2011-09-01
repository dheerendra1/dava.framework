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
#include "ImageUsageScreen.h"
#include "TouchEffectsControl.h"

#include "Render/ImageConvert.h"

void ImageUsageScreen::LoadResources()
{
	testImageOriginal = Image::CreateFromFile("~res:/PNGImages/logo.png");
	testImage4444 = Image::Create(testImageOriginal->GetWidth() / 2, testImageOriginal->GetHeight() / 2, Image::FORMAT_RGBA4444);

// 	ConvertDirect<uint32, uint16, ConvertRGBA8888toRGBA4444> convertRGBA8888toRGBA4444;
// 	convertRGBA8888toRGBA4444(
// 		testImageOriginal->GetData(), testImageOriginal->GetWidth(), testImageOriginal->GetHeight(), testImageOriginal->GetWidth() * 4,
// 		testImage4444->GetData(), testImage4444->GetWidth(), testImage4444->GetHeight(), testImage4444->GetWidth() * 2);

	ConvertDownscaleTwiceBillinear<uint32, uint16, UnpackRGBA8888, PackRGBA4444> convertDownscale;
	convertDownscale(testImageOriginal->GetData(), testImageOriginal->GetWidth(), testImageOriginal->GetHeight(), testImageOriginal->GetWidth() * 4,
		testImage4444->GetData(), testImage4444->GetWidth(), testImage4444->GetHeight(), testImage4444->GetWidth() *  2);


	//Texture * tex = Texture::CreateFromData((T`))
	//Sprite * sprite = Sprite::CreateFromTexture(DAVA::Texture *fromTexture, DAVA::int32 xOffset, DAVA::int32 yOffset, DAVA::float32 sprWidth, DAVA::float32 sprHeight, bool contentScaleIncluded = false)

	Texture * texOrig = Texture::CreateFromData((Texture::PixelFormat)testImageOriginal->GetPixelFormat(), 
												testImageOriginal->GetData(), testImageOriginal->GetWidth(), 
												testImageOriginal->GetHeight());
	testImageRGBA8888 = Sprite::CreateFromTexture(texOrig, 0, 0, (float32)testImageOriginal->GetWidth(), (float32)testImageOriginal->GetHeight());
	SafeRelease(texOrig);


	Texture * texOrig2 = Texture::CreateFromData((Texture::PixelFormat)testImage4444->GetPixelFormat(), 
		testImage4444->GetData(), testImage4444->GetWidth(), 
		testImage4444->GetHeight());
	testImageRGBA4444 = Sprite::CreateFromTexture(texOrig2, 0, 0, (float32)testImage4444->GetWidth(), (float32)testImage4444->GetHeight());
	SafeRelease(texOrig2);
}

void ImageUsageScreen::UnloadResources()
{
	SafeRelease(testImageOriginal);
	SafeRelease(testImage4444);
	SafeRelease(testImageRGBA4444);
	SafeRelease(testImageRGBA8888);
}

void ImageUsageScreen::Draw(const UIGeometricData &geometricData)
{
	testImageRGBA8888->SetPosition(0, 0);
	testImageRGBA8888->Draw();

	testImageRGBA4444->SetPosition(0, testImageRGBA8888->GetHeight());
	testImageRGBA4444->Draw();
}
