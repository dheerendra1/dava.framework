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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __DAVAENGINE_UI_CONTROL_BACKGROUND_H__
#define __DAVAENGINE_UI_CONTROL_BACKGROUND_H__

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
#include "Render/2D/Sprite.h"

namespace DAVA
{
	
class UIControl;
class UIGeometricData;

	/**
	 \ingroup controlsystem
	 \brief Control background.
		Responsible for control graphical representation.
		In most cases UIControlBackground draws sprite inside control rect in compliance 
		with the set of requested rules.
	 */
	
class UIControlBackground : public BaseObject
{
public:
	/**
	 \enum Control draw types.
	 */
	enum eDrawType 
	{
		DRAW_ALIGNED = 0,			//!<Align sprite inside ronctrol rect.
		DRAW_SCALE_TO_RECT,			//!<Scale sprite along the all control rect.
		DRAW_SCALE_PROPORTIONAL,	//!<Scale sptire to fit both width and height into the control rect but with keeping sprite proportions.
        DRAW_SCALE_PROPORTIONAL_ONE,//!<Scale sptire to fit width or height into control rect but with keeping sprite proportions.
		DRAW_FILL,					//!<Fill control rect with the control color.
		DRAW_STRETCH_HORIZONTAL,	//!<Stretch sprite horizontally along the control rect.
		DRAW_STRETCH_VERTICAL,		//!<Stretch sprite vertically along the control rect.
		DRAW_STRETCH_BOTH			//!<Stretch sprite along the all control rect.
	};
	
	/**
	 \enum Type of the color inheritnce from the parent control.
	 */
	enum eColorInheritType 
	{
		COLOR_MULTIPLY_ON_PARENT = 0	//!<Draw color = control color * parent color.
		,	COLOR_ADD_TO_PARENT			//!<Draw color = Min(control color + parent color, 1.0f).
		,	COLOR_REPLACE_TO_PARENT		//!<Draw color = parent color.
		,	COLOR_IGNORE_PARENT			//!<Draw color = control color.
		,	COLOR_MULTIPLY_ALPHA_ONLY	//!<Draw color = control color. Draw alpha = control alpha * parent alpha.
		,	COLOR_REPLACE_ALPHA_ONLY	//!<Draw color = control color. Draw alpha = parent alpha.

		,	COLOR_INHERIT_TYPES_COUNT
	};
	
	/**
	 \brief Constructor.
	 */
	UIControlBackground();
	
	/**
	 \brief Returns Sprite used for draw.
	 \returns Sprite used for draw.
	 */
	virtual Sprite*	GetSprite();
	/**
	 \brief Returns Sprite frame used for draw.
	 \returns Sprite frame used for draw.
	 */
	virtual int32	GetFrame();
	/**
	 \brief Returns Sprite align in the control rect.
	 \returns Sprite eAlign bit mask used for draw.
	 */
	virtual int32	GetAlign();
	/**
	 \brief Returns current draw type.
	 \returns Draw type used for draw.
	 */
	virtual UIControlBackground::eDrawType	GetDrawType();
	/**
	 \brief Returns horizontal or vertical sprite flips.
	 \returns eSpriteModification bits.
	 */
	virtual int32	GetModification();
	
	
	
	/**
	 \brief Sets control Sprite.
	 \param[in] drawSprite Pointer for a Sprite.
	 \param[in] drawFrame Sprite frame you want to use for draw.
	 */
	virtual void SetSprite(Sprite* drawSprite, int32 drawFrame);
	/**
	 \brief Sets control Sprite.
	 \param[in] drawSprite Sprite path-name.
	 \param[in] drawFrame Sprite frame you want to use for draw.
	 */
	virtual void SetSprite(const String &drawSprite, int32 drawFrame);
	/**
	 \brief Sets Sprite align in the control rect you want to use for draw.
	 \param[in] drawAlign Sprite eAlign bit mask.
	 */
	virtual void SetAlign(int32 drawAlign);
	/**
	 \brief Sets draw type you want to use.
	 \param[in] drawType Draw type eDrawType to use for drawing.
	 */
	virtual void SetDrawType(UIControlBackground::eDrawType drawType);
	/**
	 \brief Sets Sprite frame you want to use.
	 \param[in] drawFrame Sprite frame.
	 */
	virtual void SetFrame(int32 drawFrame);
	/**
	 \brief Sets size of the left and right unscalable sprite part.
		Middle sprite part would be scaled along a full control width.
		Used for DRAW_STRETCH_HORIZONTAL, DRAW_STRETCH_BOTH draw types.
	 \param[in] leftStretchCap Unscalable part in pixels.
	 */
	virtual void SetLeftRightStretchCap(float32 leftStretchCap);
	/**
	 \brief Sets size of the top and bottom unscalable sprite part.
		Middle sprite part would be scaled along a full control height.
		Used for DRAW_STRETCH_VERTICAL, DRAW_STRETCH_BOTH draw types.
	 \param[in] topStretchCap Unscalable part in pixels.
	 */
	virtual void SetTopBottomStretchCap(float32 topStretchCap);
	/**
	 \brief Sets horizontal or vertical sprite flip modificators.
	 \param[in] modification eSpriteModification bit mask.
	 */
	virtual void SetModification(int32 modification);
	
	
	/**
	 \brief Sets color inheritance type.
		Color inheritance type it's a rules of the calculating resulting draw color 
		from the current control color and parent control color.
		Color inheritance type by default is COLOR_IGNORE_PARENT.
	 \param[in] inheritType color inheritance type.
	 */
	virtual void SetColorInheritType(UIControlBackground::eColorInheritType inheritType);
	/**
	 \brief Returns current color inheritance type.
		Color inheritance type it's a rules of the calculating resulting draw color 
		from the current control color and parent control color.
		Color inheritance type by default is COLOR_IGNORE_PARENT.
	 \param[in] eColorInheritType color inheritance type.
	 */
	virtual eColorInheritType GetColorInheritType();

	/**
	 \brief Draw selected sprite by selected rules in the current control rect.
		Default color is Color(1,1,1,1).
	 \param[in] geometricData Control geometric data.
	 */
	virtual void Draw(const UIGeometricData &geometricData);
	
	/**
	 \brief Creates the absoulutely identic copy of the background.
	 \returns UIControlBackground copy
	 */
	virtual UIControlBackground *Clone();
	/**
	 \brief Copies all background parameters from the source.
	 \param[in] srcBackground Source background to copy parameters from.
	 */
	virtual void CopyDataFrom(UIControlBackground *srcBackground);
	
	/**
	 \brief Returns final draw color. This color is affected by the parrent color.
	 \returns Real draw color.
	 */
	const Color & GetDrawColor();
	/**
	 \brief Sets parent control color.
	 \param[in] parentColor parent control color.
	 */
	void SetParentColor(const Color &parentColor);
	
	/**
	 \brief Sets draw color.
		Default color is Color(1,1,1,1).
	 \param[in] color control draw color.
	 */
	inline void SetColor(const Color & color); 
	
	// WTF? Probably we should move it to protected to avoid problems in future? 
	Color color;//!<Control color. By default is Color(1,1,1,1).
	int32 frame;//!<Sprite frame.
	bool usePerPixelAccuracy;//!<Is sprite should be drawn with per pixel accuracy. Used for texts, for example.
protected:
	void DrawStretched(const Rect &drawRect);

	Sprite *spr;
	int32 align;
	int32 type;
	int32 spriteModification;
	float32 leftStretchCap;
	float32 topStretchCap;
	int colorInheritType;
	
	Vector2 lastDrawPos;
	RenderDataObject * rdoObject;
    RenderDataStream * vertexStream;
    RenderDataStream * texCoordStream;
protected:
	~UIControlBackground();
	Color drawColor;
	
	
};
	
// Implementation	
inline void UIControlBackground::SetColor(const Color & _color)
{
	color = _color;
}

};

#endif