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
#include "TestScreen.h"
#include <sys/time.h>

String emitterProps[] = {"emissionAngle", "emissionRage", "radius", "colorOverLife"};
String layerProps[] = {"sprite", "life", "lifeVariation", "number", "numberVariation", "size","sizeVariation", "sizeOverLife", "velocity", "velocityVariation", "velocityOverLife", "forces","forcesVariation", "forcesOverLife", "spin", "spinVariation", "spinOverLife", "motionRandom","motionRandomVariation", "motionRandomOverLife", "bounce", "bounceVariation", "bounceOverLife","colorRandom", "alphaOverLife", "colorOverLife"};

int32 emitterPropsCount = 4;
int32 layerPropsCount = 26;
int32 deltaIndex = 0;
bool curPropType = 0; //0 - value, 1 - Keyframed
Font *cellFont, *f;
Vector<TestScreen::Layer> layers;
int32 dblClickDelay = 500;
int32 activePropEdit = 0;
float32 curPropEditTime;

void TestScreen::LoadResources()
{
    cellH = GetScreenHeight() / 30;
    float32 buttonW = GetScreenWidth() / 6;
    
    sprite = 0;
    defSpriteFile = String("~res:/Gfx/Sparkles/default");
    
    cellFont = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    cellFont->SetSize(cellH/2);
    cellFont->SetColor(Color(1,1,1,1));
    
    selectedPropElement = -1;
    selectedEmitterElement = -1;
    selectedForceElement = -1;
    
    f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(18);
    f->SetColor(Color(1,1,1,1));

    newEmitter = new UIButton(Rect(0, 0, buttonW*2/3, cellH));
    newEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    newEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    newEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    newEmitter->SetStateText(UIControl::STATE_NORMAL, L"New");
	newEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(newEmitter);
    
	loadEmitter = new UIButton(Rect(buttonW*2/3, 0, buttonW*2/3, cellH));
    loadEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    loadEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    loadEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    loadEmitter->SetStateText(UIControl::STATE_NORMAL, L"Load");
	loadEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(loadEmitter);
    
    saveEmitter = new UIButton(Rect(buttonW*4/3, 0, buttonW*2/3, cellH));
    saveEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    saveEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    saveEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    saveEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    saveEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    saveEmitter->SetStateText(UIControl::STATE_NORMAL, L"Save");
	saveEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(saveEmitter);
    
    addLayer = new UIButton(Rect(0, cellH, buttonW/2, cellH));
    addLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    addLayer->SetStateText(UIControl::STATE_NORMAL, L"+ Layer");
	addLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addLayer);
    
    delLayer = new UIButton(Rect(buttonW/2, cellH, buttonW/2, cellH));
    delLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    delLayer->SetStateText(UIControl::STATE_NORMAL, L"- Layer");
	delLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delLayer);

    addProp = new UIButton(Rect(0, cellH*8, buttonW/2, cellH));
    addProp->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addProp->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addProp->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addProp->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addProp->SetStateFont(UIControl::STATE_NORMAL, f);
    addProp->SetStateText(UIControl::STATE_NORMAL, L"+ Prop");
	addProp->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addProp);

    delProp = new UIButton(Rect(buttonW/2, cellH*8, buttonW/2, cellH));
    delProp->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delProp->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delProp->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delProp->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delProp->SetStateFont(UIControl::STATE_NORMAL, f);
    delProp->SetStateText(UIControl::STATE_NORMAL, L"- Prop");
	delProp->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delProp);

    valueBut = new UIButton(Rect(buttonW, cellH*8, buttonW/2, cellH));
    valueBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    valueBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    valueBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    valueBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    valueBut->SetStateFont(UIControl::STATE_NORMAL, f);
    valueBut->SetStateText(UIControl::STATE_NORMAL, L"Value");
	valueBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(valueBut);
    
    KFBut = new UIButton(Rect(buttonW*3/2, cellH*8, buttonW/2, cellH));
    KFBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    KFBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    KFBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    KFBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    KFBut->SetStateFont(UIControl::STATE_NORMAL, f);
    KFBut->SetStateText(UIControl::STATE_NORMAL, L"KF");
	KFBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(KFBut);
    
    addForce = new UIButton(Rect(buttonW, cellH*7, buttonW/2, cellH));
    addForce->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addForce->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addForce->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addForce->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addForce->SetStateFont(UIControl::STATE_NORMAL, f);
    addForce->SetStateText(UIControl::STATE_NORMAL, L"+ Force");
	addForce->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addForce);
    
    delForce = new UIButton(Rect(buttonW*3/2, cellH*7, buttonW/2, cellH));
    delForce->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delForce->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delForce->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delForce->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delForce->SetStateFont(UIControl::STATE_NORMAL, f);
    delForce->SetStateText(UIControl::STATE_NORMAL, L"- Force");
	delForce->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delForce);
    
    emitterList = new UIList(Rect(0, cellH*2, buttonW, cellH*6), UIList::ORIENTATION_VERTICAL);
    emitterList->SetDelegate(this);
    emitterList->GetBackground()->SetColor(Color(0.2, 0.2, 0.2, 0.5));
    emitterList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    AddControl(emitterList);
    
    propList = new UIList(Rect(0, cellH*9, buttonW, GetScreenHeight() - cellH*9), UIList::ORIENTATION_VERTICAL);
    propList->SetDelegate(this);
    propList->GetBackground()->SetColor(Color(0.4, 0.4, 0.4, 0.5));
    propList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    AddControl(propList);
    
    addPropList = new UIList(Rect(buttonW, cellH*2, buttonW, GetScreenHeight() - cellH*2), UIList::ORIENTATION_VERTICAL);
    addPropList->SetDelegate(this);
    addPropList->GetBackground()->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addPropList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    AddControl(addPropList);
    
    forcesList = new UIList(Rect(buttonW, cellH*2, buttonW, cellH*5), UIList::ORIENTATION_VERTICAL);
    forcesList->SetDelegate(this);
    forcesList->GetBackground()->SetColor(Color(0.3, 0.3, 0.3, 0.5));
    forcesList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    AddControl(forcesList);
    
    OKBut = new UIButton(Rect(buttonW, cellH, buttonW/2, cellH));
    OKBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    OKBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    OKBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    OKBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    OKBut->SetStateFont(UIControl::STATE_NORMAL, f);
    OKBut->SetStateText(UIControl::STATE_NORMAL, L"OK");
	OKBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(OKBut);
    
    cancelBut = new UIButton(Rect(buttonW*3/2, cellH, buttonW/2, cellH));
    cancelBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    cancelBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    cancelBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    cancelBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    cancelBut->SetStateFont(UIControl::STATE_NORMAL, f);
    cancelBut->SetStateText(UIControl::STATE_NORMAL, L"Cancel");
	cancelBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(cancelBut);

    vSliders[0] = new UISlider(Rect(buttonW, cellH*19/2, buttonW, cellH/2));
    vSliders[1] = new UISlider(Rect(buttonW, cellH*21/2, buttonW, cellH/2));
    vSliders[2] = new UISlider(Rect(buttonW, cellH*23/2, buttonW, cellH/2));
    vSliders[3] = new UISlider(Rect(buttonW, cellH*25/2, buttonW, cellH/2));
    
    valueText[0] = new UIStaticText(Rect(buttonW*4/3, cellH*9, buttonW/2, cellH/2));
    valueText[1] = new UIStaticText(Rect(buttonW*4/3, cellH*10, buttonW/2, cellH/2));
    valueText[2] = new UIStaticText(Rect(buttonW*4/3, cellH*11, buttonW/2, cellH/2));
    valueText[3] = new UIStaticText(Rect(buttonW*4/3, cellH*12, buttonW/2, cellH/2));

    for(int i = 0; i < 4; i++)
    {
        vSliders[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        vSliders[i]->GetBackground()->SetColor(Color(0.85, 0.85, 0.85, 0.45));
        vSliders[i]->GetThumb()->size = Vector2(cellH/4, cellH/2);
        vSliders[i]->GetThumb()->pivotPoint = vSliders[i]->GetThumb()->size/2;
        vSliders[i]->GetThumb()->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        vSliders[i]->GetThumb()->GetBackground()->SetColor(Color(1.0, 1.0, 1.0, 0.85));
        vSliders[i]->SetEventsContinuos(false);
        vSliders[i]->SetValue(0);
        vSliders[i]->AddEvent(UIControl::EVENT_VALUE_CHANGED, Message(this, &TestScreen::SliderChanged));
        
        valueText[i]->SetFont(cellFont);
        valueText[i]->SetAlign(DAVA::ALIGN_LEFT);
        
        AddControl(valueText[i]);
        AddControl(vSliders[i]);
    }
    
    propEdit[0] = new PropertyLineEditControl();
    propEdit[0]->SetRect(Rect(buttonW, cellH*9, buttonW, cellH*3));
    propEdit[1] = new PropertyLineEditControl();
    propEdit[1]->SetRect(Rect(buttonW, cellH*12, buttonW, cellH*3));
    propEdit[2] = new PropertyLineEditControl();
    propEdit[2]->SetRect(Rect(buttonW, cellH*15, buttonW, cellH*3));
    propEdit[3] = new PropertyLineEditControl();
    propEdit[3]->SetRect(Rect(buttonW, cellH*18, buttonW, cellH*3));
    for(int i = 0; i < 4; i++)
    {
        propEdit[i]->SetDelegate(this);
        AddControl(propEdit[i]);
    }
    
    fsDlg = new FileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlg->SetDelegate(this);
    Vector<String> filter;
    filter.push_back(".yaml");
    filter.push_back(".YAML");
    fsDlg->SetExtensionFilter(filter);
    fsDlg->SetTitle(L"Loading from .yaml file");
    
    fsDlgSprite = new FileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlgSprite->SetDelegate(this);
    Vector<String> filter2;
    fsDlgSprite->SetTitle(L"Selecte Sprite");
    
    spritePanel = new UIControl(Rect(buttonW, cellH*8, buttonW, buttonW + cellH));
    spritePanel->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    spritePanel->GetBackground()->SetColor(Color(0.0f, 0.0f, 0.0f, 0.25f));
    AddControl(spritePanel);
    
    spriteControl = new UIControl(Rect(0, 0, spritePanel->GetSize().x, spritePanel->GetSize().y));
    spriteControl->SetSpriteDrawType(UIControlBackground::DRAW_ALIGNED);
    spriteControl->SetSpriteAlign(DAVA::ALIGN_BOTTOM);
    spritePanel->AddControl(spriteControl);
    
    spriteSelect = new UIButton(Rect(0, 0, cellH*2, cellH));
    spriteSelect->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    spriteSelect->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    spriteSelect->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    spriteSelect->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    spriteSelect->SetStateFont(UIControl::STATE_NORMAL, f);
    spriteSelect->SetStateText(UIControl::STATE_NORMAL, L"...");
	spriteSelect->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    spritePanel->AddControl(spriteSelect);
    
    spriteInfo = new UIStaticText(Rect(cellH*2, 0, spritePanel->GetSize().x - cellH*2, cellH));
    spriteInfo->SetFont(cellFont);
    spritePanel->AddControl(spriteInfo);
    
    emitter = new ParticleEmitter();
    layers.push_back(Layer(emitterProps, emitterPropsCount, defSpriteFile));
    
    preview = new PreviewControl(emitter);
    preview->SetRect(Rect(buttonW*2, 0, GetScreenWidth() - buttonW*2, GetScreenHeight()));
    AddControl(preview);
    
    forcePreview = new ForcePreviewControl();
    forcePreview->SetRect(Rect(buttonW*2, cellH*2, buttonW, buttonW));
    forcePreview->SetValue(Vector2(0, 0));
    AddControl(forcePreview);
    
    HideAndResetEditFields();
    HideForcesList();
    HideAddProps();
}

void TestScreen::SliderChanged(BaseObject *obj, void *data, void *callerData)
{
    for(int i = 0; i < 4; i++)
    {
        if(obj == vSliders[i])
        {
            if(selectedEmitterElement == 0)
            {
                SetEmitterPropValue(selectedPropElement);
                GetEmitterPropValue(selectedPropElement);
            }
            if(selectedEmitterElement > 0)
            {
                SetLayerPropValue(selectedPropElement);
                GetLayerPropValue(selectedPropElement);
            }
        }
        if(selectedPropElement == 11)
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->GetForces()[selectedForceElement].Get()->GetValue(curPropEditTime));
        if(selectedPropElement == 12)
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->GetForcesVariation()[selectedForceElement].Get()->GetValue(curPropEditTime));
    }
}

void TestScreen::ButtonPressed(BaseObject *obj, void *data, void *callerData)
{
    if(obj == newEmitter)
    {
        selectedEmitterElement = -1;
        selectedPropElement = -1;
        selectedForceElement = -1;
        forcePreview->SetValue(Vector2(0, 0));
        emitter = new ParticleEmitter();
        preview->SetEmitter(emitter);
        layers.clear();
        layers.push_back(Layer(emitterProps, emitterPropsCount, defSpriteFile));
        emitterList->RefreshList();
        propList->RefreshList();        
        HideAndResetEditFields();
    }
    if(obj == loadEmitter)
    {
        fsDlg->SetOperationType(FileSystemDialog::OPERATION_LOAD);
        fsDlg->Show(this);
    }
    if(obj == saveEmitter)
    {
        fsDlg->SetOperationType(FileSystemDialog::OPERATION_SAVE);
        fsDlg->Show(this);
    }
    if(obj == addLayer)
    {
        layers.push_back(Layer(layerProps, layerPropsCount, defSpriteFile));
        ParticleLayer *layer = new ParticleLayer();
        layer->SetSprite(Sprite::Create(defSpriteFile));
        emitter->AddLayer(layer);
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == delLayer)
    {
        if(selectedEmitterElement > 0)
        {
            layers.erase(layers.begin()+selectedEmitterElement);
            emitter->GetLayers().erase(emitter->GetLayers().begin()+selectedEmitterElement-1);
            selectedEmitterElement = -1;
        }
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == addProp)
    {
        if(selectedEmitterElement >= 0)
        {
            ShowAddProps();
            addPropList->RefreshList();
            
            HideAndResetEditFields();
            HideForcesList();
            
            for(int i = 0; i < 4; i++)
                propEdit[i]->Reset();
        }
    }
    if(obj == delProp)
    {
        if(selectedEmitterElement >= 0 && selectedPropElement >= 0)
            layers.at(selectedEmitterElement).props.at(selectedPropElement).isDefault = true;
        if(selectedEmitterElement == 0)
        {
            ResetEmitterPropValue(selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {
            ResetLayerPropValue(selectedPropElement);
        }
        deltaIndex = 0;
        propList->RefreshList();
        HideAndResetEditFields();
        HideForcesList();
    }
    if(obj == OKBut)
    {
        layers.at(selectedEmitterElement).props.at(selectedAddPropElement).isDefault = false;
        deltaIndex = 0;
        propList->RefreshList();
        HideAddProps();
        HideAndResetEditFields();
        HideForcesList();
    }
    if(obj == cancelBut)
    {
        HideAddProps();
        HideAndResetEditFields();
        HideForcesList();
    }
    if(obj == spriteSelect)
    {
        fsDlgSprite->Show(this);
    }
    if(obj == valueBut)
    {
        HideAndResetEditFields();
        HideForcesList();
        curPropType = false;
        if(selectedEmitterElement == 0)
        {
            SetEmitterPropValue(selectedPropElement, 1);
            GetEmitterPropValue(selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {   
            SetLayerPropValue(selectedPropElement, 1);
            GetLayerPropValue(selectedPropElement);
        }
    }
    if(obj == KFBut)
    {
        HideAndResetEditFields();
        HideForcesList();
        curPropType = true;
        if(selectedEmitterElement == 0)
        {
            SetEmitterPropValue(selectedPropElement, 1);
            GetEmitterPropValue(selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {   
            SetLayerPropValue(selectedPropElement, 1);
            GetLayerPropValue(selectedPropElement);
        }
    }
    if(obj == addForce)
    {
        if(selectedPropElement == 11)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().push_back(RefPtr<PropertyLine<Vector2> >(0));
        }
        if(selectedPropElement == 12)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().push_back(RefPtr<PropertyLine<Vector2> >(0));
        }
        if(selectedPropElement == 13)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().push_back(RefPtr<PropertyLine<float32> >(0));
        }
        HideAndResetEditFields();
        selectedForceElement = -1;
        forcePreview->SetValue(Vector2(0, 0));
        forcesList->RefreshList();
    }
    if(obj == delForce)
    {
        if(selectedForceElement >= 0)
        {
            if(selectedPropElement == 11)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().erase(emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().begin() + selectedForceElement);
            }
            if(selectedPropElement == 12)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().erase(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().begin() + selectedForceElement);
            }
            if(selectedPropElement == 13)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().erase(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().begin() + selectedForceElement);
            }
            forcesList->RefreshList();
            selectedForceElement = -1;
            forcePreview->SetValue(Vector2(0, 0));
        }
    }
}

bool TestScreen::GetProp(PropertyLineValue<float32> *pv)
{
    if(pv)
    {
        vSliders[0]->SetVisible(true);
        valueText[0]->SetVisible(true);
        vSliders[0]->SetValue(pv->GetValue(0));
        valueText[0]->SetText(StringToWString(Format("X: %f", pv->GetValue(0))));
    }
    else return false;
    curPropType = false;
    return true;
}
bool TestScreen::GetProp(PropertyLineKeyframes<float32> *pk)
{
    if(pk)
    {
        propEdit[0]->SetVisible(true);
        propEdit[0]->GetValues().clear();
        for(int i = 0; i < pk->keys.size(); i++)
        {
            propEdit[0]->GetValues().push_back(PropertyLineEditControl::PropertyRect(pk->keys.at(i).t, pk-> keys.at(i).value));
        }
        propEdit[0]->SetText(L"X");
    }
    else return false;
    curPropType = true;
    return true;
}
bool TestScreen::GetProp(PropertyLineValue<Vector2> *vv)
{
    if(vv)
    {
        for(int i = 0; i < 2; i++)
        {
            vSliders[i]->SetVisible(true);
            valueText[i]->SetVisible(true);
        }
        vSliders[0]->SetValue(vv->GetValue(0).x);
        vSliders[1]->SetValue(vv->GetValue(0).y);
        valueText[0]->SetText(StringToWString(Format("X: %f", vv->GetValue(0).x)));
        valueText[1]->SetText(StringToWString(Format("Y: %f", vv->GetValue(0).y)));
    }
    else return false;
    curPropType = false;
    return true;
}
bool TestScreen::GetProp(PropertyLineKeyframes<Vector2> *vk)
{
    if(vk)
    {
        for(int i = 0; i < 2; i++)
        {
            propEdit[i]->SetVisible(true);
        }
        propEdit[0]->GetValues().clear();
        propEdit[1]->GetValues().clear();
        for(int i = 0; i < vk->keys.size(); i++)
        {
            propEdit[0]->GetValues().push_back(PropertyLineEditControl::PropertyRect(vk->keys.at(i).t, vk->keys.at(i).value.x));
            propEdit[1]->GetValues().push_back(PropertyLineEditControl::PropertyRect(vk->keys.at(i).t, vk->keys.at(i).value.y));
        }
        propEdit[0]->SetText(L"X");
        propEdit[1]->SetText(L"Y");
    }
    else return false;
    curPropType = true;
    return true;
}
bool TestScreen::GetProp(PropertyLineValue<Color> *cv)
{
    if(cv)
    {
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetVisible(true);
            valueText[i]->SetVisible(true);
        }
        vSliders[3]->SetValue(cv->GetValue(0).a);
        vSliders[2]->SetValue(cv->GetValue(0).b);
        vSliders[1]->SetValue(cv->GetValue(0).g);
        vSliders[0]->SetValue(cv->GetValue(0).r);
        valueText[0]->SetText(StringToWString(Format("R: %f", cv->GetValue(0).r)));
        valueText[1]->SetText(StringToWString(Format("G: %f", cv->GetValue(0).g)));
        valueText[2]->SetText(StringToWString(Format("B: %f", cv->GetValue(0).b)));
        valueText[3]->SetText(StringToWString(Format("A: %f", cv->GetValue(0).a)));
    }
    else return false;
    curPropType = false;
    return true;  
}
bool TestScreen::GetProp(PropertyLineKeyframes<Color> *ck)
{
    if(ck)
    {
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->SetVisible(true);
        }
        propEdit[0]->GetValues().clear();
        propEdit[1]->GetValues().clear();
        propEdit[2]->GetValues().clear();
        propEdit[3]->GetValues().clear();
        for(int i = 0; i < ck->keys.size(); i++)
        {
            propEdit[0]->GetValues().push_back(PropertyLineEditControl::PropertyRect(ck->keys.at(i).t, ck->keys.at(i).value.r));
            propEdit[1]->GetValues().push_back(PropertyLineEditControl::PropertyRect(ck->keys.at(i).t, ck->keys.at(i).value.g));
            propEdit[2]->GetValues().push_back(PropertyLineEditControl::PropertyRect(ck->keys.at(i).t, ck->keys.at(i).value.b));
            propEdit[3]->GetValues().push_back(PropertyLineEditControl::PropertyRect(ck->keys.at(i).t, ck->keys.at(i).value.a));
        }
        propEdit[0]->SetText(L"R");
        propEdit[1]->SetText(L"G");
        propEdit[2]->SetText(L"B");
        propEdit[3]->SetText(L"A");
    }
    else return false;
    curPropType = true;
    return true;
}

void TestScreen::GetEmitterPropValue(int32 id)
{
    valueBut->SetVisible(true);
    KFBut->SetVisible(true);
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    //PropertyLineValue<Vector2> *vv;
    //PropertyLineKeyframes<Vector2> *vk;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    switch (id) {
        case 0:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetEmissionAngle().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetEmissionAngle().Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 1:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetEmissionRange().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetEmissionRange().Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 2:
            pk = dynamic_cast< PropertyLineKeyframes<float> *>(emitter->GetRadius().Get());
            pv = dynamic_cast< PropertyLineValue<float> *>(emitter->GetRadius().Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 3:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetColorOverLife().Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetColorOverLife().Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(ck))
                if(!GetProp(cv))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        default:
            break;
    }
}

void TestScreen::SetEmitterPropValue(int32 id, bool def)
{
    float32 value[4];
    if(def)
    {
        memset(&value, 0, sizeof(float32)*4);
    }
    else
    {
        for(int i = 0; i < 4; i++)
        {
            value[i] = vSliders[i]->GetValue();
        }
    }
    PropertyLine<float32> *valueDim1;
    PropertyLine<Vector2> *valueDim2;
    PropertyLine<Color> *valueDim4;
    if(!curPropType)
    {
        valueDim1 = new PropertyLineValue<float32>(value[0]);
        valueDim2 = new PropertyLineValue<Vector2>(Vector2(value[0],value[1]));
        valueDim4 = new PropertyLineValue<Color>(Color(value[0], value[1], value[2], value[3]));
    }
    else
    {
        PropertyLineKeyframes<float32> *value1 = new PropertyLineKeyframes<float32>();
        PropertyLineKeyframes<Vector2> *value2 = new PropertyLineKeyframes<Vector2>();
        PropertyLineKeyframes<Color> *value4 = new PropertyLineKeyframes<Color>();
    
        PropertyLineKeyframes<float32> *KFValues[4];
        for(int i = 0; i < 4; i++)
        {
            KFValues[i] = propEdit[i]->GetPropertyLine();
        }
        
        for(int32 i = 0; i < KFValues[activePropEdit]->keys.size(); i++)
        {
            float32 t = KFValues[activePropEdit]->keys.at(i).t;
            value1->AddValue(t, KFValues[0]->GetValue(t));
            value2->AddValue(t, Vector2(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t)));
            value4->AddValue(t, Color(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t), KFValues[3]->GetValue(t)));
        }
        
        valueDim1 = value1;
        valueDim2 = value2;
        valueDim4 = value4;
    }
    switch (id) {
        case 0:
            emitter->GetEmissionAngle().Set(valueDim1);
            break;
            
        case 1:
            emitter->GetEmissionRange().Set(valueDim1);      
            break;
           
        case 2:
            emitter->GetRadius().Set(valueDim1);
            break;
            
        case 3:
            emitter->GetColorOverLife().Set(valueDim4);
            break;
            
        default:
            break;
    }    
}

void TestScreen::ResetEmitterPropValue(int32 id)
{
    switch (id) {
        case 0:
            emitter->GetEmissionAngle().Set(0);
            break;
            
        case 1:
            emitter->GetEmissionRange().Set(0);      
            break;
            
        case 2:
            emitter->GetRadius().Set(0);
            break;
            
        case 3:
            emitter->GetColorOverLife().Set(0);
            break;
            
        default:
            break;
    }        
}

void TestScreen::ResetLayerPropValue(int32 id)
{
    switch (id) {
        case 0:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(Sprite::Create(defSpriteFile));
            break;
        case 1:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetLife().Set(0);
            break;
            
        case 2:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetLifeVariation().Set(0);       
            break;
            
        case 3:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetNumber().Set(0);     
            break;
            
        case 4:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetNumberVariation().Set(0);
            break;
            
        case 5:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSize().Set(0);
            break;
            
        case 6:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeVariation().Set(0);      
            break;
            
        case 7:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeOverLife().Set(0);     
            break;
            
        case 8:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocity().Set(0);      
            break;
            
        case 9:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityVariation().Set(0);      
            break;
            
        case 10:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityOverLife().Set(0);     
            break;
            
        case 11:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().clear();
            break;
            
        case 12:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().clear();
            break;
            
        case 13:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().clear();
            break;
            
        case 14:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpin().Set(0);    
            break;
            
        case 15:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinVariation().Set(0);
            break;
            
        case 16:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinOverLife().Set(0);     
            break;
            
        case 17:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandom().Set(0);    
            break;
            
        case 18:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomVariation().Set(0);     
            break;
            
        case 19:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomOverLife().Set(0);      
            break;
            
        case 20:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetBounce().Set(0);       
            break;
            
        case 21:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceVariation().Set(0);      
            break;
            
        case 22:emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceOverLife().Set(0);    
            break;
            
        case 23:emitter->GetLayers().at(selectedEmitterElement-1)->GetColorRandom().Set(0);      
            break;
            
        case 24:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetAlphaOverLife().Set(0);      
            break;
            
        case 25:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetColorOverLife().Set(0);  
            break;
            
        default:
            break;
    }    

}

void TestScreen::GetLayerPropValue(int32 id)
{
    if(id > 0)
    {
        valueBut->SetVisible(true);
        KFBut->SetVisible(true);
    }
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector2> *vv;
    PropertyLineKeyframes<Vector2> *vk;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    switch (id) {
        case 0:
            sprite = emitter->GetLayers().at(selectedEmitterElement-1)->GetSprite();
            spriteControl->SetSprite(sprite, 0);
            if(sprite)
                layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            spritePanel->SetVisible(true);
            spriteInfo->SetText(StringToWString(Format("%.0fx%.0f@%d", sprite->GetWidth(), sprite->GetHeight(), sprite->GetFrameCount())));
            break;
            
        case 1:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 2:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetLifeVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetLifeVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 3:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetNumber().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetNumber().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;   
            break;
            
        case 4:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetNumberVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetNumberVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 5:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSize().Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSize().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(vk))
                if(!GetProp(vv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true; 
            break;
            
        case 6:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeVariation().Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(vk))
                if(!GetProp(vv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 7:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;    
            break;
            
        case 8:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocity().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocity().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;     
            break;
            
        case 9:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;    
            break;
            
        case 10:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 11:
            
        case 12:
            
        case 13:
            if(selectedForceElement >= 0)
                GetForcesValue(selectedForceElement);
            break;
            
        case 14:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpin().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpin().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;   
            break;
            
        case 15:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 16:
            
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;  
            break;
            
        case 17:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandom().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandom().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 18:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 19:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 20:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounce().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounce().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 21:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceVariation().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceVariation().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 22:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 23:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetColorRandom().Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetColorRandom().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(ck))
                if(!GetProp(cv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 24:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetAlphaOverLife().Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetAlphaOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk))
                if(!GetProp(pv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 25:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetColorOverLife().Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetColorOverLife().Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(ck))
                if(!GetProp(cv))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        default:
            break;
    }
}

void TestScreen::SetLayerPropValue(int32 id, bool def)
{
    float32 value[4];
    if(def)
    {
        memset(&value, 0, sizeof(float32)*4);
    }
    else
    {
        for(int i = 0; i < 4; i++)
        {
            value[i] = vSliders[i]->GetValue();
        }
    }
    PropertyLine<float32> *valueDim1;
    PropertyLine<Vector2> *valueDim2;
    PropertyLine<Color> *valueDim4;
    if(!curPropType)
    {
        valueDim1 = new PropertyLineValue<float32>(value[0]);
        valueDim2 = new PropertyLineValue<Vector2>(Vector2(value[0],value[1]));
        valueDim4 = new PropertyLineValue<Color>(Color(value[0], value[1], value[2], value[3]));
    }
    else
    {
        PropertyLineKeyframes<float32> *value1 = new PropertyLineKeyframes<float32>();
        PropertyLineKeyframes<Vector2> *value2 = new PropertyLineKeyframes<Vector2>();
        PropertyLineKeyframes<Color> *value4 = new PropertyLineKeyframes<Color>();
        
        PropertyLineKeyframes<float32> *KFValues[4];
        for(int i = 0; i < 4; i++)
        {
            KFValues[i] = propEdit[i]->GetPropertyLine();
        }
        
        for(int32 i = 0; i < KFValues[activePropEdit]->keys.size(); i++)
        {
            float32 t = KFValues[activePropEdit]->keys.at(i).t;
            value1->AddValue(t, KFValues[0]->GetValue(t));
            value2->AddValue(t, Vector2(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t)));
            value4->AddValue(t, Color(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t), KFValues[3]->GetValue(t)));
        }
        
        valueDim1 = value1;
        valueDim2 = value2;
        valueDim4 = value4;
    }
    switch (id) {
        case 0:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(sprite);
            break;
            
        case 1:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetLife().Set(valueDim1);
            break;
            
        case 2:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetLifeVariation().Set(valueDim1);       
            break;
            
        case 3:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetNumber().Set(valueDim1);     
            break;
            
        case 4:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetNumberVariation().Set(valueDim1);
            break;
            
        case 5:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSize().Set(valueDim2);
            break;
            
        case 6:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeVariation().Set(valueDim2);      
            break;
            
        case 7:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSizeOverLife().Set(valueDim1);     
            break;
            
        case 8:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocity().Set(valueDim1);      
            break;
            
        case 9:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityVariation().Set(valueDim1);      
            break;
            
        case 10:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetVelocityOverLife().Set(valueDim1);     
            break;
            
        case 11:
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().at(selectedForceElement).Set(valueDim2);
            break;
            
        case 12:
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().at(selectedForceElement).Set(valueDim2);
            break;
            
        case 13:
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().at(selectedForceElement).Set(valueDim1);
            break;
            
        case 14:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpin().Set(valueDim1);    
            break;
            
        case 15:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinVariation().Set(valueDim1);
            break;
            
        case 16:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetSpinOverLife().Set(valueDim1);     
            break;
            
        case 17:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandom().Set(valueDim1);    
            break;
            
        case 18:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomVariation().Set(valueDim1);     
            break;
            
        case 19:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetMotionRandomOverLife().Set(valueDim1);      
            break;
            
        case 20:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetBounce().Set(valueDim1);       
            break;
            
        case 21:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceVariation().Set(valueDim1);      
            break;
            
        case 22:emitter->GetLayers().at(selectedEmitterElement-1)->GetBounceOverLife().Set(valueDim1);    
            break;
            
        case 23:emitter->GetLayers().at(selectedEmitterElement-1)->GetColorRandom().Set(valueDim4);      
            break;
            
        case 24:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetAlphaOverLife().Set(valueDim1);      
            break;
            
        case 25:
            emitter->GetLayers().at(selectedEmitterElement-1)->GetColorOverLife().Set(valueDim4);  
            break;
            
        default:
            break;
    }
    
}

void TestScreen::GetForcesValue(int32 id)
{
    ShowForcesList();
    if (selectedPropElement == 11)
    {
        PropertyLineKeyframes<Vector2> *vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().at(selectedForceElement).Get());
        PropertyLineValue<Vector2> *vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().at(selectedForceElement).Get());
        if(vv)
        {
            GetProp(vv);
            forcePreview->SetValue(vv->GetValue(0));
        }
        else if(vk)
        {
            GetProp(vk);
            forcePreview->SetValue(vk->GetValue(0));
        }    
    }
    if (selectedPropElement == 12)
    {
        PropertyLineKeyframes<Vector2> *vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().at(selectedForceElement).Get());
        PropertyLineValue<Vector2> *vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().at(selectedForceElement).Get());
        if(vv)
        {
            GetProp(vv);
            forcePreview->SetValue(vv->GetValue(0));
        }
        else if(vk)
        {
            GetProp(vk);
            forcePreview->SetValue(vk->GetValue(0));
        }   
    }
    if (selectedPropElement == 13)
    {
        PropertyLineKeyframes<float32> *vk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().at(selectedForceElement).Get());
        PropertyLineValue<float32> *vv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().at(selectedForceElement).Get());
        if(vv)
        {
            GetProp(vv);
        }
        else if(vk)
        {
            GetProp(vk);
        }
    }
}

void TestScreen::OnPointAdd(PropertyLineEditControl *forControl, float32 t, float32 value)
{
    for(int32 i = 0; i < 4; i++)
    {
        if(forControl == propEdit[i])
        {
            activePropEdit = i;
            propEdit[i]->AddPoint(t, value);
        }
        else
            propEdit[i]->AddPoint(t, propEdit[i]->GetPropertyLine()->GetValue(t));
    }
    if(selectedEmitterElement == 0)
    {
        SetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue(selectedPropElement);
    }
    if(selectedEmitterElement == 0)
    {
        GetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        GetLayerPropValue(selectedPropElement);
    }
}

void TestScreen::OnPointDelete(PropertyLineEditControl *forControl, float32 t)
{
    for(int i = 0; i < 4; i++)
    {
        if(forControl == propEdit[i])
            activePropEdit = i;
        propEdit[i]->DeletePoint(t);
    }
    if(selectedEmitterElement == 0)
    {
        SetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue(selectedPropElement);
    }
    if(selectedEmitterElement == 0)
    {
        GetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        GetLayerPropValue(selectedPropElement);
    }
    
}

void TestScreen::OnPointMove(PropertyLineEditControl *forControl, float32 lastT, float32 newT, float32 newV)
{
    for(int i = 0; i < 4; i++)
    {
        if(forControl == propEdit[i])
        {
            propEdit[i]->MovePoint(lastT, newT, newV, true);
        }
        else
        {
            propEdit[i]->MovePoint(lastT, newT, newV, false);
        }
    }
    if(selectedEmitterElement == 0)
    {
        SetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue(selectedPropElement);
    }
    if(selectedEmitterElement == 0)
    {
        GetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        GetLayerPropValue(selectedPropElement);
    }
}

void TestScreen::OnMouseMove(float32 t)
{
    curPropEditTime = t;
    if(selectedPropElement == 11)
        forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->GetForces()[selectedForceElement].Get()->GetValue(curPropEditTime));
    if(selectedPropElement == 12)
        forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->GetForcesVariation()[selectedForceElement].Get()->GetValue(curPropEditTime));
        
}
void TestScreen::OnFileSelected(FileSystemDialog *forDialog, const String &pathToFile)
{
    if(forDialog == fsDlg)
    {
        if(forDialog->GetOperationType() == FileSystemDialog::OPERATION_LOAD)
        {
            selectedEmitterElement = -1;
            selectedPropElement = -1;
            selectedForceElement = -1;
            HideAndResetEditFields();
            forcePreview->SetValue(Vector2(0, 0));
            emitter = new ParticleEmitter();
            emitter->LoadFromYaml(pathToFile);
            preview->SetEmitter(emitter);
            layers.clear();
            layers.push_back(Layer(emitterProps, emitterPropsCount, defSpriteFile));
            for(int i = 0; i < emitter->GetLayers().size(); i++)
            {
                String spritePath = emitter->GetLayers()[i]->GetSprite()->GetName();
                layers.push_back(Layer(layerProps, layerPropsCount, spritePath.substr(0, spritePath.size()-4)));
            }
            emitterList->RefreshList();
            propList->RefreshList();
        }
        if(forDialog->GetOperationType() == FileSystemDialog::OPERATION_SAVE)
        {
            SaveToYaml(pathToFile);
        }
    }
    if(forDialog == fsDlgSprite)
    {
        String path = "~res:" + pathToFile.substr(pathToFile.rfind("/Data") + 5);
        path = path.substr(0, path.size() - 4);

        layers[selectedEmitterElement].spritePath = path;
        
        sprite = Sprite::Create(path);
        SetLayerPropValue(0);
        GetLayerPropValue(0);
    }
}

void TestScreen::OnFileSytemDialogCanceled(FileSystemDialog *forDialog)
{
    
}

void TestScreen::HideForcesList()
{
    forcesList->SetVisible(false);
    addForce->SetVisible(false);
    delForce->SetVisible(false);
    forcePreview->SetVisible(false);
}

void TestScreen::ShowForcesList()
{
    forcesList->SetVisible(true);
    addForce->SetVisible(true);
    delForce->SetVisible(true);
    forcesList->RefreshList();
    forcePreview->SetVisible(true);
}

void TestScreen::HideAndResetEditFields()
{
    for(int i = 0; i < 4; i++)
    {
        propEdit[i]->SetVisible(false);
        vSliders[i]->SetVisible(false);
        valueText[i]->SetVisible(false);
        vSliders[i]->SetValue(0.0f);
        propEdit[i]->Reset();
    }
    KFBut->SetVisible(false);
    valueBut->SetVisible(false);
    
    spritePanel->SetVisible(false);
}

void TestScreen::ShowAddProps()
{
    AddControl(addPropList);
    AddControl(OKBut);
    AddControl(cancelBut);
    emitterList->SetDisabled(true);
    propList->SetDisabled(true);
    loadEmitter->SetDisabled(true);
    saveEmitter->SetDisabled(true);
    addLayer->SetDisabled(true);
    delLayer->SetDisabled(true);
    addProp->SetDisabled(true);
    delProp->SetDisabled(true);
}

void TestScreen::HideAddProps()
{
    RemoveControl(addPropList);
    RemoveControl(OKBut);
    RemoveControl(cancelBut);
    emitterList->SetDisabled(false);
    propList->SetDisabled(false);
    loadEmitter->SetDisabled(false);
    saveEmitter->SetDisabled(false);
    addLayer->SetDisabled(false);
    delLayer->SetDisabled(false);
    addProp->SetDisabled(false);
    delProp->SetDisabled(false);
}

void TestScreen::UnloadResources()
{
    SafeRelease(loadEmitter);
    SafeRelease(saveEmitter);
    SafeRelease(newEmitter);
    SafeRelease(addLayer);
    SafeRelease(delLayer);
    SafeRelease(addProp);
    SafeRelease(delProp);
    SafeRelease(OKBut);
    SafeRelease(cancelBut);
    SafeRelease(KFBut);
    SafeRelease(valueBut);
    SafeRelease(spriteSelect);
    SafeRelease(addForce);
    SafeRelease(delForce);
    
    for(int i = 0 ;i < 4; i++)
    {
        SafeRelease(valueText[i]);
        SafeRelease(vSliders[i]);
        SafeRelease(propEdit[i]);
    }
    
    SafeRelease(spriteInfo);
    
    SafeRelease(emitterList);
    SafeRelease(propList);
    SafeRelease(addPropList);
    SafeRelease(forcesList);
    
    SafeRelease(fsDlg);
    SafeRelease(fsDlgSprite);
    
    SafeRelease(spritePanel);
    SafeRelease(spriteControl);
    
    SafeRelease(sprite);
    
    SafeRelease(preview);
    
    SafeRelease(forcePreview);
    
    SafeRelease(cellFont);
    SafeRelease(f);
    
    SafeRelease(emitter);    
}

int32 TestScreen::ElementsCount(UIList *forList)
{
    int32 n = 0;
    if(forList == emitterList)
    {
        n = layers.size();
    }
    if(forList == propList)
    {
        if(selectedEmitterElement == 0)
        {
            for(int i = 0; i < emitterPropsCount; i++)
            {
                if(!layers.at(selectedEmitterElement).props.at(i).isDefault)
                {
                    n++;
                }
            }
        }
        else if(selectedEmitterElement > 0)
        {
            for(int i = 0; i < layerPropsCount; i++)
            {
                if(!layers.at(selectedEmitterElement).props.at(i).isDefault)
                {
                    n++;
                }
            }
        }
    }
    if(forList == addPropList)
    {
        if(selectedEmitterElement == 0)
        {
            n = emitterPropsCount;
        }
        else if(selectedEmitterElement > 0)
        {
            n = layerPropsCount;
        }
    }
    if(forList == forcesList)
    {
        switch (selectedPropElement) {
            case 11:
                n = emitter->GetLayers().at(selectedEmitterElement-1)->GetForces().size();
                break;
                
            case 12:
                n = emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesVariation().size();
                break;
                
            case 13:
                n = emitter->GetLayers().at(selectedEmitterElement-1)->GetForcesOverLife().size();
                break;
            default:
                break;
        }
    }
    return n;
}

int32 TestScreen::CellHeight(UIList *forList, int32 index)
{
    return 20;
}

UIListCell *TestScreen::CellAtIndex(UIList *forList, int32 index)
{
    if (forList == emitterList) 
    {
        UIListCell *c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "EmitterList Cell");
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
        
        if(index == 0)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, L"Emitter");
        }
        else
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(Format("Layer %d", index)));
        }
        
        if (index == selectedEmitterElement) 
        {
            c->SetSelected(true);
        }
        else
        {
            c->SetSelected(false);
        }
        return c;
    }
    if (forList == propList)
    {
        PropListCell *c = new PropListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "EmitterList Cell");
        c->SetFront(cellFont);
        int32 n = layers.at(selectedEmitterElement).props.size();
        while((index+deltaIndex < n) && layers.at(selectedEmitterElement).props.at(index+deltaIndex).isDefault)
        {
            deltaIndex++;
        }
        c->SetId(index+deltaIndex);
        if(index+deltaIndex < n)
            c->SetText(StringToWString(layers.at(selectedEmitterElement).props.at(index+deltaIndex).name));
        return c;
    }
    if (forList == addPropList)
    {
        UIListCell *c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "EmitterList Cell");
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
        
        if(selectedEmitterElement == 0)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(emitterProps[index]));
        }
        if(selectedEmitterElement > 0) 
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(layerProps[index]));       
        }
        return c;
    }
    
    if(forList == forcesList)
    {
        UIListCell *c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "Force Cell");
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
        if(selectedPropElement == 11)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(Format("Force %d", index)));
        }
        if(selectedPropElement == 12)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(Format("ForceVariation %d", index)));
        }
        if(selectedPropElement == 13)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(Format("ForceOverLife %d", index)));
        }
        if(selectedForceElement == index)
        {
            c->SetSelected(true);
        }
        return c;
    }
    return 0;
}

void TestScreen::OnCellSelected(UIList *forList, UIListCell *selectedCell)
{
    forList->SetSelected(false);
    
    if(forList == emitterList)
    {
        selectedEmitterElement = selectedCell->GetIndex();
        selectedCell->SetSelected(true);
        propList->RefreshList();
        deltaIndex = 0;
        selectedPropElement = -1;
        selectedForceElement = -1;
        forcePreview->SetValue(Vector2(0, 0));
        
        if(selectedEmitterElement == 0)
        {
            for(int i = 0; i < emitterPropsCount; i++)
            {
                GetEmitterPropValue(i);
            }
        }
        if(selectedEmitterElement > 0)
        {
            for(int i = 0; i < layerPropsCount; i++)
            {
                GetLayerPropValue(i);
            }
        }
        
        HideAndResetEditFields();
        HideForcesList();
    }
    
    if(forList == propList)
    {
        selectedPropElement = ((PropListCell *)selectedCell)->GetId();
        selectedCell->SetSelected(true);
        HideAndResetEditFields();
        HideForcesList();
        selectedForceElement = -1;
        forcePreview->SetValue(Vector2(0, 0));
        if(selectedEmitterElement == 0)
        {
            GetEmitterPropValue(selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {
            GetLayerPropValue(selectedPropElement);
            
            if(selectedPropElement == 11 || selectedPropElement == 12|| selectedPropElement == 13)
            {
                HideAndResetEditFields();
                ShowForcesList();
            }
        }
    }
    
    if(forList == addPropList)
    {
        selectedAddPropElement = selectedCell->GetIndex();
        selectedCell->SetSelected(true);

        static timeval lastTime;
        timeval t;
        gettimeofday(&t, 0);
        if( Abs(t.tv_usec/1000 + t.tv_sec*1000 - lastTime.tv_usec/1000 - lastTime.tv_sec*1000) < dblClickDelay)
        {
            layers.at(selectedEmitterElement).props.at(selectedAddPropElement).isDefault = false;
            deltaIndex = 0;
            propList->RefreshList();
            HideAddProps();
            HideAndResetEditFields();
            HideForcesList();
        }
        gettimeofday(&lastTime, 0);
    }
    
    if(forList == forcesList)
    {
        selectedForceElement = selectedCell->GetIndex();
        selectedCell->SetSelected(true);
        
        HideAndResetEditFields();
        HideForcesList();
        
        GetLayerPropValue(selectedPropElement);
    }
}

void TestScreen::WillAppear()
{
    
}

void TestScreen::WillDisappear()
{
    
}

void TestScreen::Input(UIEvent * event)
{
	if (event->phase == UIEvent::PHASE_KEYCHAR)
		if (event->keyChar == '1')
		{
			//Core::Instance()->ToggleFullscreen();
		}
}

void TestScreen::Update(float32 timeElapsed)
{
    static float32 spriteTime;
    static int32 curSpriteFrame;
    spriteTime += timeElapsed;
    if(spriteTime > 0.5f)
    {
        spriteTime -= 0.5f;
        curSpriteFrame++;
        if(spriteControl->GetSprite() && curSpriteFrame > spriteControl->GetSprite()->GetFrameCount())
        {
            spriteControl->GetSprite()->SetFrame(curSpriteFrame);
        }
    }
}

void TestScreen::Draw(const UIGeometricData &geometricData)
{
    
}

void TestScreen::SaveToYaml(const String &pathToFile)
{
    FILE* file = fopen(pathToFile.c_str(), "w");
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector2> *vv;
    PropertyLineKeyframes<Vector2> *vk;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    
    fprintf(file, "emitter:\n");
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetEmissionAngle().Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetEmissionAngle().Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[0], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[0], pv);
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetEmissionRange().Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetEmissionRange().Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[1], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[1], pv);
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetRadius().Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetRadius().Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[2], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[2], pv);
    
    ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetColorOverLife().Get());
    cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetColorOverLife().Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[3], ck);
    else if(pv)
        PrintPropValue(file, emitterProps[3], cv);
    
    fprintf(file, "\n");
    
    for(int i = 0; i < emitter->GetLayers().size(); i++)
    {
        fprintf(file, "layer%d:\n    type: layer\n", i);
        
        int32 propIndex = 0;
        
        fprintf(file, "    %s: \"%s\"\n", layerProps[propIndex].c_str(), layers[i+1].spritePath.c_str());
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetLifeVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetLifeVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetNumber().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetNumber().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetNumberVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetNumberVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->GetSize().Get());
        vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->GetSize().Get());
        if(vk)
            PrintPropKFValue(file, layerProps[propIndex], vk);
        else if(vv)
            PrintPropValue(file, layerProps[propIndex], vv);
        propIndex++;
        
        vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->GetSizeVariation().Get());
        vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->GetSizeVariation().Get());
        if(vk)
            PrintPropKFValue(file, layerProps[propIndex], vk);
        else if(vv)
            PrintPropValue(file, layerProps[propIndex], vv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetSizeOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetSizeOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetVelocity().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetVelocity().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetVelocityVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetVelocityVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetVelocityOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetVelocityOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        int32 forceCount = emitter->GetLayers()[i]->GetForces().size();
        if(forceCount > 0)
            fprintf(file, "\n    forceCount: %d\n", forceCount);
        
        for(int j = 0; j < forceCount; j++)
        {
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->GetForces()[j].Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->GetForces()[j].Get());
            if(vk)
                PrintPropKFValue(file, Format("force%d", j), vk);
            else if(vv)
                PrintPropValue(file, Format("force%d", j), vv);
            
            
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->GetForcesVariation()[j].Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->GetForcesVariation()[j].Get());
            if(vk)
                PrintPropKFValue(file, Format("forceVariation%d", j), vk);
            else if(vv)
                PrintPropValue(file, Format("forceVariation%d", j), vv);
            
            
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetForcesOverLife()[j].Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetForcesOverLife()[j].Get());
            if(pk)
                PrintPropKFValue(file, Format("force%d", j), pk);
            else if(pv)
                PrintPropValue(file, Format("forceOverLife%d", j), pv);
            
            fprintf(file, "\n");
        }
        propIndex += 3;
        fprintf(file, "\n");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetSpin().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetSpin().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetSpinVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetSpinVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetSpinOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetSpinOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetMotionRandom().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetMotionRandom().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetMotionRandomVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetMotionRandomVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetMotionRandomOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetMotionRandomOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetBounce().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetBounce().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetBounceVariation().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetBounceVariation().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetBounceOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetBounceOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        fprintf(file, "\n");
        
        ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers()[i]->GetColorRandom().Get());
        cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers()[i]->GetColorRandom().Get());
        if(ck)
            PrintPropKFValue(file, layerProps[propIndex], ck);
        else if(cv)
            PrintPropValue(file, layerProps[propIndex], cv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->GetAlphaOverLife().Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->GetAlphaOverLife().Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers()[i]->GetColorOverLife().Get());
        cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers()[i]->GetColorOverLife().Get());
        if(ck)
            PrintPropKFValue(file, layerProps[propIndex], ck);
        else if(cv)
            PrintPropValue(file, layerProps[propIndex], cv);
        propIndex++;
        fprintf(file, "\n");
    }
    
    fclose(file);
}

void TestScreen::PrintPropValue(FILE *file, String propName, PropertyLineValue<float32> *pv)
{
    fprintf(file, "    %s: %f\n", propName.c_str(), pv->GetValue(0));
}

void TestScreen::PrintPropValue(FILE *file, String propName, PropertyLineValue<Vector2> *pv)
{
    Vector2 value = pv->GetValue(0);
    fprintf(file, "    %s: [%f, %f]\n", propName.c_str(), value.x, value.y);
}

void TestScreen::PrintPropValue(FILE *file, String propName, PropertyLineValue<Color> *pv)
{
    Color value = pv->GetValue(0);
    fprintf(file, "    %s: [%f, %f, %f, %f]\n", propName.c_str(), value.r, value.g, value.b, value.a);
}

void TestScreen::PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<float32> *pv)
{
    fprintf(file, "    %s: [", propName.c_str());
    for(int i = 0; i < pv->keys.size(); i++)
    {
        fprintf(file, "%f, %f, ", pv->keys[i].t, pv->keys[i].value);
    }
    fpos_t pos;
    fgetpos(file, &pos);
    pos -= 2;
    fsetpos(file, &pos);
    fprintf(file, "]\n");
}

void TestScreen::PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<Vector2> *pv)
{
    fprintf(file, "    %s: [", propName.c_str());
    for(int i = 0; i < pv->keys.size(); i++)
    {
        fprintf(file, "%f, [%f, %f],", pv->keys[i].t, pv->keys[i].value.x, pv->keys[i].value.y);
    }
    fpos_t pos;
    fgetpos(file, &pos);
    pos -= 2;
    fsetpos(file, &pos);
    fprintf(file, "]\n");
}

void TestScreen::PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<Color> *pv)
{
    fprintf(file, "    %s: [", propName.c_str());
    for(int i = 0; i < pv->keys.size(); i++)
    {
        fprintf(file, "%f, [%f, %f, %f, %f],", pv->keys[i].t, pv->keys[i].value.r, pv->keys[i].value.g, pv->keys[i].value.b, pv->keys[i].value.r);
    }
    fpos_t pos;
    fgetpos(file, &pos);
    pos -= 2;
    fsetpos(file, &pos);
    fprintf(file, "]\n");    
}