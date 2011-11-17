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
        * Created by Kiryl Polyakov 
=====================================================================================*/
#ifndef __TEXT_GAME_OBJECT__
#define __TEXT_GAME_OBJECT__

#include "Scene2D/GameObject.h"

namespace DAVA
{
    class Font;
    struct TextBlock;
/**
	\ingroup scene2d
	\brief represent text as game object
 */
class TextGameObject : public GameObject
{
public: // from UIStaticText
    TextGameObject(const Rect &rect);
    TextGameObject(const Rect &rect, Font *font, const WideString &string);

    void SetText(const WideString &string, const Vector2 &requestedTextRectSize = Vector2(0,0));
    void SetFont(Font *font, bool prepareSprite = true);
    void SetMultiline(bool isMultilineEnabled);
    void SetFittingOption(int32 fittingType);
    void SetAlign(int32 alignment);

protected:
    void PrepareSprite();

protected:
    TextBlock *textBlock;
};
};
#endif // __TEXT_GAME_OBJECT__