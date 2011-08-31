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


#ifndef __DAVAENGINE_UI_LIST_H__
#define __DAVAENGINE_UI_LIST_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"
#include "UI/UIListCell.h"
#include "UI/UIScrollBar.h"

#include "UI/ScrollHelper.h"

namespace DAVA 
{

class UIList;
/**
	\ingroup controlsystem
	\brief UIListDelegate interface declares methods that are implemented by the delegate of UIList control. 
	The methods provide date for UIList, and define it's content and allow to modify it's behaviour. 
 */
class UIListDelegate 
{
	friend class UIList;
	virtual int32 ElementsCount(UIList *forList) = 0;
	virtual UIListCell *CellAtIndex(UIList *forList, int32 index) = 0;
	virtual int32 CellWidth(UIList *forList, int32 index)//calls only for horizontal orientation
	{return 20;};
	virtual int32 CellHeight(UIList *forList, int32 index)//calls only for vertical orientation
	{return 20;};
	virtual void OnCellSelected(UIList *forList, UIListCell *selectedCell)
	{};
};
/**
	\ingroup controlsystem
	\brief UIList is a control for displaying lists of information on the screen. 
	It's simple and powerfull. Using this class you can create list. 
	Lists can be vertical also, so you can create scrollable pages easily.
 
	Example of UIList usage:
	\code
	//on list creation you need to set your class as the delegate
	void MultiplayerScreen::LoadResources()
	{
		serversList = new UIList(Rect(10, 45, 460, 210), UIList::ORIENTATION_VERTICAL);
		serversList->SetDelegate(this);
		AddControl(serversList);
	}

	//next method should be realized in a delegate class

	//returns total cells count in the list
	int32 MultiplayerScreen::ElementsCount(UIList *forList)
	{
		return GameServer::Instance()->totalServers.size();
	}

	//returns cell dimension for the UIList calculations
	int32 MultiplayerScreen::CellHeight(UIList *forList, int32 index)//calls only for vertical orientation
	{
		return SERVER_CELL_HEIGHT;
	}

	//create cells and fill them with data
	UIListCell *MultiplayerScreen::CellAtIndex(UIList *forList, int32 index)
	{
		GameServerCell *c = (GameServerCell *)forList->GetReusableCell("Server cell"); //try to get cell from the reusable cells store
		if(!c)
		{ //if cell of requested type isn't find in the store create new cell
			c = new GameServerCell(Rect((0, 0, 200, SERVER_CELL_HEIGHT), "Server cell");
		}
		//fill cell whith data
		c->serverName = GameServer::Instance()->totalServers[index].name + LocalizedString("'s game");
		c->SetStateText(UIControl::STATE_NORMAL, c->serverName, Vector2(c->GetStateBackground(UIControl::STATE_NORMAL)->GetSprite()->GetWidth() * 1.7 - 30, 0));
		c->connection = GameServer::Instance()->totalServers[index].connection;
		c->serverIndex = GameServer::Instance()->totalServers[index].index;

		return c;//returns cell
		//your application don't need to manage cells. UIList do all cells management.
		//you can create cells of your own types derived from the UIListCell
	}

	//when cell is pressed
	void MultiplayerScreen::OnCellSelected(UIList *forList, UIListCell *selectedCell)
	{
		PlayButtonSound();

		currentName = selectedCell->serverName;
		currentConnection = selectedCell->connection;
	}
	\endcode
 */
class UIList : public UIControl , public UIScrollBarDelegate
{
public:
	
	static const int32 maximumElementsCount = 100000;
	enum eListOrientation 
	{
			ORIENTATION_VERTICAL = 0
		,	ORIENTATION_HORIZONTAL
	};
	
	UIList();
	UIList(const Rect &rect, eListOrientation requiredOrientation, bool rectInAbsoluteCoordinates = false);
	
	void SetDelegate(UIListDelegate *newDelegate);
	
	void ResetScrollPos();
	void RefreshList();
	
	void SetSlowDownTime(float newValue);//sets how fast reduce speed (for example 0.25 reduces speed to zero for the 0.25 second ). To remove inertion effect set tihs value to 0
	void SetBorderMoveModifer(float newValue);//sets how scrolling element moves after reachig a border (0.5 as a default). To remove movement effect after borders set thus value to 0

	void SetTouchHoldDelta(int32 holdDelta);//the amount of pixels user must move the finger on the button to switch from button to scrolling (default 30)
	int32 GetTouchHoldDelta();

	void ScrollTo(float delta);

	const List<UIControl*> &GetVisibleCells();


	UIListCell* GetReusableCell(const String &cellIdentifier);//returns cell from the cells cache, if returns 0 you need to create the new one
	
	virtual void SystemWillAppear(); // Internal method used by ControlSystem
	
	virtual void LoadFromYamlNode(YamlNode * node, UIYamlLoader * loader);


    virtual float32 VisibleAreaSize(UIScrollBar *forScrollBar);
    virtual float32 TotalAreaSize(UIScrollBar *forScrollBar);
    virtual float32 ViewPosition(UIScrollBar *forScrollBar);
    virtual void OnViewPositionChanged(UIScrollBar *byScrollBar, float32 newPosition);

protected:
	void InitAfterYaml();
	virtual ~UIList();

	void FullRefresh();

	virtual void Update(float32 timeElapsed);
	
	virtual void Input(UIEvent *currentInput);
	virtual bool SystemInput(UIEvent *currentInput);// Internal method used by ControlSystem

	virtual void SetRect(const Rect &rect, bool rectInAbsoluteCoordinates = FALSE);

	virtual void Draw(const UIGeometricData &geometricData);

	
    

	int32 addPos;
	
	Vector<UIListCell*> *GetStoreVector(const String &cellIdentifier);
	void AddCellAtPos(UIListCell *cell, int32 pos, int32 size, int32 index);
	
	void OnSelectEvent(BaseObject *pCaller, void *pUserData, void *callerData);

	
	UIListDelegate *delegate;
	eListOrientation orientation;
	
	UIControl *scrollContainer;
	
	int mainTouch;
	
	ScrollHelper *scroll;
	
	float oldPos;
	float newPos;
	bool lockTouch;
	
	int32 touchHoldSize;
	
	bool needRefresh;
	

	Map<String,Vector<UIListCell*>*> cellStore;

};
};
#endif
