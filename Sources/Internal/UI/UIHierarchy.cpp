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

#include "UI/UIHierarchy.h"
#include "UI/UIHierarchyCell.h"
#include "UI/UIControlSystem.h"

namespace DAVA
{
UIHierarchy::UIHierarchy(const Rect &rect, bool rectInAbsoluteCoordinates)
: UIControl(rect, rectInAbsoluteCoordinates)
{
    baseNode = new UIHierarchyNode(NULL);
    baseNode->isOpen = true;
    needRecalc = false;

    Rect r = GetRect();
    r.x = 0;
    r.y = 0;
    scrollContainer = new UIControl(r);
    AddControl(scrollContainer);
    
    oldPos = 0;
    newPos = 0;
    
    mainTouch = 0;
    
    touchHoldSize = 15;
    
    cellHeight = 30;
    shiftWidth = 25;
    
    lockTouch = false;
    
    needRedraw = false;
    
    scroll = new ScrollHelper();
}

UIHierarchy::~UIHierarchy()
{
    SafeDelete(baseNode);

    SafeRelease(scrollContainer);
    SafeRelease(scroll);
    
    for (Map<String,Vector<UIHierarchyCell*>*>::iterator mit = cellStore.begin() ; mit != cellStore.end(); mit++) 
    {
        for (Vector<UIHierarchyCell*>::iterator it = mit->second->begin(); it != mit->second->end(); it++) 
        {
            SafeRelease(*it);
        }
        delete mit->second;
        mit->second = NULL;
    }
}

void UIHierarchy::SetDelegate(UIHierarchyDelegate *newDelegate)
{
    delegate = newDelegate;
    Refresh();
}

void UIHierarchy::SetCellHeight(int32 newCellHeight)
{
    cellHeight = newCellHeight;
}

int32 UIHierarchy::GetCellHeight()
{
    return cellHeight;
}

void UIHierarchy::SetShiftWidth(int32 newShiftWidth)
{
    shiftWidth = newShiftWidth;
}

int32 UIHierarchy::GetShiftWidth()
{
    return shiftWidth;
}


void UIHierarchy::FullRedraw()
{
    scrollContainer->RemoveAllControls();
    if(!delegate)
    {
        return;
    }
    
    needRedraw = false;
    
    addPos = 0;
    scroll->SetViewSize(size.y);
    if (needRecalc) 
    {
        RecalcHierarchy();
    }

    iheritanceQueue.clear();
    AddAfter(baseNode);
    
    scroll->SetElementSize((float32)baseNode->openedChildrenCount * cellHeight);
}

void UIHierarchy::AddCellsAfter(UIHierarchyNode *afterNode)
{
    iheritanceQueue.clear();
    UIHierarchyNode *nd = afterNode;
    while (nd != baseNode)
    {
        iheritanceQueue.push_front(nd);
        nd = nd->parent;
    }
    
    AddAfter(baseNode);
}

void UIHierarchy::AddAfter(UIHierarchyNode *forParent)
{
    UIHierarchyNode *checkNode = NULL;
    if (!iheritanceQueue.empty()) 
    {
        checkNode = *(iheritanceQueue.begin());
        iheritanceQueue.pop_front();
    }
    for (List<UIHierarchyNode *>::iterator it = forParent->children.begin(); it != forParent->children.end(); it++) 
    {
        if (checkNode) 
        {
            if ((*it) == checkNode)
            {
                checkNode = NULL;
                if ((*it)->isOpen) 
                {
                    AddAfter((*it));
                }
                if(addPos + (int32)scrollContainer->relativePosition.y > size.y * 2)
                {
                    return;
                }
            }
        }
        else 
        {
            if(addPos + (int32)scrollContainer->relativePosition.y + cellHeight > -size.y)
            {
                AddCellAtPos(delegate->CellForNode(this, (*it)->userNode), addPos, cellHeight, (*it));
            }
            addPos += cellHeight;
            if(addPos + (int32)scrollContainer->relativePosition.y > size.y * 2)
            {
                return;
            }
            if ((*it)->isOpen) 
            {
                AddAfter((*it));
            }
        }
    }
}

void UIHierarchy::AddCellsBefore(UIHierarchyNode *beforeNode)
{
    iheritanceQueue.clear();
    UIHierarchyNode *nd = beforeNode;
    while (nd != baseNode)
    {
        iheritanceQueue.push_front(nd);
        nd = nd->parent;
    }
    
    AddBefore(baseNode);
}

void UIHierarchy::AddBefore(UIHierarchyNode *forParent)
{
    UIHierarchyNode *checkNode = NULL;
    if (!iheritanceQueue.empty()) 
    {
        checkNode = *(iheritanceQueue.begin());
        iheritanceQueue.pop_front();
    }
    for (List<UIHierarchyNode *>::reverse_iterator it = forParent->children.rbegin(); it != forParent->children.rend(); it++) 
    {
        if (checkNode) 
        {
            if ((*it) == checkNode)
            {
                checkNode = NULL;
                if ((*it)->isOpen && !iheritanceQueue.empty())
                {
                    AddBefore((*it));
                    if(addPos + (int32)scrollContainer->relativePosition.y + cellHeight <= -size.y)
                    {
                        return;
                    }
                    if(addPos + (int32)scrollContainer->relativePosition.y <= size.y * 2)
                    {
                        AddCellAtPos(delegate->CellForNode(this, (*it)->userNode), addPos, cellHeight, (*it));
                    }
                    addPos -= cellHeight;
                }
            }
        }
        else 
        {
            if(addPos + (int32)scrollContainer->relativePosition.y + cellHeight <= -size.y)
            {
                return;
            }
            if ((*it)->isOpen) 
            {
                AddBefore((*it));
                if(addPos + (int32)scrollContainer->relativePosition.y + cellHeight <= -size.y)
                {
                    return;
                }
            }
            if(addPos + (int32)scrollContainer->relativePosition.y <= size.y * 2)
            {
                AddCellAtPos(delegate->CellForNode(this, (*it)->userNode), addPos, cellHeight, (*it));
            }
            addPos -= cellHeight;
        }
    }
}


void UIHierarchy::Refresh()
{
    needRedraw = true;
    needRecalc = true;
}

void UIHierarchy::Redraw()
{
    needRedraw = true;
}

void UIHierarchy::Update(float32 timeElapsed)
{
    if(!delegate)
    {
        return;
    }
    
    if(needRedraw)
    {
        FullRedraw();
    }
    
    scroll->SetViewSize(size.y);
    
    float d = newPos - oldPos;
    oldPos = newPos;
    float32 newY = scroll->GetPosition(d, timeElapsed, lockTouch);
    if (scrollContainer->relativePosition.y != newY) 
    {
        scrollContainer->relativePosition.y = newY;

        List<UIControl*>::const_iterator it;
        Rect viewRect = GetGeometricData().GetUnrotatedRect();//GetRect(TRUE);
        const List<UIControl*> &scrollList = scrollContainer->GetChildren();
        List<UIControl*> removeList;
        
            //removing invisible elements
        for(it = scrollList.begin(); it != scrollList.end(); it++)
        {
            Rect crect = (*it)->GetGeometricData().GetUnrotatedRect();//GetRect(TRUE);
            if(crect.y <= viewRect.y - viewRect.dy || crect.y > viewRect.y + viewRect.dy*2)
            {
                removeList.push_back(*it);
            }
        }
        for(it = removeList.begin(); it != removeList.end(); it++)
        {
            scrollContainer->RemoveControl((*it));
        }
        
        UIControl *lastCell = NULL;
        UIControl *firstCell = NULL;
        int32 maxPos = -999999; 
        int32 minPos = 999999;
        for(it = scrollList.begin(); it != scrollList.end(); it++)
        {
            if ((*it)->relativePosition.y > maxPos) 
            {
                maxPos = (int32)(*it)->relativePosition.y;
                lastCell = (*it);
            }
            if ((*it)->relativePosition.y < minPos)
            {
                minPos = (int32)(*it)->relativePosition.y;
                firstCell = (*it);
            }
        }
        
            //adding elements to the list bottom
        if (lastCell) 
        {
            addPos = (int32)lastCell->relativePosition.y + cellHeight;
            if(addPos + (int32)scrollContainer->relativePosition.y <= size.y * 2)
            {
                AddCellsAfter(((UIHierarchyCell *)lastCell)->node);
            }
        }
        
            //adding elements to the list top
        if (firstCell) 
        {
            addPos = (int32)firstCell->relativePosition.y - cellHeight;
            if(addPos + (int32)scrollContainer->relativePosition.y + cellHeight > -size.y)
            {
                AddCellsBefore(((UIHierarchyCell *)firstCell)->node);
            }
        }
    }

    
    
    
}

void UIHierarchy::Draw(const UIGeometricData &geometricData)
{
    if(needRedraw)
    {
        FullRedraw();
    }
    UIControl::Draw(geometricData);
}

void UIHierarchy::Input(UIEvent *currentInput)
{
    newPos = currentInput->point.y;
    
    switch (currentInput->phase) 
    {
        case UIEvent::PHASE_BEGAN:
        {
            lockTouch = true;
            oldPos = newPos;
            mainTouch = currentInput->tid;
        }
            break;
        case UIEvent::PHASE_DRAG:
        {
        }
            break;
        case UIEvent::PHASE_ENDED:
        {
            lockTouch = false;
            mainTouch = 0;
        }
            break;
    }
}

bool UIHierarchy::SystemInput(UIEvent *currentInput)
{
    if(!inputEnabled || !visible || controlState & STATE_DISABLED)
    {
        return false;
    }
    
    if(currentInput->touchLocker != this)
    {
        if(currentInput->phase == UIEvent::PHASE_BEGAN)
        {
            if(IsPointInside(currentInput->point))
            {
                PerformEvent(EVENT_TOUCH_DOWN);
                Input(currentInput);
            }
        }
        else if(currentInput->tid == mainTouch && currentInput->phase == UIEvent::PHASE_DRAG)
        {
            if(abs(currentInput->point.y - newPos) > touchHoldSize)
            {
                UIControlSystem::Instance()->SwitchInputToControl(mainTouch, this);
                newPos = currentInput->point.y;
                return true;
            }
        }
        else if(currentInput->tid == mainTouch && currentInput->phase == UIEvent::PHASE_ENDED)
        {
            mainTouch = 0;
            lockTouch = false;
        }
        
        
        
    }
    
    return UIControl::SystemInput(currentInput);
}

void UIHierarchy::OnSelectEvent(BaseObject *pCaller, void *pUserData, void *callerData)
{
    if(delegate)
    {
        delegate->OnCellSelected(this, (UIHierarchyCell *)pCaller);
    }
}

void UIHierarchy::OnOpenPressedEvent(BaseObject *pCaller, void *pUserData, void *callerData)
{
    UIHierarchyCell *c = (UIHierarchyCell *)((UIControl *)pCaller)->GetParent();
    if (!c->node->isOpen)
    {
        c->node->isOpen = true;
        CorrectOpenedInParent(c->node->parent, RecalcNode(c->node));
        Refresh();
    }
    else 
    {
        c->node->isOpen = false;
        CorrectOpenedInParent(c->node->parent, -c->node->openedChildrenCount);
        Refresh();
    }
    Logger::Info("Elements count = %d", baseNode->openedChildrenCount);
}


void UIHierarchy::AddCellAtPos(UIHierarchyCell *cell, int32 pos, int32 size, UIHierarchyNode *node)
{
    DVASSERT(cell);
    DVASSERT(cell->cellStore == NULL || cell->cellStore == this);
    if(!cell->cellStore)
    {
        cell->cellStore = this;
        cell->AddEvent(EVENT_TOUCH_UP_INSIDE, Message(this, &UIHierarchy::OnSelectEvent));
        cell->openButton->AddEvent(EVENT_TOUCH_UP_INSIDE, Message(this, &UIHierarchy::OnOpenPressedEvent));
        Vector<UIHierarchyCell*> *store = GetStoreVector(cell->identifier);
        if(!store)
        {
            store = new Vector<UIHierarchyCell*>;
            cellStore[cell->identifier] = store;
        }
        store->push_back(cell);
    }
    cell->node = node;
    cell->RemoveControl(cell->openButton);
    if (node->hasChildren) 
    {
        cell->AddControl(cell->openButton);
    }
    cell->size.y = (float32)size;
    cell->relativePosition.y = (float32)pos;
    cell->relativePosition.x = (float32)cell->node->nodeLevel * shiftWidth;
    scrollContainer->AddControl(cell);
    
}

Vector<UIHierarchyCell*> *UIHierarchy::GetStoreVector(const String &cellIdentifier)
{
    Map<String,Vector<UIHierarchyCell*>*>::const_iterator mit;
    mit = cellStore.find(cellIdentifier);
    if (mit == cellStore.end())
    {
        return NULL;
    }
    
    return mit->second;
}

UIHierarchyCell* UIHierarchy::GetReusableCell(const String &cellIdentifier)
{
    Vector<UIHierarchyCell*> *store = GetStoreVector(cellIdentifier);
    if(!store)
    {
        return NULL;
    }
    
    for(Vector<UIHierarchyCell*>::iterator it = store->begin(); it != store->end(); it++)
    {
        if((*it)->node == NULL)
        {
            return (*it);
        }
    }
    
    return NULL;
    
}

const List<UIControl*> &UIHierarchy::GetVisibleCells()
{
    return scrollContainer->GetChildren();	
}


//    void UIHierarchyCell::SetTouchHoldDelta(int32 holdDelta)
//    {
//        touchHoldSize = holdDelta;
//    }
//    int32 UIHierarchyCell::GetTouchHoldDelta()
//    {
//        return touchHoldSize;
//    }
//    
//    void UIHierarchyCell::SetSlowDownTime(float newValue)
//    {
//        scroll->SetSlowDownTime(newValue);
//    }
//    void UIHierarchyCell::SetBorderMoveModifer(float newValue)
//    {
//        scroll->SetBorderMoveModifer(newValue);
//    }
//    
//    void UIHierarchyCell::SystemWillAppear()
//    {
//        UIControl::SystemWillAppear();	
//        Refresh();
//    }



//    int32 UIHierarchy::ElementsCount(UIList *forList)
//    {
//        if (!delegate) 
//        {
//            return 0;
//        }
//        if (needRecalc) 
//        {
//            RecalcHierarchy();
//        }
//        
//        return elementsCount;
//    }
//    
//    UIListCell *UIHierarchy::CellAtIndex(UIList *forList, int32 index)
//    {
//        return NULL;
//    }
//    
//    int32 UIHierarchy::CellHeight(UIList *forList, int32 index)//calls only for vertical orientation
//    {
//        return 30;
//    }

void UIHierarchy::RecalcHierarchy()
{
    DVASSERT(delegate);
    needRecalc = false;
    RecalcNode(baseNode);
}

int32 UIHierarchy::RecalcNode(UIHierarchyNode *parent)
{
    int32 totalCount = 0;
    int32 cnt = delegate->ChildrenCount(this, parent->userNode);

    List<UIHierarchyNode *> newNodes;
    for (int i = 0; i < cnt; i++)
    {
        void *userNode = delegate->ChildAtIndex(this, parent->userNode, i);
        UIHierarchyNode *nd = parent->CheckChildrenForUserNode(userNode);
        if (!nd) 
        {
            nd = new UIHierarchyNode(userNode);
        }
        else
        {
            parent->RemoveChild(nd);
        }
        newNodes.push_back(nd);
        nd->nodeLevel = parent->nodeLevel + 1;
    }
    parent->DeleteChildren();

    for (List<UIHierarchyNode *>::iterator it = newNodes.begin(); it != newNodes.end(); it++) 
    {
        parent->AddChild(*it);
        if ((*it)->isOpen) 
        {
            totalCount += RecalcNode((*it));
        }
        else 
        {
            (*it)->hasChildren = delegate->IsNodeExpandable(this, (*it)->userNode);
            
        }
        totalCount++;
    }
    
    parent->openedChildrenCount = totalCount;
    return totalCount;
}

void UIHierarchy::CorrectOpenedInParent(UIHierarchyNode *parent, int32 nodesDelta)
{
    if (parent) 
    {
        parent->openedChildrenCount += nodesDelta;
        CorrectOpenedInParent(parent->parent, nodesDelta);
    }
}



float32 UIHierarchy::VisibleAreaSize(UIScrollBar *forScrollBar)
{
    return scroll->GetViewSize();
}

float32 UIHierarchy::TotalAreaSize(UIScrollBar *forScrollBar)
{
    return scroll->GetElementSize();
}

float32 UIHierarchy::ViewPosition(UIScrollBar *forScrollBar)
{
    return scroll->GetPosition();
}

void UIHierarchy::OnViewPositionChanged(UIScrollBar *byScrollBar, float32 newPosition)
{
    scroll->SetPosition(-newPosition);
}


};