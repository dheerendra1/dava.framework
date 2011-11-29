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

TestScreen::TestScreen()
{
    emitterProps.push_back("type");
    emitterProps.push_back("emissionAngle");
    emitterProps.push_back("emissionRange");
    emitterProps.push_back("emissionVector");
    emitterProps.push_back("radius");
    emitterProps.push_back("colorOverLife");
    emitterProps.push_back("size");
    emitterProps.push_back("life");
    
    layerProps.push_back("sprite");
    layerProps.push_back("life");
    layerProps.push_back("lifeVariation");
    layerProps.push_back("number");
    layerProps.push_back("numberVariation");
    layerProps.push_back("size");
    layerProps.push_back("sizeVariation");
    layerProps.push_back("sizeOverLife");
    layerProps.push_back("velocity");
    layerProps.push_back("velocityVariation");
    layerProps.push_back("velocityOverLife");
    layerProps.push_back("forces");
    layerProps.push_back("forcesVariation");
    layerProps.push_back("forcesOverLife");
    layerProps.push_back("spin");
    layerProps.push_back("spinVariation");
    layerProps.push_back("spinOverLife");
    layerProps.push_back("motionRandom");
    layerProps.push_back("motionRandomVariation");
    layerProps.push_back("motionRandomOverLife");
    layerProps.push_back("bounce");
    layerProps.push_back("bounceVariation");
    layerProps.push_back("bounceOverLife");
    layerProps.push_back("colorRandom");
    layerProps.push_back("alphaOverLife");
    layerProps.push_back("colorOverLife");
    
    emitterTypes.push_back("POINT");
    emitterTypes.push_back("LINE");
    emitterTypes.push_back("RECT");
    emitterTypes.push_back("ONCIRCLE");
    
    deltaIndex = 0;
    curPropType = 0; //0 - value, 1 - Keyframed
    dblClickDelay = 500;
    activePropEdit = 0;    
}

void TestScreen::SafeAddControl(UIControl *control)
{
    if(!control->GetParent())
        AddControl(control);
}

void TestScreen::SafeRemoveControl(UIControl *control)
{
    if(control->GetParent())
        RemoveControl(control);
}

void TestScreen::LoadResources()
{
    cellH = GetScreenHeight() / 25;
    buttonW = GetScreenWidth() / 5;
    float32 thumbSliderW = cellH/4;

    sprite = 0;

    cellFont = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    cellFont->SetSize((int32)(cellH/2.5f));
    cellFont->SetColor(Color(1, 1, 1, 1));

    selectedPropElement = -1;
    selectedEmitterElement = -1;
    selectedForceElement = -1;
    selectedEmitterTypeElement = -1;

    f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(18);
    f->SetColor(Color(1,1,1,1));

    preview = new PreviewControl();
    preview->SetRect(Rect(buttonW*2, 0, GetScreenWidth() - buttonW*2, GetScreenHeight()));
    AddControl(preview);
    
    chooseProject = new UIButton(Rect(0, 0, buttonW/2, cellH));
    chooseProject->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    chooseProject->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    chooseProject->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    chooseProject->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    chooseProject->SetStateFont(UIControl::STATE_NORMAL, f);
    chooseProject->SetStateText(UIControl::STATE_NORMAL, LocalizedString("Project"));
	chooseProject->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(chooseProject);
    
    newEmitter = new UIButton(Rect(buttonW/2, 0, buttonW/2, cellH));
    newEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    newEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    newEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    newEmitter->SetStateText(UIControl::STATE_NORMAL, LocalizedString("New"));
	newEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(newEmitter);
    
	loadEmitter = new UIButton(Rect(buttonW, 0, buttonW/2, cellH));
    loadEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    loadEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    loadEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    loadEmitter->SetStateText(UIControl::STATE_NORMAL, LocalizedString("Load"));
	loadEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(loadEmitter);
    
    saveEmitter = new UIButton(Rect(buttonW*3/2, 0, buttonW/2, cellH));
    saveEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    saveEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    saveEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    saveEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    saveEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    saveEmitter->SetStateText(UIControl::STATE_NORMAL, LocalizedString("Save"));
	saveEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(saveEmitter);
    
    addLayer = new UIButton(Rect(0, cellH, buttonW/2, cellH));
    addLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    addLayer->SetStateText(UIControl::STATE_NORMAL, LocalizedString("AddLayer"));
	addLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addLayer);
    
    cloneLayer = new UIButton(Rect(0, cellH*2, buttonW/2, cellH));
    cloneLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    cloneLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    cloneLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    cloneLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    cloneLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    cloneLayer->SetStateText(UIControl::STATE_NORMAL, LocalizedString("CopyLayer"));
	cloneLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(cloneLayer);
    
    disableLayer = new UIButton(Rect(buttonW/2, cellH*2, buttonW/2, cellH));
    disableLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    disableLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    disableLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    disableLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    disableLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    disableLayer->SetStateText(UIControl::STATE_NORMAL, LocalizedString("DELayer"));
	disableLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(disableLayer);
    
    delLayer = new UIButton(Rect(buttonW/2, cellH, buttonW/2, cellH));
    delLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    delLayer->SetStateText(UIControl::STATE_NORMAL, LocalizedString("DelLayer"));
	delLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delLayer);
    
    addProp = new UIButton(Rect(0, cellH*8, buttonW/2, cellH));
    addProp->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addProp->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addProp->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addProp->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addProp->SetStateFont(UIControl::STATE_NORMAL, f);
    addProp->SetStateText(UIControl::STATE_NORMAL, LocalizedString("AddProp"));
	addProp->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addProp);

    delProp = new UIButton(Rect(buttonW/2, cellH*8, buttonW/2, cellH));
    delProp->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delProp->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delProp->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delProp->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delProp->SetStateFont(UIControl::STATE_NORMAL, f);
    delProp->SetStateText(UIControl::STATE_NORMAL, LocalizedString("DelProp"));
	delProp->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delProp);

    valueBut = new UIButton(Rect(buttonW, cellH*8, buttonW/2, cellH));
    valueBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    valueBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    valueBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    valueBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    valueBut->SetStateFont(UIControl::STATE_NORMAL, f);
    valueBut->SetStateText(UIControl::STATE_NORMAL, LocalizedString("Value"));
	valueBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(valueBut);

    KFBut = new UIButton(Rect(buttonW*3/2, cellH*8, buttonW/2, cellH));
    KFBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    KFBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    KFBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    KFBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    KFBut->SetStateFont(UIControl::STATE_NORMAL, f);
    KFBut->SetStateText(UIControl::STATE_NORMAL, LocalizedString("KF"));
	KFBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(KFBut);

    addForce = new UIButton(Rect(buttonW, cellH*7, buttonW/2, cellH));
    addForce->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    addForce->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    addForce->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    addForce->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    addForce->SetStateFont(UIControl::STATE_NORMAL, f);
    addForce->SetStateText(UIControl::STATE_NORMAL, LocalizedString("AddForce"));
	addForce->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(addForce);

    delForce = new UIButton(Rect(buttonW*3/2, cellH*7, buttonW/2, cellH));
    delForce->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    delForce->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    delForce->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    delForce->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    delForce->SetStateFont(UIControl::STATE_NORMAL, f);
    delForce->SetStateText(UIControl::STATE_NORMAL, LocalizedString("DelForce"));
	delForce->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(delForce);

    emitterList = new UIList(Rect(0, cellH*3, buttonW, cellH*5), UIList::ORIENTATION_VERTICAL);
    emitterList->SetDelegate(this);
    emitterList->GetBackground()->SetColor(Color(0.2, 0.2, 0.2, 0.5));
    emitterList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    AddControl(emitterList);

    propList = new UIList(Rect(0, cellH*9, buttonW, GetScreenHeight() - cellH*13), UIList::ORIENTATION_VERTICAL);
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

    emitterTypeList = new UIList(Rect(buttonW, cellH*9, buttonW, cellH*5), UIList::ORIENTATION_VERTICAL);
    emitterTypeList->SetDelegate(this);
    emitterTypeList->GetBackground()->SetColor(Color(0.3, 0.3, 0.3, 0.5));
    emitterTypeList->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    
    emitter3D = new UIButton(Rect(buttonW - cellH*2/3, cellH*3, cellH*2/3, cellH*2/3));
    emitter3D->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    emitter3D->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.5, 0.5, 0.5, 0.0));
    emitter3D->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    emitter3D->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.35, 0.35, 0.35, 1.0));
    emitter3D->SetStateFont(UIControl::STATE_NORMAL, cellFont);
    emitter3D->SetStateText(UIControl::STATE_NORMAL, L"3D");
	emitter3D->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(emitter3D);
    
    OKBut = new UIButton(Rect(buttonW, cellH, buttonW/2, cellH));
    OKBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    OKBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    OKBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    OKBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    OKBut->SetStateFont(UIControl::STATE_NORMAL, f);
    OKBut->SetStateText(UIControl::STATE_NORMAL, LocalizedString("OK"));
	OKBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(OKBut);
    
    cancelBut = new UIButton(Rect(buttonW*3/2, cellH, buttonW/2, cellH));
    cancelBut->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    cancelBut->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    cancelBut->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    cancelBut->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    cancelBut->SetStateFont(UIControl::STATE_NORMAL, f);
    cancelBut->SetStateText(UIControl::STATE_NORMAL, LocalizedString("Cancel"));
	cancelBut->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(cancelBut);

    for(int i = 0; i < 4; i++)
    {
        vSliders[i] = new UISlider(Rect(buttonW*7/6 + thumbSliderW/2, cellH*(9.5f+i), buttonW*2/3 - thumbSliderW, cellH/2));
        vSliders[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        vSliders[i]->GetBackground()->SetColor(Color(0.85, 0.85, 0.85, 0.45));
        vSliders[i]->GetThumb()->size = Vector2(thumbSliderW, thumbSliderW*2);
        vSliders[i]->GetThumb()->pivotPoint = vSliders[i]->GetThumb()->size/2;
        vSliders[i]->GetThumb()->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        vSliders[i]->GetThumb()->GetBackground()->SetColor(Color(1.0, 1.0, 1.0, 0.85));
        vSliders[i]->SetEventsContinuos(false);
        vSliders[i]->SetValue(0);
        vSliders[i]->AddEvent(UIControl::EVENT_VALUE_CHANGED, Message(this, &TestScreen::SliderChanged));
        AddControl(vSliders[i]);
        
        tfValue[i] = new UITextField(Rect(buttonW*11/8, cellH*(9+i), buttonW/4, cellH/2));
        tfValue[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfValue[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfValue[i]->SetFont(cellFont);
        tfValue[i]->SetDelegate(this);
        AddControl(tfValue[i]);
        
        valueText[i] = new UIStaticText(Rect(buttonW*10/8, cellH*(9+i), buttonW/8, cellH/2));
        valueText[i]->SetFont(cellFont);
        valueText[i]->SetAlign(DAVA::ALIGN_LEFT);
        AddControl(valueText[i]);
        
        propEdit[i] = new PropertyLineEditControl();
        propEdit[i]->SetRect(Rect(buttonW, cellH*(9.5f+i*3), buttonW*5/6, cellH*3));
        propEdit[i]->SetDelegate(this);
        AddControl(propEdit[i]);
        
        keysValueTextPos[i] = Rect(buttonW, cellH*(13+i*3), buttonW, cellH/2);
        tfKeysValuePos[i][0] = Rect(buttonW*9/8, cellH*(12.5f+i*3), buttonW/4, cellH/2);
        tfKeysValuePos[i][1] = Rect(buttonW*12/8, cellH*(12.5f+i*3), buttonW/4, cellH/2);
        tfKeysValueTextPos[i][0] = Rect(buttonW, cellH*(12.5f+i*3), buttonW/8, cellH/2);
        tfKeysValueTextPos[i][1] = Rect(buttonW*11/8, cellH*(12.5f+i*3), buttonW/8, cellH/2);
    }
    
    keysValueText = new UIStaticText(keysValueTextPos[0]);
    keysValueText->SetFont(cellFont);
    keysValueText->SetAlign(DAVA::ALIGN_LEFT);
    AddControl(keysValueText);
    
    for(int i = 0; i < 2; i++)
    {
        tfKeysValue[i] = new UITextField(tfKeysValuePos[0][i]);
        tfKeysValue[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfKeysValue[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfKeysValue[i]->SetFont(cellFont);
        tfKeysValue[i]->SetDelegate(this);
        AddControl(tfKeysValue[i]);
        
        tfKeysValueText[i] = new UIStaticText(tfKeysValueTextPos[0][i]);
        tfKeysValueText[i]->SetFont(cellFont);
        AddControl(tfKeysValueText[i]);
        
        tfTimeLimitsText[i] = new UIStaticText(Rect(buttonW*(12+i*5)/12, cellH*9, buttonW/4, cellH/2));
        tfTimeLimitsText[i]->SetFont(cellFont);
        AddControl(tfTimeLimitsText[i]); 
    }
    
    tfKeysValueText[0]->SetText(L"T:");
    tfKeysValueText[1]->SetText(L"V:");
    
    tfTimeLimitsText[0]->SetText(L" Tmin:");
    tfTimeLimitsText[1]->SetText(L" Tmax:");
    
    tfPosSlider[0] = Rect(buttonW, cellH*(9.5f), buttonW/6, cellH/2);
    tfPosSlider[1] = Rect(buttonW*11/6, cellH*(9.5f), buttonW/6, cellH/2);
    
    tfPosKFEdit[0] = Rect(buttonW*11/6, cellH*(12), buttonW/6, cellH/2);
    tfPosKFEdit[1] = Rect(buttonW*11/6, cellH*(9.5f), buttonW/6, cellH/2);
    
    for(int i = 0; i < 2; i++)
    {
        tfValueLimits[i] = new UITextField(tfPosSlider[i]);
        tfValueLimits[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfValueLimits[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfValueLimits[i]->SetFont(cellFont);
        tfValueLimits[i]->SetDelegate(this);
        AddControl(tfValueLimits[i]);
        
        tfTimeLimits[i] = new UITextField(Rect(buttonW*(15+i*5)/12, cellH*9, buttonW/6, cellH/2));
        tfTimeLimits[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfTimeLimits[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfTimeLimits[i]->SetFont(cellFont);
        tfTimeLimits[i]->SetDelegate(this);
        AddControl(tfTimeLimits[i]);
    }
    
    fsDlg = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlg->SetDelegate(this);
    Vector<String> filter;
    filter.push_back(".yaml");
    filter.push_back(".YAML");
    fsDlg->SetExtensionFilter(filter);
    fsDlg->SetTitle(LocalizedString("Dlg.Load"));
    fsDlg->SetCurrentDir("~res:/");
    
    fsDlgSprite = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlgSprite->SetDelegate(this);
    Vector<String> filter2;
    filter2.push_back(".txt");
    filter2.push_back(".TXT");
    fsDlgSprite->SetExtensionFilter(filter2);
    fsDlgSprite->SetTitle(LocalizedString("Dlg.SelectSprite"));
    fsDlgSprite->SetCurrentDir("~res:/");

    fsDlgProject = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlgProject->SetDelegate(this);
    fsDlgProject->SetOperationType(UIFileSystemDialog::OPERATION_CHOOSE_DIR);
    fsDlgProject->SetTitle(LocalizedString("Dlg.ChoosePrj"));

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

    tip = new UIStaticText(Rect(5, GetScreenHeight() - cellH*4, buttonW-5, cellH*4));
    tip->SetFont(cellFont);
    tip->SetMultiline(true);
    tip->SetAlign(DAVA::ALIGN_LEFT|DAVA::ALIGN_TOP);
    AddControl(tip);
    
    emitter = new ParticleEmitter();
    layers.push_back(new Layer(emitterProps, "", cellFont));
    layers[0]->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
    AddControl(layers[0]->curLayerTime);
    preview->SetEmitter(emitter);
    
    layers[0]->props[EMITTER_EMISSION_RANGE]->minValue = 0;
    layers[0]->props[EMITTER_EMISSION_RANGE]->maxValue = 360;
    
    forcePreview = new ForcePreviewControl();
    forcePreview->SetRect(Rect(buttonW*3/2, GetScreenHeight() - cellH*4, buttonW/2, buttonW*0.625f));
    forcePreview->SetValue(Vector3(0, 0, 0));
    AddControl(forcePreview);
    
    colorViewPosSlider = Rect(buttonW, cellH*13.5f, buttonW, cellH);
    colorViewPosKFEdit = Rect(buttonW, cellH*22.5f, buttonW, cellH);
    
    colorView = new UIControl();
    colorView->SetRect(colorViewPosSlider);
    colorView->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    colorView->GetBackground()->SetColor(Color(0, 0, 0, 1));
    AddControl(colorView);
    
    particleCountText = new UIStaticText();
    particleCountText->SetRect(Rect(buttonW*2.1f, 0, buttonW, cellH));
    particleCountText->SetFont(cellFont);
    particleCountText->SetAlign(DAVA::ALIGN_LEFT);
    AddControl(particleCountText);
    
    if(emitter->GetIs3D())
        emitter3D->SetStateText(UIControl::STATE_NORMAL, L"3D");
    else
        emitter3D->SetStateText(UIControl::STATE_NORMAL, L"2D");
    
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
                SetEmitterPropValue((eProps)selectedPropElement);
                GetEmitterPropValue((eProps)selectedPropElement);
            }
            if(selectedEmitterElement > 0)
            {
                SetLayerPropValue((lProps)selectedPropElement);
                GetLayerPropValue((lProps)selectedPropElement);
            }
        }
        if(selectedPropElement == LAYER_FORCES)
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forces[selectedForceElement].Get()->GetValue(curPropEditTime));
        if(selectedPropElement == LAYER_FORCES_VARIATION)
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forcesVariation[selectedForceElement].Get()->GetValue(curPropEditTime));
    }
}

void TestScreen::TextFieldShouldReturn(UITextField * textField)
{
    int32 value;
    if(textField == tfValueLimits[0])
    {
        swscanf(textField->GetText().c_str(), L"%d", &value);
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMinValue(value);
            propEdit[i]->SetMinY(value);
        }
        layers[selectedEmitterElement]->props[selectedPropElement]->minValue = value;
    }
    if(textField == tfValueLimits[1])
    {
        swscanf(textField->GetText().c_str(), L"%d", &value);
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMaxValue(value);
            propEdit[i]->SetMaxY(value);
        }
        layers[selectedEmitterElement]->props[selectedPropElement]->maxValue = value;
    }

    if(textField == tfTimeLimits[0])
    {
        swscanf(textField->GetText().c_str(), L"%d", &value);
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->SetMinX(value);
        }
        layers[selectedEmitterElement]->props[selectedPropElement]->minT = value;
    }
    
    if(textField == tfTimeLimits[1])
    {
        swscanf(textField->GetText().c_str(), L"%d", &value);
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->SetMaxX(value);
        }
        layers[selectedEmitterElement]->props[selectedPropElement]->maxT = value;
    }
    
    for(int i = 0; i < 4; i++)
    {
        if(textField == tfValue[i])
        {
            float32 v = 0;
            swscanf(textField->GetText().c_str(), L"%f", &v);
            vSliders[i]->SetValue(v);
        }
    }
    
    if(textField == tfKeysValue[0])
    {
        float32 val = 0;
        swscanf(textField->GetText().c_str(), L"%f", &val);
        if(val > 1.0f)
            val = 1.0f;
        if(val < 0.0f)
            val = 0.0f;
        Vector2 v2;
        propEdit[activeKFEdit]->GetSelectedValue(v2);
        v2.x = val;
        propEdit[activeKFEdit]->SetSelectedValue(v2);
    }

    if(textField == tfKeysValue[1])
    {
        float32 val = 0;
        swscanf(textField->GetText().c_str(), L"%f", &val);
        Vector2 v2;
        propEdit[activeKFEdit]->GetSelectedValue(v2);
        v2.y = val;
        propEdit[activeKFEdit]->SetSelectedValue(v2);
    }
    
    if(selectedEmitterElement == 0)
    {
        SetEmitterPropValue((eProps)selectedPropElement);
        GetEmitterPropValue((eProps)selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue((lProps)selectedPropElement);
        GetLayerPropValue((lProps)selectedPropElement);
    }    
}

bool TestScreen::TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString)
{
    int v;
    if(replacementLength == -1 || replacementString == L"-" || replacementString == L".")
    {
        return true;
    }
    else
    {
        int n = swscanf(replacementString.c_str(), L"%d", &v);
        if(n == 0)
            return false;
    }
    
    return true;
}

void TestScreen::ButtonPressed(BaseObject *obj, void *data, void *callerData)
{
    if(obj == newEmitter)
    {
        tip->SetText(L"");
        selectedEmitterElement = -1;
        selectedPropElement = -1;
        selectedForceElement = -1;
        forcePreview->SetValue(Vector3(0, 0, 0));
        
        SafeRelease(emitter);
        emitter = new ParticleEmitter();
        preview->SetEmitter(emitter);
        for(int i = 0; i < layers.size(); i++)
        {
            SafeRemoveControl(layers[i]->curLayerTime);
        }
        layers.clear();
        layers.push_back(new Layer(emitterProps, "", cellFont));
        layers[0]->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
        SafeAddControl(layers[0]->curLayerTime);
        
        layers[0]->props[EMITTER_EMISSION_RANGE]->minValue = 0;
        layers[0]->props[EMITTER_EMISSION_RANGE]->maxValue = 360;
        emitterList->RefreshList();
        propList->RefreshList();    
        
        if(emitter->GetIs3D())
            emitter3D->SetStateText(UIControl::STATE_NORMAL, L"3D");
        else
            emitter3D->SetStateText(UIControl::STATE_NORMAL, L"2D");
        
        HideAndResetEditFields();
    }
    if(obj == loadEmitter)
    {
        fsDlg->SetOperationType(UIFileSystemDialog::OPERATION_LOAD);
        fsDlg->Show(this);
    }
    if(obj == saveEmitter)
    {
        fsDlg->SetOperationType(UIFileSystemDialog::OPERATION_SAVE);
        fsDlg->Show(this);
    }
    if(obj == emitter3D)
    {
        emitter->Set3D(!emitter->GetIs3D());
        
        if(emitter->GetIs3D())
            emitter3D->SetStateText(UIControl::STATE_NORMAL, L"3D");
        else
            emitter3D->SetStateText(UIControl::STATE_NORMAL, L"2D");
        
        HideAndResetEditFields();
    }
    if(obj == addLayer)
    {        
        ParticleLayer *layer = new ParticleLayer();
        layer->endTime = 100000000.0f;
        emitter->AddLayer(layer);
        SafeRelease(layer);
        
        Layer *l = new Layer(layerProps, "", cellFont);
        l->spritePath = "";
        l->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()), buttonW, cellH));
        layers.push_back(l);
        SafeAddControl(l->curLayerTime);        
        
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == delLayer)
    {
        if(selectedEmitterElement > 0)
        {
            SafeRemoveControl(layers[selectedEmitterElement]->curLayerTime);
            
            layers.erase(layers.begin()+selectedEmitterElement);
            emitter->GetLayers().erase(emitter->GetLayers().begin()+selectedEmitterElement-1);
            selectedEmitterElement = -1;
        }
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == cloneLayer)
    {
        if(selectedEmitterElement > 0)
        {
            ParticleLayer *layer = emitter->GetLayers()[selectedEmitterElement-1]->Clone();
            emitter->AddLayer(layer);
            
            Layer *l = layers[selectedEmitterElement]->Clone();
            l->spritePath = "";
            l->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()), buttonW, cellH));
            layers.push_back(l);
            SafeAddControl(l->curLayerTime);
        }
        selectedEmitterElement = -1;
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == disableLayer)
    {
        if(selectedEmitterElement > 0)
        {
            bool disabled = emitter->GetLayers()[selectedEmitterElement-1]->isDisabled;
            layers[selectedEmitterElement]->isDisabled = !disabled;
            emitter->GetLayers()[selectedEmitterElement-1]->isDisabled = !disabled;
        }
        selectedEmitterElement = -1;
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
            layers[selectedEmitterElement]->props.at(selectedPropElement)->isDefault = true;
        
        if(selectedPropElement == LAYER_FORCES)
            layers[selectedEmitterElement]->props.at(LAYER_FORCES_OVER_LIFE)->isDefault = true;
        
        if(selectedPropElement == LAYER_FORCES_OVER_LIFE)
            layers[selectedEmitterElement]->props.at(LAYER_FORCES)->isDefault = true;
        
        if(selectedEmitterElement == 0)
        {
            ResetEmitterPropValue((eProps)selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {
            ResetLayerPropValue((lProps)selectedPropElement);
        }
        deltaIndex = 0;
        propList->RefreshList();
        HideAndResetEditFields();
        HideForcesList();
    }
    if(obj == OKBut)
    {
        layers[selectedEmitterElement]->props.at(selectedAddPropElement)->isDefault = false;
        
        if(selectedAddPropElement == LAYER_FORCES)
            layers[selectedEmitterElement]->props.at(LAYER_FORCES_OVER_LIFE)->isDefault = false;
        
        if(selectedAddPropElement == LAYER_FORCES_OVER_LIFE)
            layers[selectedEmitterElement]->props.at(LAYER_FORCES)->isDefault = false;
        
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
            SetEmitterPropValue((eProps)selectedPropElement, 1);
            GetEmitterPropValue((eProps)selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {   
            SetLayerPropValue((lProps)selectedPropElement, 1);
            GetLayerPropValue((lProps)selectedPropElement);
        }
    }
    if(obj == KFBut)
    {
        HideAndResetEditFields();
        HideForcesList();
        curPropType = true;
        if(selectedEmitterElement == 0)
        {
            SetEmitterPropValue((eProps)selectedPropElement, 1);
            GetEmitterPropValue((eProps)selectedPropElement);
        }
        if(selectedEmitterElement > 0)
        {   
            SetLayerPropValue((lProps)selectedPropElement, 1);
            GetLayerPropValue((lProps)selectedPropElement);
        }
    }
    if(obj == addForce)
    {
        if(selectedPropElement == LAYER_FORCES || selectedPropElement == LAYER_FORCES_OVER_LIFE)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->forces.push_back(RefPtr<PropertyLine<Vector3> >(new PropertyLineValue<Vector3>(Vector3(0, 0, 0))));
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.push_back(RefPtr<PropertyLine<float32> >(new PropertyLineValue<float32>(1.0f)));
        }
        if(selectedPropElement == LAYER_FORCES_VARIATION)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.push_back(RefPtr<PropertyLine<Vector3> >(new PropertyLineValue<Vector3>(Vector3(0, 0, 0))));
        }
        
        emitter->Restart();
        HideAndResetEditFields();
        selectedForceElement = -1;
        forcePreview->SetValue(Vector3(0, 0, 0));
        forcesList->RefreshList();
    }
    if(obj == delForce)
    {
        if(selectedForceElement >= 0)
        {
            if(selectedPropElement == LAYER_FORCES || selectedPropElement == LAYER_FORCES_OVER_LIFE)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->forces.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forces.begin() + selectedForceElement);
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.begin() + selectedForceElement);
            }
            if(selectedPropElement == LAYER_FORCES_VARIATION)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.begin() + selectedForceElement);
            }
            
            HideAndResetEditFields();
            forcesList->RefreshList();
            selectedForceElement = -1;
            forcePreview->SetValue(Vector3(0, 0, 0));
        }
        emitter->Restart();
    }
    if(obj == chooseProject)
    {
        SetDisabled(true);
        fsDlgProject->Show(this);
    }
}

bool TestScreen::GetProp(PropertyLineValue<float32> *pv, int32 id, bool getLimits)
{
    if(pv)
    {
        if(getLimits)
        {
            float32 value = pv->GetValue(0);
            if(value >= 0)
            {
                layers[selectedEmitterElement]->props[id]->minValue = 0;
                layers[selectedEmitterElement]->props[id]->maxValue = value*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement]->props[id]->minValue = value*2;
                layers[selectedEmitterElement]->props[id]->maxValue = abs(value);
            }
            
            if(selectedEmitterElement == 0 && (id == 0 || id == 1))
            {
                layers[selectedEmitterElement]->props[id]->minValue = 0;
                layers[selectedEmitterElement]->props[id]->maxValue = 360;
            }

            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement]->props[id]->minValue;
        int32 max = layers[selectedEmitterElement]->props[id]->maxValue;
        
        ShowValueEditFields(1);
        
        vSliders[0]->SetMaxValue(max);
        vSliders[0]->SetMinValue(min);
        vSliders[0]->SetValue(pv->GetValue(0));
        
        tfValueLimits[0]->SetText(Format(L"%d", min));
        tfValueLimits[1]->SetText(Format(L"%d", max));
        
        tfValue[0]->SetText(Format(L"%.2f", pv->GetValue(0)));
        valueText[0]->SetText(L"X:");
    }
    else return false;
    curPropType = false;
    return true;
}

bool TestScreen::GetProp(PropertyLineKeyframes<float32> *pk, int32 id, bool getLimits)
{
    if(pk)
    {
        if(getLimits)
        {
            float32 maxV = 1, minV = 0, minT = 0, maxT = 1;
            for(int i = 0; i < pk->keys.size(); i++)
            {
                minV = Min(minV, pk->keys[i].value);
                maxV = Max(maxV, pk->keys[i].value);
                minT = Min(minT, pk->keys[i].t);
                maxT = Max(maxT, pk->keys[i].t);
            }
            layers[selectedEmitterElement]->props[id]->minValue = minV;
            layers[selectedEmitterElement]->props[id]->maxValue = maxV;
            layers[selectedEmitterElement]->props[id]->minT = minT;
            layers[selectedEmitterElement]->props[id]->maxT = maxT;
            
            if(selectedEmitterElement == 0 && id == 2)
            {
                layers[selectedEmitterElement]->props[id]->minValue = 0;
                layers[selectedEmitterElement]->props[id]->maxValue = 360;
            }
            
            curPropType = false;
            return true;
        }
        int32 minV = layers[selectedEmitterElement]->props[id]->minValue;
        int32 maxV = layers[selectedEmitterElement]->props[id]->maxValue;
        int32 minT = layers[selectedEmitterElement]->props[id]->minT;
        int32 maxT = layers[selectedEmitterElement]->props[id]->maxT;

        ShowKeyedEditFields(1);
        
        keysValueText->SetText(Format(L" t = 0.00 : %.2f", pk->GetValue(0)));
        cur1DimProp = pk;
        
        tfValueLimits[0]->SetText(Format(L"%d", minV));
        tfValueLimits[1]->SetText(Format(L"%d", maxV));
        tfTimeLimits[0]->SetText(Format(L"%d", minT));
        tfTimeLimits[1]->SetText(Format(L"%d", maxT));
        
        propEdit[0]->SetMinY(minV);
        propEdit[0]->SetMaxY(maxV);
        propEdit[0]->SetMinX(minT);
        propEdit[0]->SetMaxX(maxT);
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

bool TestScreen::GetProp(PropertyLineValue<Vector2> *vv, int32 id, bool getLimits)
{
    if(vv)
    {
        if(getLimits)
        {
            float32 max = Max(vv->GetValue(0).x, vv->GetValue(0).y);
            float32 min = Min(vv->GetValue(0).x, vv->GetValue(0).y);
            if(max >= 0)
            {
                if(min > 0)
                    layers[selectedEmitterElement]->props[id]->minValue = 0;
                else
                    layers[selectedEmitterElement]->props[id]->minValue = min*2;
                layers[selectedEmitterElement]->props[id]->maxValue = max*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement]->props[id]->minValue = min*2;
                layers[selectedEmitterElement]->props[id]->maxValue = 0;
            }
            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement]->props[id]->minValue;
        int32 max = layers[selectedEmitterElement]->props[id]->maxValue;
        
        ShowValueEditFields(2);
        
        tfValueLimits[0]->SetText(Format(L"%d", min));
        tfValueLimits[1]->SetText(Format(L"%d", max));
        
        for(int i = 0; i < 2; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[0]->SetValue(vv->GetValue(0).x);
        vSliders[1]->SetValue(vv->GetValue(0).y);
        
        tfValue[0]->SetText(Format(L"%.2f", vv->GetValue(0).x));
        tfValue[1]->SetText(Format(L"%.2f", vv->GetValue(0).y));
        
        valueText[0]->SetText(L"X:");
        valueText[1]->SetText(L"Y:");
    }
    else return false;
    curPropType = false;
    return true;
}

bool TestScreen::GetProp(PropertyLineKeyframes<Vector2> *vk, int32 id, bool getLimits)
{
    if(vk)
    {
        if(getLimits)
        {
            float32 maxV1 = 1, minV1 = 0, maxV2 = 1, minV2 = 0, minT = 0, maxT = 1;
            for(int i = 0; i < vk->keys.size(); i++)
            {
                minV1 = Min(minV1, vk->keys[i].value.x);
                minV2 = Min(minV2, vk->keys[i].value.x);
                maxV1 = Max(maxV1, vk->keys[i].value.y);
                maxV2 = Max(maxV2, vk->keys[i].value.y);
                minT = Min(minT, vk->keys[i].t);
                maxT = Max(maxT, vk->keys[i].t);
            }
            layers[selectedEmitterElement]->props[id]->minValue = Min(minV1, minV2);
            layers[selectedEmitterElement]->props[id]->maxValue = Max(maxV1, maxV2);
            layers[selectedEmitterElement]->props[id]->minT = minT;
            layers[selectedEmitterElement]->props[id]->maxT = maxT;
            
            curPropType = false;
            return true;
        }
        int32 minV = layers[selectedEmitterElement]->props[id]->minValue;
        int32 maxV = layers[selectedEmitterElement]->props[id]->maxValue;
        int32 minT = layers[selectedEmitterElement]->props[id]->minT;
        int32 maxT = layers[selectedEmitterElement]->props[id]->maxT;
        
        ShowKeyedEditFields(2);
        
        keysValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f)", vk->GetValue(0).x, vk->GetValue(0).y));
        cur2DimProp = vk;
        
        tfValueLimits[0]->SetText(Format(L"%d", minV));
        tfValueLimits[1]->SetText(Format(L"%d", maxV));
        tfTimeLimits[0]->SetText(Format(L"%d", minT));
        tfTimeLimits[1]->SetText(Format(L"%d", maxT));
        
        for(int i = 0; i < 2; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(maxV);
            propEdit[i]->SetMinY(minV);
            propEdit[i]->SetMaxX(maxT);
            propEdit[i]->SetMinX(minT);
        }
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

bool TestScreen::GetProp(PropertyLineValue<Vector3> *vv, int32 id, bool getLimits)
{
    if(vv)
    {
        if(getLimits)
        {
            float32 max = Max(Max(vv->GetValue(0).x, vv->GetValue(0).y), vv->GetValue(0).z);
            float32 min = Min(Min(vv->GetValue(0).x, vv->GetValue(0).y), vv->GetValue(0).z);
            if(max >= 0)
            {
                if(min > 0)
                    layers[selectedEmitterElement]->props[id]->minValue = 0;
                else
                    layers[selectedEmitterElement]->props[id]->minValue = min*2;
                layers[selectedEmitterElement]->props[id]->maxValue = max*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement]->props[id]->minValue = min*2;
                layers[selectedEmitterElement]->props[id]->maxValue = 0;
            }
            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement]->props[id]->minValue;
        int32 max = layers[selectedEmitterElement]->props[id]->maxValue;
        
        
        tfValueLimits[0]->SetText(Format(L"%d", min));
        tfValueLimits[1]->SetText(Format(L"%d", max));
        
        int n = 2;
        if(emitter->GetIs3D())
            n = 3;
        
        ShowValueEditFields(n);
        
        for(int i = 0; i < 3; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[0]->SetValue(vv->GetValue(0).x);
        vSliders[1]->SetValue(vv->GetValue(0).y);
        vSliders[2]->SetValue(vv->GetValue(0).z);
        
        tfValue[0]->SetText(Format(L"%.2f", vv->GetValue(0).x));
        tfValue[1]->SetText(Format(L"%.2f", vv->GetValue(0).y));
        tfValue[2]->SetText(Format(L"%.2f", vv->GetValue(0).z));
        
        valueText[0]->SetText(L"X:");
        valueText[1]->SetText(L"Y:");
        valueText[2]->SetText(L"Z:");
    }
    else return false;
    curPropType = false;
    return true;
}

bool TestScreen::GetProp(PropertyLineKeyframes<Vector3> *vk, int32 id, bool getLimits)
{
    if(vk)
    {
        if(getLimits)
        {
            float32 maxV[3] = {1, 1, 1}, minV[3] = {0, 0, 0}, maxT = 1, minT = 0;
            for(int i = 0; i < vk->keys.size(); i++)
            {
                minV[0] = Min(minV[0], vk->keys[i].value.x);
                minV[1] = Min(minV[1], vk->keys[i].value.y);
                minV[2] = Min(minV[2], vk->keys[i].value.z);
                maxV[0] = Max(maxV[0], vk->keys[i].value.x);
                maxV[1] = Max(maxV[1], vk->keys[i].value.y);
                maxV[2] = Max(maxV[2], vk->keys[i].value.z);
                
                minT = Min(minT, vk->keys[i].t);
                maxT = Max(maxT, vk->keys[i].t);
            }
            layers[selectedEmitterElement]->props[id]->minValue = Min(Min(minV[0], minV[1]), minV[2]);
            layers[selectedEmitterElement]->props[id]->maxValue = Max(Max(maxV[0], maxV[1]), maxV[2]);
            layers[selectedEmitterElement]->props[id]->minT = minT;
            layers[selectedEmitterElement]->props[id]->maxT = maxT;
            
            curPropType = false;
            return true;
        }
        int32 minV = layers[selectedEmitterElement]->props[id]->minValue;
        int32 maxV = layers[selectedEmitterElement]->props[id]->maxValue;
        int32 minT = layers[selectedEmitterElement]->props[id]->minT;
        int32 maxT = layers[selectedEmitterElement]->props[id]->maxT;
        
        int n = 2;
        if(emitter->GetIs3D())
            n = 3;
        
        ShowKeyedEditFields(n);
        
        keysValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f, %.2f)", vk->GetValue(0).x, vk->GetValue(0).y, vk->GetValue(0).z));
        cur3DimProp = vk;
        
        tfValueLimits[0]->SetText(Format(L"%d", minV));
        tfValueLimits[1]->SetText(Format(L"%d", maxV));
        tfTimeLimits[0]->SetText(Format(L"%d", minT));
        tfTimeLimits[1]->SetText(Format(L"%d", maxT));
        
        for(int i = 0; i < 3; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(maxV);
            propEdit[i]->SetMinY(minV);
            propEdit[i]->SetMaxX(maxT);
            propEdit[i]->SetMinX(minT);
        }
        for(int i = 0; i < vk->keys.size(); i++)
        {
            propEdit[0]->GetValues().push_back(PropertyLineEditControl::PropertyRect(vk->keys.at(i).t, vk->keys.at(i).value.x));
            propEdit[1]->GetValues().push_back(PropertyLineEditControl::PropertyRect(vk->keys.at(i).t, vk->keys.at(i).value.y));
            propEdit[2]->GetValues().push_back(PropertyLineEditControl::PropertyRect(vk->keys.at(i).t, vk->keys.at(i).value.z));
        }
        propEdit[0]->SetText(L"X");
        propEdit[1]->SetText(L"Y");
        propEdit[2]->SetText(L"Z");
    }
    else return false;
    curPropType = true;
    return true;
}

bool TestScreen::GetProp(PropertyLineValue<Color> *cv, int32 id, bool getLimits)
{
    if(cv)
    {
        int32 min = layers[selectedEmitterElement]->props[id]->minValue;
        int32 max = layers[selectedEmitterElement]->props[id]->maxValue;
        
        ShowValueEditFields(4);
        
        SafeAddControl(colorView);
        
        colorView->SetRect(colorViewPosSlider);
        Color c = cv->GetValue(0);
        c.a = 1.0f;
        colorView->GetBackground()->SetColor(c);
        
        tfValueLimits[0]->SetText(Format(L"%d", min));
        tfValueLimits[1]->SetText(Format(L"%d", max));
        
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[3]->SetValue(cv->GetValue(0).a);
        vSliders[2]->SetValue(cv->GetValue(0).b);
        vSliders[1]->SetValue(cv->GetValue(0).g);
        vSliders[0]->SetValue(cv->GetValue(0).r);
        
        tfValue[0]->SetText(Format(L"%.2f", cv->GetValue(0).r));
        tfValue[1]->SetText(Format(L"%.2f", cv->GetValue(0).g));
        tfValue[2]->SetText(Format(L"%.2f", cv->GetValue(0).b));
        tfValue[3]->SetText(Format(L"%.2f", cv->GetValue(0).a));
        
        valueText[0]->SetText(L"R:");
        valueText[1]->SetText(L"G:");
        valueText[2]->SetText(L"B:");
        valueText[3]->SetText(L"A:");
    }
    else return false;
    curPropType = false;
    return true;  
}

bool TestScreen::GetProp(PropertyLineKeyframes<Color> *ck, int32 id, bool getLimits)
{
    if(ck)
    {
        int32 minV = layers[selectedEmitterElement]->props[id]->minValue;
        int32 maxV = layers[selectedEmitterElement]->props[id]->maxValue;
        int32 minT = layers[selectedEmitterElement]->props[id]->minT;
        int32 maxT = layers[selectedEmitterElement]->props[id]->maxT;

        ShowKeyedEditFields(4);
        
        SafeAddControl(colorView);
        colorView->SetRect(colorViewPosKFEdit);
        Color c = ck->GetValue(0);
        c.a = 1.0f;
        colorView->GetBackground()->SetColor(c);
        curColorProp = ck;
        
        keysValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f, %.2f, %.2f)", ck->GetValue(0).r, ck->GetValue(0).g, ck->GetValue(0).b, ck->GetValue(0).a));
        
        tfValueLimits[0]->SetText(Format(L"%d", minV));
        tfValueLimits[1]->SetText(Format(L"%d", maxV));
        tfTimeLimits[0]->SetText(Format(L"%d", minT));
        tfTimeLimits[1]->SetText(Format(L"%d", maxT));
        
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(maxV);
            propEdit[i]->SetMinY(minV);
            propEdit[i]->SetMaxX(maxT);
            propEdit[i]->SetMinX(minT);
        }
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

void TestScreen::GetEmitterPropValue(eProps id, bool getLimits)
{
    SafeAddControl(valueBut);
    SafeAddControl(KFBut);
    
    cur1DimProp = 0;
    cur2DimProp = 0;
    cur3DimProp = 0;
    curColorProp = 0;
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector3> *v3v;
    PropertyLineKeyframes<Vector3> *v3k;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    switch (id) {
        case EMITTER_TYPE:
            
            selectedEmitterTypeElement = emitter->type;
            
            SafeAddControl(emitterTypeList);
            SafeRemoveControl(valueBut);
            SafeRemoveControl(KFBut);
            
            if(getLimits && selectedEmitterTypeElement == 0)
                    layers[0]->props[EMITTER_TYPE]->isDefault = true;
            else
                    layers[0]->props[EMITTER_TYPE]->isDefault = false;
            
            break;
            
        case EMITTER_EMISSION_ANGLE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionAngle.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionAngle.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_EMISSION_VECTOR:
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->emissionVector.Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->emissionVector.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(v3k, id, getLimits))
                if(!GetProp(v3v, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_EMISSION_RANGE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionRange.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionRange.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_RADIUS:
            pk = dynamic_cast< PropertyLineKeyframes<float> *>(emitter->radius.Get());
            pv = dynamic_cast< PropertyLineValue<float> *>(emitter->radius.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_COLOR_OVER_LIFE:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->colorOverLife.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->colorOverLife.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_SIZE:
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->size.Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->size.Get());
            layers[0]->props.at(id)->isDefault = false;
            if(!GetProp(v3k, id, getLimits))
                if(!GetProp(v3v, id, getLimits))
                    layers[0]->props.at(id)->isDefault = true;
            break;
            
        case EMITTER_LIFE:
            if(emitter->GetLifeTime() == 1000000000.0f)
                layers[0]->props.at(id)->isDefault = true;
            else
                layers[0]->props.at(id)->isDefault = false;
            
            SafeAddControl(valueText[0]);
            SafeAddControl(tfValue[0]);
            vSliders[0]->SetValue(emitter->GetLifeTime());
            tfValue[0]->SetText(Format(L"%.2f", emitter->GetLifeTime()));
            valueText[0]->SetText(L"X:");            
            break;
        default:
            break;
    }
}

void TestScreen::SetEmitterPropValue(eProps id, bool def)
{
    float32 value[4];
    if(def)
    {
        memset(&value, 0, sizeof(float32)*4);
        layers[0]->props[id]->minValue = 0;
        layers[0]->props[id]->maxValue = 1;
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
    PropertyLine<Vector3> *valueDim3;
    PropertyLine<Color> *valueDim4;
    if(!curPropType)
    {
        valueDim1 = new PropertyLineValue<float32>(value[0]);
        valueDim2 = new PropertyLineValue<Vector2>(Vector2(value[0],value[1]));
        valueDim3 = new PropertyLineValue<Vector3>(Vector3(value[0], value[1], value[2]));
        valueDim4 = new PropertyLineValue<Color>(Color(value[0], value[1], value[2], value[3]));
    }
    else
    {
        PropertyLineKeyframes<float32> *value1 = new PropertyLineKeyframes<float32>();
        PropertyLineKeyframes<Vector2> *value2 = new PropertyLineKeyframes<Vector2>();
        PropertyLineKeyframes<Vector3> *value3 = new PropertyLineKeyframes<Vector3>();
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
            value3->AddValue(t, Vector3(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t)));
            value4->AddValue(t, Color(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t), KFValues[3]->GetValue(t)));
        }
        
        for(int i = 0; i < 4; i++)
        {
            SafeRelease(KFValues[i]);
        }
        
        valueDim1 = value1;
        valueDim2 = value2;
        valueDim3 = value3;
        valueDim4 = value4;
    }
    switch (id) {
        case EMITTER_TYPE:
            emitter->type = (ParticleEmitter::eType)selectedEmitterTypeElement;
            break;
            
        case EMITTER_EMISSION_ANGLE:
            emitter->emissionAngle = valueDim1;
            break;
            
        case EMITTER_EMISSION_VECTOR:
            emitter->emissionVector = valueDim3;
            break;
            
        case EMITTER_EMISSION_RANGE:
            emitter->emissionRange = valueDim1;      
            break;
           
        case EMITTER_RADIUS:
            emitter->radius = valueDim1;
            break;
            
        case EMITTER_COLOR_OVER_LIFE:
            emitter->colorOverLife = valueDim4;
            break;
            
        case EMITTER_SIZE:
            emitter->size = valueDim3;
            break;
            
        case EMITTER_LIFE:
            emitter->SetLifeTime(value[0]);
            break;
            
        default:
            break;
    }
    SafeRelease(valueDim1);
    SafeRelease(valueDim2);
    SafeRelease(valueDim3);
    SafeRelease(valueDim4);
}

void TestScreen::ResetEmitterPropValue(eProps id)
{
    switch (id) {
        case EMITTER_TYPE:
            selectedEmitterTypeElement = 0;
            emitter->type = ParticleEmitter::EMITTER_POINT;
            break;
            
        case EMITTER_EMISSION_ANGLE:
            emitter->emissionAngle = 0;
            break;
            
        case EMITTER_EMISSION_VECTOR:
            emitter->emissionVector = 0;
            break;
            
        case EMITTER_EMISSION_RANGE:
            emitter->emissionRange = 0;      
            break;
            
        case EMITTER_RADIUS:
            emitter->radius = 0;
            break;
            
        case EMITTER_COLOR_OVER_LIFE:
            emitter->colorOverLife = 0;
            break;

        case EMITTER_SIZE:
            emitter->size = 0;
            break;
            
        case EMITTER_LIFE:
            emitter->SetLifeTime(1000000000.0f);
            break;
            
        default:
            break;
    } 
    emitter->Restart();
}

void TestScreen::ResetLayerPropValue(lProps id)
{
    switch (id) {
        case LAYER_SPRITE:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(0);
            break;
            
        case LAYER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->life = 0;
            break;
            
        case LAYER_LIFE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation = 0;       
            break;
            
        case LAYER_NUMBER:
            emitter->GetLayers().at(selectedEmitterElement-1)->number = 0;     
            break;
            
        case LAYER_NUMBER_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation = 0;
            break;
            
        case LAYER_SIZE:
            emitter->GetLayers().at(selectedEmitterElement-1)->size = 0;
            break;
            
        case LAYER_SIZE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation = 0;      
            break;
            
        case LAYER_SIZE_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife = 0;     
            break;
            
        case LAYER_VELOCITY:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocity = 0;      
            break;
            
        case LAYER_VELOCITY_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation = 0;      
            break;
            
        case LAYER_VELOCITY_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife = 0;     
            break;
            
        case LAYER_FORCES:
        case LAYER_FORCES_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->forces.clear();
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.clear();
            break;
            
        case LAYER_FORCES_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.clear();
            break;
            
        case LAYER_SPIN:
            emitter->GetLayers().at(selectedEmitterElement-1)->spin = 0;    
            break;
            
        case LAYER_SPIN_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation = 0;
            break;
            
        case LAYER_SPIN_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife = 0;     
            break;
            
        case LAYER_MOTION_RANDOM:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom = 0;    
            break;
            
        case LAYER_MOTION_RANDOM_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation = 0;     
            break;
            
        case LAYER_MOTION_RANDOM_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife = 0;      
            break;
            
        case LAYER_BOUNCE:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounce = 0;       
            break;
            
        case LAYER_BOUNCE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation = 0;      
            break;
            
        case LAYER_BOUNCE_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife = 0;    
            break;
            
        case LAYER_COLOR_RANDOM:
            emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom = 0;      
            break;
            
        case LAYER_ALPHA_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife = 0;
            break;
            
        case LAYER_COLOR_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife = 0;  
            break;
            
        default:
            break;
    }    
    emitter->Restart();
}

void TestScreen::GetLayerPropValue(lProps id, bool getLimits)
{
    if(id > 0)
    {
        SafeAddControl(valueBut);
        SafeAddControl(KFBut);
    }
    
    curColorProp = 0;
    cur1DimProp = 0;
    cur2DimProp = 0;
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector2> *vv;
    PropertyLineKeyframes<Vector2> *vk;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    switch (id) {
        case LAYER_SPRITE:
            sprite = emitter->GetLayers().at(selectedEmitterElement-1)->GetSprite();
            spriteControl->SetSprite(sprite, 0);
            if(sprite)
                layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            
            SafeAddControl(spritePanel);
            
            if(sprite)
                spriteInfo->SetText(Format(L"%.0fx%.0f@%d", sprite->GetWidth(), sprite->GetHeight(), sprite->GetFrameCount()));
            else
                spriteInfo->SetText(L"");
            break;
            
        case LAYER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->life.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->life.Get());
            layers.at(selectedEmitterElement)->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_LIFE_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_NUMBER:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->number.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->number.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;   
            break;
            
        case LAYER_NUMBER_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_SIZE:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->size.Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->size.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(vk, id, getLimits))
                if(!GetProp(vv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true; 
            break;
            
        case LAYER_SIZE_VARIATION:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(vk, id, getLimits))
                if(!GetProp(vv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_SIZE_OVER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;    
            break;
            
        case LAYER_VELOCITY:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;     
            break;
            
        case LAYER_VELOCITY_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;    
            break;
            
        case LAYER_VELOCITY_OVER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_FORCES:
            
        case LAYER_FORCES_VARIATION:
            
        case LAYER_FORCES_OVER_LIFE:
            if(selectedForceElement >= 0)
                GetForcesValue(selectedForceElement, getLimits);
            break;
            
        case LAYER_SPIN:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spin.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spin.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;   
            break;
            
        case LAYER_SPIN_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_SPIN_OVER_LIFE:
            
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;  
            break;
            
        case LAYER_MOTION_RANDOM:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_MOTION_RANDOM_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_MOTION_RANDOM_OVER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_BOUNCE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_BOUNCE_VARIATION:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_BOUNCE_OVER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_COLOR_RANDOM:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_ALPHA_OVER_LIFE:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        case LAYER_COLOR_OVER_LIFE:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Get());
            layers[selectedEmitterElement]->props.at(id)->isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
                    layers[selectedEmitterElement]->props.at(id)->isDefault = true;
            break;
            
        default:
            break;
    }
}

void TestScreen::SetLayerPropValue(lProps id, bool def)
{
    float32 value[4];
    if(def)
    {
        memset(&value, 0, sizeof(float32)*4);
        layers[selectedEmitterElement]->props[id]->minValue = 0;
        layers[selectedEmitterElement]->props[id]->maxValue = 1;
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
    PropertyLine<Vector3> *valueDim3;
    PropertyLine<Color> *valueDim4;
    if(!curPropType)
    {
        valueDim1 = new PropertyLineValue<float32>(value[0]);
        valueDim2 = new PropertyLineValue<Vector2>(Vector2(value[0],value[1]));
        valueDim3 = new PropertyLineValue<Vector3>(Vector3(value[0],value[1], value[2]));
        valueDim4 = new PropertyLineValue<Color>(Color(value[0], value[1], value[2], value[3]));
    }
    else
    {
        PropertyLineKeyframes<float32> *value1 = new PropertyLineKeyframes<float32>();
        PropertyLineKeyframes<Vector2> *value2 = new PropertyLineKeyframes<Vector2>();
        PropertyLineKeyframes<Vector3> *value3 = new PropertyLineKeyframes<Vector3>();
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
            value3->AddValue(t, Vector3(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t)));
            value4->AddValue(t, Color(KFValues[0]->GetValue(t), KFValues[1]->GetValue(t), KFValues[2]->GetValue(t), KFValues[3]->GetValue(t)));
        }
        
        for(int i = 0; i < 4; i++)
        {
            SafeRelease(KFValues[i]);
        }
        
        valueDim1 = value1;
        valueDim2 = value2;
        valueDim3 = value3;
        valueDim4 = value4;
    }
    switch (id) {
        case LAYER_SPRITE:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(sprite);
            break;
            
        case LAYER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->life = valueDim1;
            break;
            
        case LAYER_LIFE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation = valueDim1;
            break;
            
        case LAYER_NUMBER:
            emitter->GetLayers().at(selectedEmitterElement-1)->number = valueDim1;
            break;
            
        case LAYER_NUMBER_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation = valueDim1;
            break;
            
        case LAYER_SIZE:
            emitter->GetLayers().at(selectedEmitterElement-1)->size = valueDim2;
            break;
            
        case LAYER_SIZE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation = valueDim2;
            break;
            
        case LAYER_SIZE_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife = valueDim1;
            break;
            
        case LAYER_VELOCITY:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocity = valueDim1;
            break;
            
        case LAYER_VELOCITY_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation = valueDim1;
            break;
            
        case LAYER_VELOCITY_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife = valueDim1;
            break;
            
        case LAYER_FORCES:
            emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(selectedForceElement) = valueDim3;
            break;
            
        case LAYER_FORCES_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(selectedForceElement) = valueDim3;
            break;
            
        case LAYER_FORCES_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(selectedForceElement) = valueDim1;
            break;
            
        case LAYER_SPIN:
            emitter->GetLayers().at(selectedEmitterElement-1)->spin = valueDim1;
            break;
            
        case LAYER_SPIN_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation = valueDim1;
            break;
            
        case LAYER_SPIN_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife = valueDim1;
            break;
            
        case LAYER_MOTION_RANDOM:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom = valueDim1;
            break;
            
        case LAYER_MOTION_RANDOM_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation = valueDim1;
            break;
            
        case LAYER_MOTION_RANDOM_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife = valueDim1;
            break;
            
        case LAYER_BOUNCE:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounce = valueDim1;
            break;
            
        case LAYER_BOUNCE_VARIATION:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation = valueDim1;
            break;
            
        case LAYER_BOUNCE_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife = valueDim1;
            break;
            
        case LAYER_COLOR_RANDOM:
            emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom = valueDim4;
            break;
            
        case LAYER_ALPHA_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife = valueDim1;
            break;
            
        case LAYER_COLOR_OVER_LIFE:
            emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife = valueDim4;
            break;
            
        default:
            break;
    }
    SafeRelease(valueDim1);
    SafeRelease(valueDim2);
    SafeRelease(valueDim3);
    SafeRelease(valueDim4);    
}

void TestScreen::GetForcesValue(int32 id, bool getLimits)
{
    ShowForcesList();
    if (selectedPropElement == 11)
    {
        PropertyLineKeyframes<Vector3> *vk = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(id).Get());
        PropertyLineValue<Vector3> *vv = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(id).Get());
        if(!GetProp(vk, 11, getLimits))
        {
            if(GetProp(vv, 11, getLimits))
            {
                forcePreview->SetValue(vv->GetValue(0));
                layers[selectedEmitterElement]->props.at(11)->isDefault = false;
            }
        }
        else
        {
            forcePreview->SetValue(vk->GetValue(0));
            layers[selectedEmitterElement]->props.at(11)->isDefault = false;
        }
    }
    if (selectedPropElement == 12)
    {
        PropertyLineKeyframes<Vector3> *vk = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(id).Get());
        PropertyLineValue<Vector3> *vv = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(id).Get());
        if(!GetProp(vk, 12, getLimits))
        {
            if(GetProp(vv, 12, getLimits))
            {
                forcePreview->SetValue(vv->GetValue(0));
                layers[selectedEmitterElement]->props.at(12)->isDefault = false;
            }
        }
        else
        {
            forcePreview->SetValue(vk->GetValue(0));
            layers[selectedEmitterElement]->props.at(12)->isDefault = false;
        }    
    }
    if (selectedPropElement == 13)
    {   
        SafeRemoveControl(forcePreview);
        
        PropertyLineKeyframes<float32> *vk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(id).Get());
        PropertyLineValue<float32> *vv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(id).Get());
        if(!GetProp(vk, 13, getLimits))
        {
            if(GetProp(vv, 13, getLimits))
            {
                layers[selectedEmitterElement]->props.at(13)->isDefault = false;
            }
        }
        else
        {
            layers[selectedEmitterElement]->props.at(13)->isDefault = false;
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
        {
            PropertyLineKeyframes<float32> *p = propEdit[i]->GetPropertyLine();
            propEdit[i]->AddPoint(t, p->GetValue(t));
            SafeRelease(p);
        }
    }
    if(selectedEmitterElement == 0)
    {
        SetEmitterPropValue((eProps)selectedPropElement);
        GetEmitterPropValue((eProps)selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue((lProps)selectedPropElement);
        GetLayerPropValue((lProps)selectedPropElement);
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
        SetEmitterPropValue((eProps)selectedPropElement);
        GetEmitterPropValue((eProps)selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue((lProps)selectedPropElement);
        GetLayerPropValue((lProps)selectedPropElement);
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
        SetEmitterPropValue((eProps)selectedPropElement);
        GetEmitterPropValue((eProps)selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue((lProps)selectedPropElement);
        GetLayerPropValue((lProps)selectedPropElement);
    }
}

void TestScreen::OnPointSelected(PropertyLineEditControl *forControl, int32 index, Vector2 value)
{
    if(index == -1)
    {
        for(int i = 0; i < 4; i++)
            if(forControl != propEdit[i])
                propEdit[i]->DeselectPoint();
            else
                activeKFEdit = i;
        
        for(int i = 0; i < 2; i++)
        {
            SafeRemoveControl(tfKeysValue[i]);
            SafeRemoveControl(tfKeysValueText[i]);
        }
    }
    else
    {
        for(int i = 0; i < 4; i++)
            if(forControl != propEdit[i])
                propEdit[i]->DeselectPoint();
            else
                activeKFEdit = i;
        
        for(int i = 0; i < 2; i++)
        {
            SafeAddControl(tfKeysValue[i]);
            SafeAddControl(tfKeysValueText[i]);
        }
        
        tfKeysValue[0]->SetText(Format(L"%.2f", value.x));
        tfKeysValue[1]->SetText(Format(L"%.2f", value.y));
    }
}

void TestScreen::OnMouseMove(PropertyLineEditControl *forControl, float32 t)
{
    curPropEditTime = t;
    if(selectedPropElement == 11)
        forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forces[selectedForceElement].Get()->GetValue(curPropEditTime));
    if(selectedPropElement == 12)
        forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forcesVariation[selectedForceElement].Get()->GetValue(curPropEditTime));
    
    if(cur1DimProp)
    {
        keysValueText->SetText(Format(L" t = %.2f : %.2f", t, cur1DimProp->GetValue(t)));
    }
    else if(cur2DimProp)
    {
        keysValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f)", t, cur2DimProp->GetValue(t).x, cur2DimProp->GetValue(t).y));
    }
    else if(cur3DimProp)
    {
        keysValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f, %.2f)", t, cur3DimProp->GetValue(t).x, cur3DimProp->GetValue(t).y, cur3DimProp->GetValue(t).z));
    }
    else if(curColorProp)
    {
        Color c = curColorProp->GetValue(t);
        keysValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f, %.2f, %.2f)", t, c.r, c.g, c.b, c.a));
        c.a = 1.0f;
        colorView->GetBackground()->SetColor(c);
    }
    
    for(int i = 0; i < 4; i++)
        if(forControl != propEdit[i])
            propEdit[i]->SetCurTime(t);
}

void TestScreen::OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile)
{
    if(forDialog == fsDlg)
    {
        if(forDialog->GetOperationType() == UIFileSystemDialog::OPERATION_LOAD)
        {
            tip->SetText(L"");
            selectedEmitterElement = -1;
            selectedPropElement = -1;
            selectedForceElement = -1;
            forcePreview->SetValue(Vector3(0, 0, 0));
            
            SafeRelease(emitter);
            emitter = new ParticleEmitter();
            emitter->LoadFromYaml(pathToFile);
            preview->SetEmitter(emitter);
            for(int i = 0; i < layers.size(); i++)
            {
                SafeRemoveControl(layers[i]->curLayerTime);
            }
            layers.clear();
            layers.push_back(new Layer(emitterProps, "", cellFont));
            layers[0]->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
            SafeAddControl(layers[0]->curLayerTime);
            selectedEmitterElement = 0;
            for(int i = 0; i < emitterProps.size(); i++)
            {
                GetEmitterPropValue((eProps)i, true);
            }
            for(int i = 0; i < emitter->GetLayers().size(); i++)
            {
                String spritePath = emitter->GetLayers()[i]->GetSprite()->GetName();
                Layer * l = new Layer(layerProps, spritePath.substr(0, spritePath.size()-4), cellFont);
                l->curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()), buttonW, cellH));
                layers.push_back(l);
                SafeAddControl(l->curLayerTime);
                
                for(int j = 0; j < layerProps.size(); j++)
                {
                    selectedEmitterElement = i + 1;
                    selectedPropElement = j;
                    if(j == 11)
                    {
                        for(int k = 0; k < emitter->GetLayers()[i]->forces.size(); k++)
                            GetForcesValue(k, true);
                    }
                    else if(j == 12)
                    {
                        for(int k = 0; k < emitter->GetLayers()[i]->forcesVariation.size(); k++)
                            GetForcesValue(k, true);
                    }
                    else if(j == 13)
                    {
                        for(int k = 0; k < emitter->GetLayers()[i]->forcesOverLife.size(); k++)
                            GetForcesValue(k, true);
                    }
                    else
                    {
                        GetLayerPropValue((lProps)j, true);
                    }
                }
            }
            selectedEmitterElement = -1;
            selectedPropElement = -1;
            HideAndResetEditFields();
            HideForcesList();
            emitterList->RefreshList();
            propList->RefreshList();
            
            if(emitter->GetIs3D())
                emitter3D->SetStateText(UIControl::STATE_NORMAL, L"3D");
            else
                emitter3D->SetStateText(UIControl::STATE_NORMAL, L"2D");
        }
        if(forDialog->GetOperationType() == UIFileSystemDialog::OPERATION_SAVE)
        {
            SaveToYaml(pathToFile);
        }
    }
    if(forDialog == fsDlgSprite)
    {
        String path = "~res:" + pathToFile.substr(pathToFile.rfind("/Data") + 5);
        path = path.substr(0, path.size() - 4);

        layers[selectedEmitterElement]->spritePath = path;
        
        sprite = Sprite::Create(path);
        SetLayerPropValue(LAYER_SPRITE);
        GetLayerPropValue(LAYER_SPRITE);
    }
    if(forDialog == fsDlgProject)
    {
        ReplaceBundleName(pathToFile + "/Data");
        
        ExecutePacker(pathToFile + "/DataSource");
        
        SetDisabled(false);
    }
}

void TestScreen::ExecutePacker(const String &path)
{
    FileList fl(path);
    for(int i = 0; i < fl.GetCount(); i++)
        if(fl.IsDirectory(i) && !fl.IsNavigationDirectory(i))
            ExecutePacker(fl.GetPathname(i));
    
    FileSystem::Instance()->Spawn("./ResourcePacker " + path);
}

void TestScreen::OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog)
{
    if(forDialog == fsDlgProject)
    {
        SetDisabled(false);
    }
}

void TestScreen::HideForcesList()
{
    SafeRemoveControl(forcesList);
    SafeRemoveControl(addForce);
    SafeRemoveControl(delForce);
    SafeRemoveControl(forcePreview);
}

void TestScreen::ShowForcesList()
{
    SafeAddControl(forcesList);
    SafeAddControl(addForce);
    SafeAddControl(delForce);
    SafeAddControl(forcesList);
    SafeAddControl(forcePreview);
}

void TestScreen::HideAndResetEditFields()
{
    for(int i = 0; i < 4; i++)
    {
        SafeRemoveControl(propEdit[i]);
        SafeRemoveControl(vSliders[i]);
        SafeRemoveControl(tfValue[i]);
        SafeRemoveControl(valueText[i]);
        vSliders[i]->SetValue(0.0f);
        propEdit[i]->Reset();
    }
    for(int i = 0; i < 2; i++)
    {
        SafeRemoveControl(tfValueLimits[i]);
        SafeRemoveControl(tfTimeLimits[i]);
        SafeRemoveControl(tfKeysValue[i]);
        SafeRemoveControl(tfKeysValueText[i]);
        SafeRemoveControl(tfTimeLimitsText[i]);
    }
    
    SafeRemoveControl(KFBut);
    SafeRemoveControl(valueBut);
    
    SafeRemoveControl(spritePanel);
    
    SafeRemoveControl(colorView);
    SafeRemoveControl(keysValueText);
    
    SafeRemoveControl(emitterTypeList);
}

void TestScreen::ShowValueEditFields(int32 dim)
{
    for(int i = 0; i < dim; i++)
    {
        SafeAddControl(vSliders[i]);
        SafeAddControl(valueText[i]);
        SafeAddControl(tfValue[i]);
    }
    
    for(int i = 0; i < 2; i++)
    {   
        SafeAddControl(tfValueLimits[i]);
        tfValueLimits[i]->SetRect(tfPosSlider[i]);
    }
}

void TestScreen::ShowKeyedEditFields(int32 dim)
{
    SafeAddControl(keysValueText);
    keysValueText->SetRect(keysValueTextPos[dim-1]);
    
    for(int i = 0; i < dim; i++)
        SafeAddControl(propEdit[i]);
    
    for(int i = 0; i < 2; i++)
    {
        SafeAddControl(tfValueLimits[i]);
        SafeAddControl(tfTimeLimits[i]);
        SafeAddControl(tfTimeLimitsText[i]);
        
        tfKeysValue[i]->SetRect(tfKeysValuePos[dim-1][i]);
        tfKeysValueText[i]->SetRect(tfKeysValueTextPos[dim-1][i]);
        
        tfValueLimits[i]->SetRect(tfPosKFEdit[i]);
    }
    
    
}

void TestScreen::ShowAddProps()
{
    SafeAddControl(addPropList);
    SafeAddControl(OKBut);
    SafeAddControl(cancelBut);
    emitterList->SetDisabled(true);
    propList->SetDisabled(true);
    loadEmitter->SetDisabled(true);
    saveEmitter->SetDisabled(true);
    addLayer->SetDisabled(true);
    delLayer->SetDisabled(true);
    addProp->SetDisabled(true);
    delProp->SetDisabled(true);
    cloneLayer->SetDisabled(true);
    disableLayer->SetDisabled(true);
}

void TestScreen::HideAddProps()
{
    SafeRemoveControl(addPropList);
    SafeRemoveControl(OKBut);
    SafeRemoveControl(cancelBut);
    emitterList->SetDisabled(false);
    propList->SetDisabled(false);
    loadEmitter->SetDisabled(false);
    saveEmitter->SetDisabled(false);
    addLayer->SetDisabled(false);
    delLayer->SetDisabled(false);
    addProp->SetDisabled(false);
    delProp->SetDisabled(false);
    cloneLayer->SetDisabled(false);
    disableLayer->SetDisabled(false);
}

void TestScreen::UnloadResources()
{
    
    SafeRelease(cloneLayer);
    SafeRelease(disableLayer);
    SafeRelease(emitterTypeList);
    
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
    SafeRelease(chooseProject);
    SafeRetain(emitter3D);
    
    for(int i = 0 ;i < 4; i++)
    {
        SafeRelease(tfValue[i]);
        SafeRelease(valueText[i]);
        SafeRelease(vSliders[i]);
        SafeRelease(propEdit[i]);
    }
    
    for(int i = 0; i < 2; i++)
    {
        SafeRelease(tfTimeLimitsText[i]);
        SafeRelease(tfKeysValueText[i]);
        SafeRelease(tfKeysValue[i]);
        SafeRelease(tfValueLimits[i]);
        SafeRelease(tfTimeLimits[i]);
    }
    
    SafeRelease(spriteInfo);
    SafeRelease(particleCountText);
    SafeRelease(keysValueText);
    
    SafeRelease(emitterList);
    SafeRelease(propList);
    SafeRelease(addPropList);
    SafeRelease(forcesList);
    
    SafeRelease(tip);
    
    SafeRelease(fsDlg);
    SafeRelease(fsDlgSprite);
    SafeRelease(fsDlgProject);
    
    SafeRelease(spritePanel);
    SafeRelease(spriteControl);
    
    SafeRelease(sprite);
    SafeRelease(colorView);
    SafeRelease(preview);
    SafeRelease(forcePreview);
    
    SafeRelease(cellFont);
    SafeRelease(f);
    
    SafeRelease(emitter); 
}

int32 TestScreen::ElementsCount(UIList *forList)
{
    if(forList == emitterList)
    {
        return layers.size();
    }
    if(forList == propList)
    {
        int32 n = 0;
        if(selectedEmitterElement == 0)
        {
            for(int i = 0; i < emitterProps.size(); i++)
            {
                if(!layers[selectedEmitterElement]->props.at(i)->isDefault)
                    n++;
            }
        }
        else if(selectedEmitterElement > 0)
        {
            for(int i = 0; i < layerProps.size(); i++)
            {
                if(!layers[selectedEmitterElement]->props.at(i)->isDefault)
                    n++;
            }
        }
        return n;
    }
    if(forList == addPropList)
    {
        if(selectedEmitterElement == 0)
            return emitterProps.size();
        else if(selectedEmitterElement > 0)
            return layerProps.size();
    }
    if(forList == forcesList)
    {
        switch (selectedPropElement) {
            case LAYER_FORCES:
                return emitter->GetLayers().at(selectedEmitterElement-1)->forces.size();
                break;
                
            case LAYER_FORCES_VARIATION:
                return emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.size();
                break;
                
            case LAYER_FORCES_OVER_LIFE:
                return emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.size();
                break;
            default:
                break;
        }
    }
    if(forList == emitterTypeList)
    {
        return emitterTypes.size();
    }
    return 0;
}

int32 TestScreen::CellHeight(UIList *forList, int32 index)
{
    return cellH*2/3;
}

UIListCell *TestScreen::CellAtIndex(UIList *forList, int32 index)
{
    if (forList == emitterList) 
    {
        UIListCell *c = forList->GetReusableCell("EmitterList Cell");
		if(!c)
        {
            c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "EmitterList Cell");
        }
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
        
        if(index == 0)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, L"Emitter");
        }
        else
        {
            if(layers[index]->isDisabled)
                c->SetStateText(UIListCell::STATE_NORMAL, Format(L"Layer %d /Disabled", index));
            else
                c->SetStateText(UIListCell::STATE_NORMAL, Format(L"Layer %d", index));
        }
        
        if (index == selectedEmitterElement) 
            c->SetSelected(true);
        else
            c->SetSelected(false);
        
        return c;
    }
    if (forList == propList)
    {
        PropListCell *c = (PropListCell *)forList->GetReusableCell("PropList Cell");
		if(!c)
        {
            c = new PropListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "PropList Cell");
        }
        c->SetFront(cellFont);
        int32 n = layers[selectedEmitterElement]->props.size();
        while((index+deltaIndex < n) && layers[selectedEmitterElement]->props.at(index+deltaIndex)->isDefault)
        {
            deltaIndex++;
        }
        c->SetId(index+deltaIndex);
        
        if(index+deltaIndex < n)
            c->SetText(StringToWString(*layers[selectedEmitterElement]->props.at(index+deltaIndex)->name));
        
        return c;
    }
    if (forList == addPropList)
    {
        UIListCell *c = forList->GetReusableCell("AddPropList Cell");
		if(!c)
        {
            c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "AddPropList Cell");
        }
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
        UIListCell *c = forList->GetReusableCell("Force Cell");
		if(!c)
        {
            c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "Force Cell");
        }
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
        if(selectedPropElement == 11)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, Format(L"Force %d", index));
        }
        if(selectedPropElement == 12)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, Format(L"ForceVariation %d", index));
        }
        if(selectedPropElement == 13)
        {
            c->SetStateText(UIListCell::STATE_NORMAL, Format(L"ForceOverLife %d", index));
        }
        if(selectedForceElement == index)
        {
            c->SetSelected(true);
        }
        return c;
    }
    
    if(forList == emitterTypeList)
    {        
        UIListCell *c = forList->GetReusableCell("EmitterType Cell");
		if(!c)
        {
            c = new UIListCell(Rect(0, 0, forList->size.x, CellHeight(forList, index)), "EmitterType Cell");
        }
        c->SetStateFont(UIListCell::STATE_NORMAL, cellFont);
        c->SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
        c->GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));  
        c->SetStateText(UIListCell::STATE_NORMAL, StringToWString(emitterTypes[index]));

        if(index == selectedEmitterTypeElement)
            c->SetSelected(true);
        else
            c->SetSelected(false);
        
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
        tip->SetText(L"");
        selectedPropElement = -1;
        selectedForceElement = -1;
        forcePreview->SetValue(Vector3(0, 0, 0));
        
        if(selectedEmitterElement == 0)
        {
            for(int i = 0; i < emitterProps.size(); i++)
            {
                GetEmitterPropValue((eProps)i);
            }
        }
        if(selectedEmitterElement > 0)
        {
            for(int i = 0; i < layerProps.size(); i++)
            {
                GetLayerPropValue((lProps)i);
            }
        }
        
        propList->RefreshList();
        
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
        forcePreview->SetValue(Vector3(0, 0, 0));
        if(selectedEmitterElement == 0)
        {
            GetEmitterPropValue((eProps)selectedPropElement);
            if(selectedPropElement == 0)
                emitterTypeList->RefreshList();
            tip->SetText(LocalizedString("emitter." + emitterProps[selectedPropElement]));
        }
        if(selectedEmitterElement > 0)
        {
            GetLayerPropValue((lProps)selectedPropElement);
            
            tip->SetText(LocalizedString("layer." + layerProps[selectedPropElement]));
            
            if(selectedPropElement == 11 || selectedPropElement == 12|| selectedPropElement == 13)
            {
                HideAndResetEditFields();
                ShowForcesList();
            }
            if(selectedPropElement == 13)
                SafeRemoveControl(forcePreview);
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
            layers[selectedEmitterElement]->props.at(selectedAddPropElement)->isDefault = false;
            
            if(selectedAddPropElement == LAYER_FORCES)
                layers[selectedEmitterElement]->props.at(LAYER_FORCES_OVER_LIFE)->isDefault = false;
            
            if(selectedAddPropElement == LAYER_FORCES_OVER_LIFE)
                layers[selectedEmitterElement]->props.at(LAYER_FORCES)->isDefault = false;
            
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
        
        GetLayerPropValue((lProps)selectedPropElement);
    }
    
    if(forList == emitterTypeList)
    {
        selectedEmitterTypeElement = selectedCell->GetIndex();
        selectedCell->SetSelected(true);
        
        tip->SetText(LocalizedString("emitterType." + emitterTypes[selectedEmitterTypeElement]));
        
        SetEmitterPropValue(EMITTER_TYPE);
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
    
//    layers[0]->curLayerTime->SetText(Format(L"Emitter Time: %.2f", emitter->GetTime()));
//    for(int i = 1; i < layers.size(); i++)
//    {
//        if(emitter->GetLayers()[i])
//            layers[i]->curLayerTime->SetText(Format(L"Layer %d Time: %.2f", i, emitter->GetLayers()[i-1]->GetLayerTime()));
//    }
//
//    particleCountText->SetText(Format(L"Particle Count: %d", emitter->GetParticleCount()));
}

void TestScreen::Draw(const UIGeometricData &geometricData)
{

}

void TestScreen::SaveToYaml(const String &pathToFile)
{
    File *file = File::Create(pathToFile, File::WRITE|File::CREATE);
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector2> *vv;
    PropertyLineKeyframes<Vector2> *vk;
    PropertyLineValue<Vector3> *v3v;
    PropertyLineKeyframes<Vector3> *v3k;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    
    file->WriteLine("emitter:");
    
    if(emitter->GetIs3D())
        file->WriteLine("    3d: true");
    else
        file->WriteLine("    3d: false");
    
    int32 emitPropIndex = 0;
    
    if(emitter->type == ParticleEmitter::EMITTER_POINT)
        file->WriteLine("    type: point");
    if(emitter->type == ParticleEmitter::EMITTER_LINE)
        file->WriteLine("    type: line");
    if(emitter->type == ParticleEmitter::EMITTER_RECT)
        file->WriteLine("    type: rect");
    if(emitter->type == ParticleEmitter::EMITTER_ONCIRCLE)
        file->WriteLine("    type: oncirlce");
    emitPropIndex++;
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionAngle.Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionAngle.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[emitPropIndex], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[emitPropIndex], pv);
    emitPropIndex++;
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionRange.Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionRange.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[emitPropIndex], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[emitPropIndex], pv);
    emitPropIndex++;
    
    v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->emissionVector.Get());
    v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->emissionVector.Get());
    if(v3k)
        PrintPropKFValue(file, emitterProps[emitPropIndex], v3k);
    else if(v3v)
        PrintPropValue(file, emitterProps[emitPropIndex], v3v);
    emitPropIndex++;
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->radius.Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->radius.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[emitPropIndex], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[emitPropIndex], pv);
    emitPropIndex++;
    
    ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->colorOverLife.Get());
    cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->colorOverLife.Get());
    if(ck)
        PrintPropKFValue(file, emitterProps[emitPropIndex], ck);
    else if(cv)
        PrintPropValue(file, emitterProps[emitPropIndex], cv);
    emitPropIndex++;
    
    v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->size.Get());
    v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->size.Get());
    if(v3k)
        PrintPropKFValue(file, emitterProps[emitPropIndex], v3k);
    else if(v3v)
        PrintPropValue(file, emitterProps[emitPropIndex], v3v);
    emitPropIndex++;
    
    file->WriteLine(Format("    life: %f", emitter->GetLifeTime()));
    emitPropIndex++;
    
    for(int i = 0; i < emitter->GetLayers().size(); i++)
    {
        file->WriteLine(Format("layer%d:\n    type: layer", i));
        
        int32 propIndex = 0;
        
        file->WriteLine(Format("    %s: \"%s\"", layerProps[propIndex].c_str(), layers[i+1]->spritePath.c_str()));
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->life.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->life.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->lifeVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->lifeVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->number.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->number.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->numberVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->numberVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->size.Get());
        vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->size.Get());
        if(vk)
            PrintPropKFValue(file, layerProps[propIndex], vk);
        else if(vv)
            PrintPropValue(file, layerProps[propIndex], vv);
        propIndex++;
        
        vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers()[i]->sizeVariation.Get());
        vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers()[i]->sizeVariation.Get());
        if(vk)
            PrintPropKFValue(file, layerProps[propIndex], vk);
        else if(vv)
            PrintPropValue(file, layerProps[propIndex], vv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->sizeOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->sizeOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->velocity.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->velocity.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->velocityVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->velocityVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->velocityOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->velocityOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        int32 forceCount = emitter->GetLayers()[i]->forces.size();
        if(forceCount > 0)
            file->WriteLine(Format("    forceCount: %d", forceCount));
        
        for(int j = 0; j < forceCount; j++)
        {
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers()[i]->forces[j].Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers()[i]->forces[j].Get());
            if(v3k)
                PrintPropKFValue(file, Format("force%d", j), v3k);
            else if(v3v)
                PrintPropValue(file, Format("force%d", j), v3v);
        }
        for(int j = 0; j < emitter->GetLayers()[i]->forcesVariation.size(); j++)
        {   
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers()[i]->forcesVariation[j].Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers()[i]->forcesVariation[j].Get());
            if(v3k)
                PrintPropKFValue(file, Format("forceVariation%d", j), v3k);
            else if(v3v)
                PrintPropValue(file, Format("forceVariation%d", j), v3v);
        }
        for(int j = 0; j < emitter->GetLayers()[i]->forcesOverLife.size(); j++)
        {    
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->forcesOverLife[j].Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->forcesOverLife[j].Get());
            if(pk)
                PrintPropKFValue(file, Format("force%d", j), pk);
            else if(pv)
                PrintPropValue(file, Format("forceOverLife%d", j), pv);
            
            file->WriteLine("");
        }
        propIndex += 3;
        file->WriteLine("");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->spin.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->spin.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->spinVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->spinVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->spinOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->spinOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->motionRandom.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->motionRandom.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->motionRandomVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->motionRandomVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->motionRandomOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->motionRandomOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->bounce.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->bounce.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->bounceVariation.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->bounceVariation.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->bounceOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->bounceOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        file->WriteLine("");
        
        ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers()[i]->colorRandom.Get());
        cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers()[i]->colorRandom.Get());
        if(ck)
            PrintPropKFValue(file, layerProps[propIndex], ck);
        else if(cv)
            PrintPropValue(file, layerProps[propIndex], cv);
        propIndex++;
        
        pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->alphaOverLife.Get());
        pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->alphaOverLife.Get());
        if(pk)
            PrintPropKFValue(file, layerProps[propIndex], pk);
        else if(pv)
            PrintPropValue(file, layerProps[propIndex], pv);
        propIndex++;
        
        ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers()[i]->colorOverLife.Get());
        cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers()[i]->colorOverLife.Get());
        if(ck)
            PrintPropKFValue(file, layerProps[propIndex], ck);
        else if(cv)
            PrintPropValue(file, layerProps[propIndex], cv);
        propIndex++;
        file->WriteLine("");
    }
    SafeRelease(file);
}

void TestScreen::PrintPropValue(File *file, const String &propName, PropertyLineValue<float32> *pv)
{
    file->WriteLine(Format("    %s: %f", propName.c_str(), pv->GetValue(0)));
}

void TestScreen::PrintPropValue(File *file, const String &propName, PropertyLineValue<Vector2> *pv)
{
    Vector2 value = pv->GetValue(0);
    file->WriteLine(Format("    %s: [%f, %f]", propName.c_str(), value.x, value.y));
}

void TestScreen::PrintPropValue(File *file, const String &propName, PropertyLineValue<Vector3> *pv)
{
    Vector3 value = pv->GetValue(0);
    file->WriteLine(Format("    %s: [%f, %f, %f]", propName.c_str(), value.x, value.y, value.z));
}

void TestScreen::PrintPropValue(File *file, const String &propName, PropertyLineValue<Color> *pv)
{
    Color value = pv->GetValue(0);
    file->WriteLine(Format("    %s: [%d, %d, %d, %d]", propName.c_str(), (int32)(value.r*255), (int32)(value.g*255), (int32)(value.b*255), (int32)(value.a*255)));
}

void TestScreen::PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<float32> *pv)
{    
    String out = "    " + propName + ": [";    
    for(int i = 0; i < pv->keys.size(); i++)
    {
        out += Format("%f, %f, ", pv->keys[i].t, pv->keys[i].value);
    }
    out = out.substr(0, out.length()-2);
    out += "]";
    file->WriteLine(out);
}

void TestScreen::PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Vector2> *pv)
{
    String out = "    " + propName + ": [";    
    for(int i = 0; i < pv->keys.size(); i++)
    {
        out += Format("%f, [%f, %f], ", pv->keys[i].t, pv->keys[i].value.x, pv->keys[i].value.y);
    }
    
    out = out.substr(0, out.length()-2);
    out += "]";
    file->WriteLine(out);
}

void TestScreen::PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Vector3> *pv)
{
    String out = "    " + propName + ": [";    
    for(int i = 0; i < pv->keys.size(); i++)
    {
        out += Format("%f, [%f, %f, %f], ", pv->keys[i].t, pv->keys[i].value.x, pv->keys[i].value.y, pv->keys[i].value.z);
    }
    out = out.substr(0, out.length()-2);
    out += "]";
    file->WriteLine(out);
}

void TestScreen::PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Color> *pv)
{
    String out = "    " + propName + ": [";    
    for(int i = 0; i < pv->keys.size(); i++)
    {
        out += Format("%f, [%d, %d, %d, %d], ", pv->keys[i].t, (int32)(pv->keys[i].value.r*255), (int32)(pv->keys[i].value.g*255), (int32)(pv->keys[i].value.b*255), (int32)(pv->keys[i].value.a*255));
    }
    out = out.substr(0, out.length()-2);
    out += "]";
    file->WriteLine(out);   
}