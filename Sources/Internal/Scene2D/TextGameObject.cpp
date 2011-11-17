#include "TextGameObject.h"
#include "Render/2D/TextBlock.h"
#include "Render/2D/FTFont.h"

namespace DAVA
{
TextGameObject::TextGameObject(const Rect &rect)
{
    textBlock = TextBlock::Create(Vector2(rect.dx, rect.dy));
    SetPosition(rect.x, rect.y);
}

TextGameObject::TextGameObject(const Rect &rect, Font *font, const WideString &string)
{
    const Vector2 requestedRect(rect.dx, rect.dy);
    textBlock = TextBlock::Create(requestedRect);
    SetPosition(rect.x, rect.y);
    SetFont(font, false);
    SetText(string, requestedRect);
}

void TextGameObject::PrepareSprite()
{
    if(textBlock->IsSpriteReady())
    {
        if(GetSprite() != textBlock->GetSprite()) 
        {
            SetSprite(textBlock->GetSprite());
        }
    }
    else 
    {
        SetSprite(NULL);
    }
}

void TextGameObject::SetText( const WideString &string, const Vector2 &requestedTextRectSize /*= Vector2(0,0)*/ )
{
    textBlock->SetText(string, requestedTextRectSize);
    PrepareSprite();
}

void TextGameObject::SetFittingOption(int32 fittingType)
{
    textBlock->SetFittingOption(fittingType);
    PrepareSprite();
}

void TextGameObject::SetFont( Font *font, bool prepareSprite /*= true*/ )
{
    textBlock->SetFont(font);
    if(prepareSprite) 
        PrepareSprite();
}

void TextGameObject::SetMultiline(bool isMultilineEnabled)
{
    textBlock->SetMultiline(isMultilineEnabled);
    PrepareSprite();
}

void TextGameObject::SetAlign(int32 alignment)
{
    textBlock->SetAlign(alignment);
}
};