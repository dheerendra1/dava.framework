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

#ifndef __DAVAENGINE_UI_HIERARCHY_H__
#define __DAVAENGINE_UI_HIERARCHY_H__

#include "Base/BaseTypes.h"
#include "UI/UIControl.h"
#include "UI/UIListCell.h"
#include "UI/UIScrollBar.h"
#include "UI/ScrollHelper.h"
#include "UIHierarchyNode.h"

namespace DAVA 
{

    
/*
    Boroda Comments:
    1) Add documentation (with example) similar to UIList
    4) Rename please to UI3
*/

    ///TODO: do-do
    ///Improve perfomance on node open/close operations
    ///Improve perfomance on node adding/removing operations
class UIHierarchy;
class UIHierarchyCell;

    /**
     \ingroup controlsystem
     \brief UIHierarchyDelegate interface declares methods that are implemented by the delegate of UIHierarchy control. 
     The methods provide data for UIHierarchy, and define it's content and allow to modify it's behaviour. 
     */
class UIHierarchyDelegate 
{
    friend class UIHierarchy;
    virtual bool IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode) = 0;
    virtual int32 ChildrenCount(UIHierarchy *forHierarchy, void *forParent) = 0;
    virtual void *ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index) = 0;
    virtual UIHierarchyCell *CellForNode(UIHierarchy *forHierarchy, void *node) = 0;
    virtual void OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
    {};
};



    /**
     \ingroup controlsystem
     \brief UIHierarchy is a control for displaying hierarchy tree information on the screen. 
     It's simple and powerfull. Using this class you can create hierarhical lists or trees. 
     
     Example of UIHierarchy usage:
     \code
     //on hierarchy creation you need to set your class as the delegate
     void AnimationTestScreen::LoadResources()
     {
         hierarchy = new UIHierarchy(Rect(0, 0, size.x/3, size.y));
         hierarchy->SetCellHeight(25);
         hierarchy->SetDelegate(this);
         AddControl(hierarchy);
     }
     
     //next method should be realized in a delegate class
     
     //returns true if node is can be opened
     bool AnimationTestScreen::IsNodeExpandable(UIHierarchy *forHierarchy, void *forNode)
     {
         if (forNode) 
         {
            return ((SceneNode*)forNode)->GetChildrenCount() > 0;
         }
         
     //for the root node
         return scene->GetChildrenCount() > 0;
     }
     
     //returns node children count
     int32 AnimationTestScreen::ChildrenCount(UIHierarchy *forHierarchy, void *forParent)
     {
         if (forParent) 
         {
            return ((SceneNode*)forParent)->GetChildrenCount();
         }
         
     //for the root node
         return scene->GetChildrenCount();
     }
     
     //retruns child of the node with the specific index
     void *AnimationTestScreen::ChildAtIndex(UIHierarchy *forHierarchy, void *forParent, int32 index)
     {
         if (forParent) 
         {
            return ((SceneNode*)forParent)->GetChild(index);
         }

     //for the root node
         return scene->GetChild(index);
     }
     
     //create cells and fill them with data
     //works similar as the UIListCell creation
     UIHierarchyCell *AnimationTestScreen::CellForNode(UIHierarchy *forHierarchy, void *node)
     {
        UIHierarchyCell *c = forHierarchy->GetReusableCell("Node cell"); //try to get cell from the reusable cells store
         if(!c)
         { //if cell of requested type isn't find in the store create new cell
            c = new UIHierarchyCell(Rect(0, 0, 200, 15), "Node cell");
         }
         //fill cell whith data
         Font *fnt;
         fnt = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
         //    fnt = GraphicsFont::Create("~res:/Fonts/korinna.def", "~res:/Gfx/Fonts2/korinna");
         fnt->SetSize(12);
         
         SceneNode *n = (SceneNode *)node;
         
         c->text->SetFont(fnt);
         c->text->SetText(StringToWString(n->name));
         c->text->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
         SafeRelease(fnt);
         
         Color color(0.1, 0.5, 0.05, 1.0);
         c->openButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
         c->openButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
         c->openButton->SetStateDrawType(UIControl::STATE_HOVER, UIControlBackground::DRAW_FILL);
         c->openButton->GetStateBackground(UIControl::STATE_NORMAL)->color = color;
         c->openButton->GetStateBackground(UIControl::STATE_HOVER)->color = color + 0.1;
         c->openButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->color = color + 0.3;
         
         return c;//returns cell
     //your application don't need to manage cells. UIHierarchy do all cells management.
     //you can create cells of your own types derived from the UIHierarchyCell
     }
     
     
     //when cell is pressed
     void AnimationTestScreen::OnCellSelected(UIHierarchy *forHierarchy, UIHierarchyCell *selectedCell)
     {
     }
     \endcode
     */
    
class UIHierarchy : public UIControl, public UIScrollBarDelegate
{
    
public:

    UIHierarchy(const Rect &rect, bool rectInAbsoluteCoordinates = false);
    ~UIHierarchy();

    void SetDelegate(UIHierarchyDelegate *newDelegate);

    const List<UIControl*> &GetVisibleCells();

    void SetCellHeight(int32 newCellHeight);
    int32 GetCellHeight();

    void SetShiftWidth(int32 newShiftWidth);
    int32 GetShiftWidth();


    UIHierarchyCell* GetReusableCell(const String &cellIdentifier);

    void Refresh();

    
        //UIScrollBar support
    virtual float32 VisibleAreaSize(UIScrollBar *forScrollBar);
    virtual float32 TotalAreaSize(UIScrollBar *forScrollBar);
    virtual float32 ViewPosition(UIScrollBar *forScrollBar);
    virtual void OnViewPositionChanged(UIScrollBar *byScrollBar, float32 newPosition);

    virtual void Update(float32 timeElapsed);
    virtual void Draw(const UIGeometricData &geometricData);
    virtual void Input(UIEvent *currentInput);
    virtual bool SystemInput(UIEvent *currentInput);
    

    
protected:
    


    void OnSelectEvent(BaseObject *pCaller, void *pUserData, void *callerData);
    void OnOpenPressedEvent(BaseObject *pCaller, void *pUserData, void *callerData);

    void FullRedraw();
    void Redraw();
    
//        void RecursiveRefresh(UIHierarchyNode *forNode);


    void AddCellsAfter(UIHierarchyNode *afterNode);
    void AddAfter(UIHierarchyNode *forParent);

    void AddCellsBefore(UIHierarchyNode *beforeNode);
    void AddBefore(UIHierarchyNode *forParent);

    List<UIHierarchyNode *> iheritanceQueue;
    
    
    
    UIControl *scrollContainer;
    
    void RecalcHierarchy();
    int32 RecalcNode(UIHierarchyNode *node);
    void CorrectOpenedInParent(UIHierarchyNode *parent, int32 nodesDelta);

    
    void AddCellAtPos(UIHierarchyCell *cell, int32 pos, int32 size, UIHierarchyNode *node);
    Vector<UIHierarchyCell*> *GetStoreVector(const String &cellIdentifier);
    

    
    
    int32 mainTouch;
    float32 oldPos;
    float32 newPos;
    bool lockTouch;

    int32 touchHoldSize;

    ScrollHelper *scroll;
    
    
    
    bool needRecalc;
    bool needRedraw;
    
    
    int32 addPos;
    
    int32 cellHeight;
    int32 shiftWidth;
    
    


    UIHierarchyDelegate *delegate;

    UIHierarchyNode *baseNode;
    
    Map<String, Vector<UIHierarchyCell*>*> cellStore;

};
};

#endif
