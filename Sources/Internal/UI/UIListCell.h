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

#ifndef __DAVAENGINE_UI_LIST_CELL_H__
#define __DAVAENGINE_UI_LIST_CELL_H__

#include "UI/UIButton.h"

namespace DAVA 
{
	/**
	 \ingroup controlsystem
	 \brief Cell unit for the UIList.
		UIButton that can be managed by the UIList.
	 */
	
	class UIListCell : public UIButton 
	{
		friend class UIList;
	public:
        UIListCell();
		/**
		 \brief Constructor.
		 \param[in] rect Used only size part of the rect. Incoming rect size can be modified by the UIList if this is neccesary.
		 \param[in] cellIdentifier literal identifier to represents cell type. For example: "Name cell", "Phone number cell", etc.
		 */
		UIListCell(const Rect &rect, const String &cellIdentifier);

        /**
		 \brief Returns cell's identifier.
		 \returns identifier
		 */
        const String & GetIdentifier();
        
		/**
		 \brief Returns current cell sequence number in the list.
		 \returns list item index
		 */
		int32 GetIndex();
        
        virtual UIControl *Clone();
        void CopyDataFrom(UIControl *srcControl);
        /**
         \brief Creates the absoulutely identic copy of the list cell.
         \returns copy of the control
         */
        UIListCell* CloneListCell();
        
        virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);
		
	protected:
		virtual ~UIListCell();

		virtual void WillDisappear();

		
	private:
		int32 currentIndex;
		String identifier;
		
		void *cellStore;
		
	};
}

#endif