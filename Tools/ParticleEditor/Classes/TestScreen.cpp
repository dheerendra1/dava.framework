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

String emitterProps[] = {"emissionDir", "emissionRage", "radius", "colorOverLife"};
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
float32 buttonW;
Rect tfPosSlider[2], tfPosKFEdit[2];
Rect colorViewPosSlider, colorViewPosKFEdit;
Rect kfValueTextPos[4], tfkfPos[4][2], tfkfTextPos[4][2];
PropertyLineKeyframes<Color> *curColorProp;
PropertyLineKeyframes<float32> *cur1DimProp;
PropertyLineKeyframes<Vector2> *cur2DimProp;
PropertyLineKeyframes<Vector3> *cur3DimProp;
int32 activeKFEdit;

void TestScreen::LoadResources()
{
    cellH = GetScreenHeight() / 25;
    buttonW = GetScreenWidth() / 6;
    float32 thumbSliderW = cellH/4;

    sprite = 0;

    cellFont = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    cellFont->SetSize((int32)(cellH/2.5f));
    cellFont->SetColor(Color(1, 1, 1, 1));

    selectedPropElement = -1;
    selectedEmitterElement = -1;
    selectedForceElement = -1;

    f = FTFont::Create("~res:/Fonts/MyriadPro-Regular.otf");
    f->SetSize(18);
    f->SetColor(Color(1,1,1,1));
    
    chooseProject = new UIButton(Rect(0, 0, buttonW/2, cellH));
    chooseProject->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    chooseProject->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    chooseProject->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    chooseProject->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    chooseProject->SetStateFont(UIControl::STATE_NORMAL, f);
    chooseProject->SetStateText(UIControl::STATE_NORMAL, L"Project");
	chooseProject->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(chooseProject);
    
    newEmitter = new UIButton(Rect(buttonW/2, 0, buttonW/2, cellH));
    newEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    newEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    newEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    newEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    newEmitter->SetStateText(UIControl::STATE_NORMAL, L"New");
	newEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(newEmitter);
    
	loadEmitter = new UIButton(Rect(buttonW, 0, buttonW/2, cellH));
    loadEmitter->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    loadEmitter->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    loadEmitter->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    loadEmitter->SetStateFont(UIControl::STATE_NORMAL, f);
    loadEmitter->SetStateText(UIControl::STATE_NORMAL, L"Load");
	loadEmitter->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(loadEmitter);
    
    saveEmitter = new UIButton(Rect(buttonW*3/2, 0, buttonW/2, cellH));
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
    
    cloneLayer = new UIButton(Rect(0, cellH*2, buttonW/2, cellH));
    cloneLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    cloneLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    cloneLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    cloneLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    cloneLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    cloneLayer->SetStateText(UIControl::STATE_NORMAL, L"copy L");
	cloneLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(cloneLayer);
    
    disableLayer = new UIButton(Rect(buttonW/2, cellH*2, buttonW/2, cellH));
    disableLayer->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    disableLayer->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.0, 0.0, 0.0, 0.5));
    disableLayer->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    disableLayer->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.5, 0.5, 0.5, 0.5));
    disableLayer->SetStateFont(UIControl::STATE_NORMAL, f);
    disableLayer->SetStateText(UIControl::STATE_NORMAL, L"on/off L");
	disableLayer->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &TestScreen::ButtonPressed));
    AddControl(disableLayer);
    
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

    emitterList = new UIList(Rect(0, cellH*3, buttonW, cellH*5), UIList::ORIENTATION_VERTICAL);
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
        
        tfv[i] = new UITextField(Rect(buttonW*11/8, cellH*(9+i), buttonW/4, cellH/2));
        tfv[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfv[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfv[i]->SetFont(cellFont);
        tfv[i]->SetDelegate(this);
        AddControl(tfv[i]);
        
        valueText[i] = new UIStaticText(Rect(buttonW*10/8, cellH*(9+i), buttonW/8, cellH/2));
        valueText[i]->SetFont(cellFont);
        valueText[i]->SetAlign(DAVA::ALIGN_LEFT);
        AddControl(valueText[i]);
        
        propEdit[i] = new PropertyLineEditControl();
        propEdit[i]->SetRect(Rect(buttonW, cellH*(9+i*3), buttonW*5/6, cellH*3));
        propEdit[i]->SetDelegate(this);
        AddControl(propEdit[i]);
        
        kfValueTextPos[i] = Rect(buttonW, cellH*(12.5f+i*3), buttonW, cellH/2);
        tfkfPos[i][0] = Rect(buttonW*9/8, cellH*(12+i*3), buttonW/4, cellH/2);
        tfkfPos[i][1] = Rect(buttonW*12/8, cellH*(12+i*3), buttonW/4, cellH/2);
        tfkfTextPos[i][0] = Rect(buttonW, cellH*(12+i*3), buttonW/8, cellH/2);
        tfkfTextPos[i][1] = Rect(buttonW*11/8, cellH*(12+i*3), buttonW/8, cellH/2);
    }
    
    kfValueText = new UIStaticText(kfValueTextPos[0]);
    kfValueText->SetFont(cellFont);
    kfValueText->SetAlign(DAVA::ALIGN_LEFT);
    AddControl(kfValueText);
    
    for(int i = 0; i < 2; i++)
    {
        tfkf[i] = new UITextField(tfkfPos[0][i]);
        tfkf[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tfkf[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tfkf[i]->SetFont(cellFont);
        tfkf[i]->SetDelegate(this);
        AddControl(tfkf[i]);
        
        tfkfText[i] = new UIStaticText(tfkfTextPos[0][i]);
        tfkfText[i]->SetFont(cellFont);
        AddControl(tfkfText[i]); 
    }
    
    tfkfText[0]->SetText(L"T:");
    tfkfText[1]->SetText(L"V:");
    
    tfPosSlider[0] = Rect(buttonW, cellH*(9.5f), buttonW/6, cellH/2);
    tfPosSlider[1] = Rect(buttonW*11/6, cellH*(9.5f), buttonW/6, cellH/2);
    
    tfPosKFEdit[0] = Rect(buttonW*11/6, cellH*(11.5f), buttonW/6, cellH/2);
    tfPosKFEdit[1] = Rect(buttonW*11/6, cellH*(9), buttonW/6, cellH/2);
    
    for(int i = 0; i < 2; i++)
    {
        tf[i] = new UITextField(tfPosSlider[i]);
        tf[i]->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        tf[i]->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.75));
        tf[i]->SetFont(cellFont);
        tf[i]->SetDelegate(this);
        AddControl(tf[i]);
    }
    
    fsDlg = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlg->SetDelegate(this);
    Vector<String> filter;
    filter.push_back(".yaml");
    filter.push_back(".YAML");
    fsDlg->SetExtensionFilter(filter);
    fsDlg->SetTitle(L"Loading from .yaml file");
    fsDlg->SetCurrentDir("~res:/");
    
    fsDlgSprite = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlgSprite->SetDelegate(this);
    Vector<String> filter2;
    filter2.push_back(".txt");
    filter2.push_back(".TXT");
    fsDlgSprite->SetExtensionFilter(filter2);
    fsDlgSprite->SetTitle(L"Selecte Sprite");
    fsDlgSprite->SetCurrentDir("~res:/");

    fsDlgProject = new UIFileSystemDialog("~res:/Fonts/MyriadPro-Regular.otf");
    fsDlgProject->SetDelegate(this);
    fsDlgProject->SetOperationType(UIFileSystemDialog::OPERATION_CHOOSE_DIR);
    fsDlgProject->SetTitle(L"Choose Project Folder");

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

    preview = new PreviewControl();
    preview->SetRect(Rect(buttonW*2, 0, GetScreenWidth() - buttonW*2, GetScreenHeight()));
    AddControl(preview);
    
    emitter = new ParticleEmitter();
    layers.push_back(Layer(emitterProps, emitterPropsCount, "", cellFont));
    layers.at(0).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
    AddControl(layers.at(0).curLayerTime);
    preview->SetEmitter(emitter);
    
    layers[0].props[0].minValue = 0;
    layers[0].props[0].maxValue = 360;
    layers[0].props[1].minValue = 0;
    layers[0].props[1].maxValue = 360;
    
    forcePreview = new ForcePreviewControl();
    forcePreview->SetRect(Rect(buttonW, cellH*(15.5f), buttonW, buttonW*1.125f));
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
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forces[selectedForceElement].Get()->GetValue(curPropEditTime));
        if(selectedPropElement == 12)
            forcePreview->SetValue(emitter->GetLayers()[selectedEmitterElement-1]->forcesVariation[selectedForceElement].Get()->GetValue(curPropEditTime));
    }
}

void TestScreen::TextFieldShouldReturn(UITextField * textField)
{
    int32 value;
    if(textField == tf[0])
    {
        value = layers[selectedEmitterElement].props[selectedPropElement].minValue;
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMinValue(value);
            propEdit[i]->SetMinY(value);
        }
        swscanf(textField->GetText().c_str(), L"%d", &value);
        layers[selectedEmitterElement].props[selectedPropElement].minValue = value;
    }
    if(textField == tf[1])
    {
        value = layers[selectedEmitterElement].props[selectedPropElement].maxValue;
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMaxValue(value);
            propEdit[i]->SetMaxY(value);
        }
        swscanf(textField->GetText().c_str(), L"%d", &value);
        layers[selectedEmitterElement].props[selectedPropElement].maxValue = value;
    }
    
    for(int i = 0; i < 4; i++)
    {
        if(textField == tfv[i])
        {
            float32 v = 0;
            swscanf(textField->GetText().c_str(), L"%f", &v);
            vSliders[i]->SetValue(v);
        }
    }
    
    if(textField == tfkf[0])
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

    if(textField == tfkf[1])
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
        SetEmitterPropValue(selectedPropElement);
        GetEmitterPropValue(selectedPropElement);
    }
    if(selectedEmitterElement > 0)
    {
        SetLayerPropValue(selectedPropElement);
        GetLayerPropValue(selectedPropElement);
    }    
}

bool TestScreen::TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString)
{
    int v;
    if(replacementLength == -1 || replacementString == L"-")
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
        selectedEmitterElement = -1;
        selectedPropElement = -1;
        selectedForceElement = -1;
        forcePreview->SetValue(Vector3(0, 0, 0));
        
        SafeRelease(emitter);
        emitter = new ParticleEmitter();
        preview->SetEmitter(emitter);
        for(int i = 0; i < layers.size(); i++)
        {
            RemoveControl(layers[i].curLayerTime);
            SafeRelease(layers[i].curLayerTime);
        }
        layers.clear();
        layers.push_back(Layer(emitterProps, emitterPropsCount, "", cellFont));
        layers.at(0).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
        AddControl(layers.at(0).curLayerTime);
        
        layers[0].props[0].minValue = 0;
        layers[0].props[0].maxValue = 360;
        layers[0].props[1].minValue = 0;
        layers[0].props[1].maxValue = 360;
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
        
        layers.push_back(Layer(layerProps, layerPropsCount, "", cellFont));
        layers.at(layers.size()-1).spritePath = "";
        layers.at(layers.size()-1).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()-1), buttonW, cellH));
        AddControl(layers.at(layers.size()-1).curLayerTime);        
        
        emitterList->RefreshList();
        propList->RefreshList();
    }
    if(obj == delLayer)
    {
        if(selectedEmitterElement > 0)
        {
            RemoveControl(layers[selectedEmitterElement].curLayerTime);
            SafeRelease(layers[selectedEmitterElement].curLayerTime);
            
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
            
            Layer l = layers[selectedEmitterElement].Clone();
            layers.push_back(l);
            layers.at(layers.size()-1).spritePath = "";
            layers.at(layers.size()-1).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()-1), buttonW, cellH));
            AddControl(layers.at(layers.size()-1).curLayerTime);
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
            layers[selectedEmitterElement].isDisabled = !disabled;
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
        if(selectedPropElement == 11 || selectedPropElement == 12 || selectedPropElement == 13)
        {
            emitter->GetLayers().at(selectedEmitterElement-1)->forces.push_back(RefPtr<PropertyLine<Vector3> >(0));
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.push_back(RefPtr<PropertyLine<Vector3> >(0));
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.push_back(RefPtr<PropertyLine<float32> >(0));
        }
        HideAndResetEditFields();
        selectedForceElement = -1;
        forcePreview->SetValue(Vector3(0, 0, 0));
        forcesList->RefreshList();
    }
    if(obj == delForce)
    {
        if(selectedForceElement >= 0)
        {
            if(selectedPropElement == 11 || selectedPropElement == 12 || selectedPropElement == 13)
            {
                emitter->GetLayers().at(selectedEmitterElement-1)->forces.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forces.begin() + selectedForceElement);
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.begin() + selectedForceElement);
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.erase(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.begin() + selectedForceElement);
            }
            HideAndResetEditFields();
            forcesList->RefreshList();
            selectedForceElement = -1;
            forcePreview->SetValue(Vector3(0, 0, 0));
        }
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
                layers[selectedEmitterElement].props[id].minValue = 0;
                layers[selectedEmitterElement].props[id].maxValue = value*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement].props[id].minValue = value*2;
                layers[selectedEmitterElement].props[id].maxValue = abs(value);
            }
            
            if(selectedEmitterElement == 0 && (id == 0 || id == 1))
            {
                layers[selectedEmitterElement].props[id].minValue = 0;
                layers[selectedEmitterElement].props[id].maxValue = 360;
            }

            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        
        vSliders[0]->SetVisible(true);
        valueText[0]->SetVisible(true);
        tfv[0]->SetVisible(true);
        vSliders[0]->SetMaxValue(max);
        vSliders[0]->SetMinValue(min);
        vSliders[0]->SetValue(pv->GetValue(0));
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosSlider[i]);
        }
        tf[0]->SetText(Format(L"%d", min));
        tf[1]->SetText(Format(L"%d", max));
        
        tfv[0]->SetText(Format(L"%.2f", pv->GetValue(0)));
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
            float32 maxV = 1, minV = 0;
            for(int i = 0; i < pk->keys.size(); i++)
            {
                minV = Min(minV, pk->keys[i].value);
                maxV = Max(maxV, pk->keys[i].value);
            }
            layers[selectedEmitterElement].props[id].minValue = minV;
            layers[selectedEmitterElement].props[id].maxValue = maxV;
            
            if(selectedEmitterElement == 0 && (id == 0 || id == 1))
            {
                layers[selectedEmitterElement].props[id].minValue = 0;
                layers[selectedEmitterElement].props[id].maxValue = 360;
            }
            
            curPropType = false;
            return true;
        }
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        propEdit[0]->SetVisible(true);
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosKFEdit[i]);
        }
        
        for(int i = 0; i < 2; i++)
        {
            tfkf[i]->SetRect(tfkfPos[0][i]);
            tfkfText[i]->SetRect(tfkfTextPos[0][i]);
        }
        kfValueText->SetVisible(true);
        kfValueText->SetRect(kfValueTextPos[0]);
        kfValueText->SetText(Format(L" t = 0.00 : %.2f", pk->GetValue(0)));
        cur1DimProp = pk;
        
        tf[0]->SetText(Format(L"%d", min));
        tf[1]->SetText(Format(L"%d", max));
        
        propEdit[0]->SetMinY(min);
        propEdit[0]->SetMaxY(max);
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
                    layers[selectedEmitterElement].props[id].minValue = 0;
                else
                    layers[selectedEmitterElement].props[id].minValue = min*2;
                layers[selectedEmitterElement].props[id].maxValue = max*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement].props[id].minValue = min*2;
                layers[selectedEmitterElement].props[id].maxValue = 0;
            }
            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        for(int i = 0; i < 2; i++)
        {
            vSliders[i]->SetVisible(true);
            tfv[i]->SetVisible(true);
            valueText[i]->SetVisible(true);
        }
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosSlider[i]);
        }
        tf[0]->SetText(Format(L"%d", min));
        tf[1]->SetText(Format(L"%d", max));
        
        for(int i = 0; i < 2; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[0]->SetValue(vv->GetValue(0).x);
        vSliders[1]->SetValue(vv->GetValue(0).y);
        
        tfv[0]->SetText(Format(L"%.2f", vv->GetValue(0).x));
        tfv[1]->SetText(Format(L"%.2f", vv->GetValue(0).y));
        
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
            float32 maxV1 = 1, minV1 = 0, maxV2 = 1, minV2 = 0;
            for(int i = 0; i < vk->keys.size(); i++)
            {
                minV1 = Min(minV1, vk->keys[i].value.x);
                minV2 = Min(minV2, vk->keys[i].value.x);
                maxV1 = Min(maxV1, vk->keys[i].value.y);
                maxV2 = Min(maxV2, vk->keys[i].value.y);
            }
            layers[selectedEmitterElement].props[id].minValue = Min(minV1, minV2);
            layers[selectedEmitterElement].props[id].maxValue = Max(maxV1, maxV2);
            
            curPropType = false;
            return true;
        }
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        
        for(int i = 0; i < 2; i++)
        {
            propEdit[i]->SetVisible(true);
        }
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosKFEdit[i]);
        }
        
        for(int i = 0; i < 2; i++)
        {
            tfkf[i]->SetRect(tfkfPos[1][i]);
            tfkfText[i]->SetRect(tfkfTextPos[1][i]);
        }
        
        kfValueText->SetVisible(true);
        kfValueText->SetRect(kfValueTextPos[1]);
        kfValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f)", vk->GetValue(0).x, vk->GetValue(0).y));
        cur2DimProp = vk;
        
        tf[0]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].minValue));
        tf[1]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].maxValue));
        
        for(int i = 0; i < 2; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(max);
            propEdit[i]->SetMinY(min);
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
            float32 max = Max(vv->GetValue(0).x, vv->GetValue(0).y);
            float32 min = Min(vv->GetValue(0).x, vv->GetValue(0).y);
            if(max >= 0)
            {
                if(min > 0)
                    layers[selectedEmitterElement].props[id].minValue = 0;
                else
                    layers[selectedEmitterElement].props[id].minValue = min*2;
                layers[selectedEmitterElement].props[id].maxValue = max*2 + 1;
            }
            else
            {
                layers[selectedEmitterElement].props[id].minValue = min*2;
                layers[selectedEmitterElement].props[id].maxValue = 0;
            }
            curPropType = false;
            return true;
        }
        
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        
        int n = 2;
        if(emitter->GetIs3D())
            n = 3;
        for(int i = 0; i < n; i++)
        {
            vSliders[i]->SetVisible(true);
            tfv[i]->SetVisible(true);
            valueText[i]->SetVisible(true);
        }
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosSlider[i]);
        }
        tf[0]->SetText(Format(L"%d", min));
        tf[1]->SetText(Format(L"%d", max));
        
        for(int i = 0; i < 3; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[0]->SetValue(vv->GetValue(0).x);
        vSliders[1]->SetValue(vv->GetValue(0).y);
        vSliders[2]->SetValue(vv->GetValue(0).z);
        
        tfv[0]->SetText(Format(L"%.2f", vv->GetValue(0).x));
        tfv[1]->SetText(Format(L"%.2f", vv->GetValue(0).y));
        tfv[2]->SetText(Format(L"%.2f", vv->GetValue(0).z));
        
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
            float32 maxV1 = 1, minV1 = 0, maxV2 = 1, minV2 = 0;
            for(int i = 0; i < vk->keys.size(); i++)
            {
                minV1 = Min(minV1, vk->keys[i].value.x);
                minV2 = Min(minV2, vk->keys[i].value.x);
                maxV1 = Min(maxV1, vk->keys[i].value.y);
                maxV2 = Min(maxV2, vk->keys[i].value.y);
            }
            layers[selectedEmitterElement].props[id].minValue = Min(minV1, minV2);
            layers[selectedEmitterElement].props[id].maxValue = Max(maxV1, maxV2);
            
            curPropType = false;
            return true;
        }
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        
        int n = 2;
        if(emitter->GetIs3D())
            n = 3;
        
        for(int i = 0; i < n; i++)
        {
            propEdit[i]->SetVisible(true);
        }
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosKFEdit[i]);
        }
        
        for(int i = 0; i < 2; i++)
        {
            tfkf[i]->SetRect(tfkfPos[n-1][i]);
            tfkfText[i]->SetRect(tfkfTextPos[n-1][i]);
        }
        
        kfValueText->SetVisible(true);
        kfValueText->SetRect(kfValueTextPos[n-1]);
        kfValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f, %.2f)", vk->GetValue(0).x, vk->GetValue(0).y, vk->GetValue(0).z));
        cur3DimProp = vk;
        
        tf[0]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].minValue));
        tf[1]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].maxValue));
        
        for(int i = 0; i < 3; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(max);
            propEdit[i]->SetMinY(min);
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
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetVisible(true);
            tfv[i]->SetVisible(true);
            valueText[i]->SetVisible(true);
        }
        colorView->SetVisible(true);
        colorView->SetRect(colorViewPosSlider);
        Color c = cv->GetValue(0);
        c.a = 1.0f;
        colorView->GetBackground()->SetColor(c);
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosSlider[i]);
        }
        tf[0]->SetText(Format(L"%d", min));
        tf[1]->SetText(Format(L"%d", max));
        
        for(int i = 0; i < 4; i++)
        {
            vSliders[i]->SetMinValue(min);
            vSliders[i]->SetMaxValue(max);
        }
        
        vSliders[3]->SetValue(cv->GetValue(0).a);
        vSliders[2]->SetValue(cv->GetValue(0).b);
        vSliders[1]->SetValue(cv->GetValue(0).g);
        vSliders[0]->SetValue(cv->GetValue(0).r);
        
        tfv[0]->SetText(Format(L"%.2f", cv->GetValue(0).r));
        tfv[1]->SetText(Format(L"%.2f", cv->GetValue(0).g));
        tfv[2]->SetText(Format(L"%.2f", cv->GetValue(0).b));
        tfv[3]->SetText(Format(L"%.2f", cv->GetValue(0).a));
        
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
        int32 min = layers[selectedEmitterElement].props[id].minValue;
        int32 max = layers[selectedEmitterElement].props[id].maxValue;
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->SetVisible(true);
        }
        for(int i = 0; i < 2; i++)
        {
            tf[i]->SetVisible(true);
            tf[i]->SetRect(tfPosKFEdit[i]);
        }
        colorView->SetVisible(true);
        colorView->SetRect(colorViewPosKFEdit);
        Color c = ck->GetValue(0);
        c.a = 1.0f;
        colorView->GetBackground()->SetColor(c);
        curColorProp = ck;
        
        for(int i = 0; i < 2; i++)
        {
            tfkf[i]->SetRect(tfkfPos[3][i]);
            tfkfText[i]->SetRect(tfkfTextPos[3][i]);
        }
        
        kfValueText->SetVisible(true);
        kfValueText->SetRect(kfValueTextPos[3]);
        kfValueText->SetText(Format(L" t = 0.00 : (%.2f, %.2f, %.2f, %.2f)", ck->GetValue(0).r, ck->GetValue(0).g, ck->GetValue(0).b, ck->GetValue(0).a));
        
        tf[0]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].minValue));
        tf[1]->SetText(Format(L"%d", layers[selectedEmitterElement].props[id].maxValue));
        
        for(int i = 0; i < 4; i++)
        {
            propEdit[i]->GetValues().clear();
            propEdit[i]->SetMaxY(max);
            propEdit[i]->SetMinY(min);
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

void TestScreen::GetEmitterPropValue(int32 id, bool getLimits)
{
    valueBut->SetVisible(true);
    KFBut->SetVisible(true);
    
    cur1DimProp = 0;
    cur2DimProp = 0;
    cur3DimProp = 0;
    curColorProp = 0;
    
    PropertyLineValue<float32> *pv;
    PropertyLineKeyframes<float32> *pk;
    PropertyLineValue<Vector3> *vv;
    PropertyLineKeyframes<Vector3> *vk;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    switch (id) {
        case 0:
            vk = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->emissionAngle.Get());
            vv = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->emissionAngle.Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(vk, id, getLimits))
                if(!GetProp(vv, id, getLimits))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 1:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionRange.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionRange.Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 2:
            pk = dynamic_cast< PropertyLineKeyframes<float> *>(emitter->radius.Get());
            pv = dynamic_cast< PropertyLineValue<float> *>(emitter->radius.Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(0).props.at(id).isDefault = true;
            break;
            
        case 3:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->colorOverLife.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->colorOverLife.Get());
            layers.at(0).props.at(id).isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
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
        layers[0].props[id].minValue = 0;
        layers[0].props[id].maxValue = 1;
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
        case 0:
            SafeRelease(valueDim2);
            SafeRelease(valueDim1);
            SafeRelease(valueDim4);
            emitter->emissionAngle.Set(valueDim3);
            break;
            
        case 1:
            SafeRelease(valueDim2);
            SafeRelease(valueDim4);
            SafeRelease(valueDim3);
            emitter->emissionRange.Set(valueDim1);      
            break;
           
        case 2:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->radius.Set(valueDim1);
            break;
            
        case 3:
            SafeRelease(valueDim2);
            SafeRelease(valueDim1);
            SafeRelease(valueDim3);
            emitter->colorOverLife.Set(valueDim4);
            break;
            
        default:
            break;
    }    
}

void TestScreen::ResetEmitterPropValue(int32 id)
{
    switch (id) {
        case 0:
            emitter->emissionAngle.Set(0);
            break;
            
        case 1:
            emitter->emissionRange.Set(0);      
            break;
            
        case 2:
            emitter->radius.Set(0);
            break;
            
        case 3:
            emitter->colorOverLife.Set(0);
            break;
            
        default:
            break;
    }        
}

void TestScreen::ResetLayerPropValue(int32 id)
{
    switch (id) {
        case 0:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(0);
            break;
        case 1:
            emitter->GetLayers().at(selectedEmitterElement-1)->life.Set(0);
            break;
            
        case 2:
            emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Set(0);       
            break;
            
        case 3:
            emitter->GetLayers().at(selectedEmitterElement-1)->number.Set(0);     
            break;
            
        case 4:
            emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Set(0);
            break;
            
        case 5:
            emitter->GetLayers().at(selectedEmitterElement-1)->size.Set(0);
            break;
            
        case 6:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Set(0);      
            break;
            
        case 7:
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Set(0);     
            break;
            
        case 8:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Set(0);      
            break;
            
        case 9:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Set(0);      
            break;
            
        case 10:
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Set(0);     
            break;
            
        case 11:
            emitter->GetLayers().at(selectedEmitterElement-1)->forces.clear();
            break;
            
        case 12:
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.clear();
            break;
            
        case 13:
            emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.clear();
            break;
            
        case 14:
            emitter->GetLayers().at(selectedEmitterElement-1)->spin.Set(0);    
            break;
            
        case 15:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Set(0);
            break;
            
        case 16:
            emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Set(0);     
            break;
            
        case 17:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Set(0);    
            break;
            
        case 18:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Set(0);     
            break;
            
        case 19:
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Set(0);      
            break;
            
        case 20:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Set(0);       
            break;
            
        case 21:
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Set(0);      
            break;
            
        case 22:emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Set(0);    
            break;
            
        case 23:emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Set(0);      
            break;
            
        case 24:
            emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Set(0);      
            break;
            
        case 25:
            emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Set(0);  
            break;
            
        default:
            break;
    }    

}

void TestScreen::GetLayerPropValue(int32 id, bool getLimits)
{
    if(id > 0)
    {
        valueBut->SetVisible(true);
        KFBut->SetVisible(true);
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
        case 0:
            sprite = emitter->GetLayers().at(selectedEmitterElement-1)->GetSprite();
            spriteControl->SetSprite(sprite, 0);
            if(sprite)
                layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            spritePanel->SetVisible(true);
            if(sprite)
                spriteInfo->SetText(Format(L"%.0fx%.0f@%d", sprite->GetWidth(), sprite->GetHeight(), sprite->GetFrameCount()));
            else
                spriteInfo->SetText(L"");
            break;
            
        case 1:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->life.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->life.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 2:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 3:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->number.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->number.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;   
            break;
            
        case 4:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 5:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->size.Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->size.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(vk, id, getLimits))
                if(!GetProp(vv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true; 
            break;
            
        case 6:
            vk = dynamic_cast< PropertyLineKeyframes<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Get());
            vv = dynamic_cast< PropertyLineValue<Vector2> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(vk, id, getLimits))
                if(!GetProp(vv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 7:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;    
            break;
            
        case 8:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;     
            break;
            
        case 9:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;    
            break;
            
        case 10:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 11:
            
        case 12:
            
        case 13:
            if(selectedForceElement >= 0)
                GetForcesValue(selectedForceElement);
            break;
            
        case 14:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spin.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spin.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;   
            break;
            
        case 15:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 16:
            
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;  
            break;
            
        case 17:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 18:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 19:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 20:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 21:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 22:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 23:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 24:
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(pk, id, getLimits))
                if(!GetProp(pv, id, getLimits))
                    layers.at(selectedEmitterElement).props.at(id).isDefault = true;
            break;
            
        case 25:
            ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Get());
            cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Get());
            layers.at(selectedEmitterElement).props.at(id).isDefault = false;
            if(!GetProp(ck, id, getLimits))
                if(!GetProp(cv, id, getLimits))
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
        layers[0].props[id].minValue = 0;
        layers[0].props[id].maxValue = 1;
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
        case 0:
            emitter->GetLayers().at(selectedEmitterElement-1)->SetSprite(sprite);
            break;
            
        case 1:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->life.Set(valueDim1);
            break;
            
        case 2:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->lifeVariation.Set(valueDim1);       
            break;
            
        case 3:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->number.Set(valueDim1);     
            break;
            
        case 4:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->numberVariation.Set(valueDim1);
            break;
            
        case 5:
            SafeRelease(valueDim1);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->size.Set(valueDim2);
            break;
            
        case 6:
            SafeRelease(valueDim1);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeVariation.Set(valueDim2);      
            break;
            
        case 7:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->sizeOverLife.Set(valueDim1);     
            break;
            
        case 8:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->velocity.Set(valueDim1);      
            break;
            
        case 9:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityVariation.Set(valueDim1);      
            break;
            
        case 10:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->velocityOverLife.Set(valueDim1);     
            break;
            
        case 11:
            SafeRelease(valueDim1);
            SafeRelease(valueDim2);
            SafeRelease(valueDim4);
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(selectedForceElement).Set(valueDim3);
            else
                SafeRelease(valueDim3);
            break;
            
        case 12:
            SafeRelease(valueDim1);
            SafeRelease(valueDim2);
            SafeRelease(valueDim4);
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(selectedForceElement).Set(valueDim3);
            else
                SafeRelease(valueDim3);
            break;
            
        case 13:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            if(selectedForceElement >= 0)
                emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(selectedForceElement).Set(valueDim1);
            else
                SafeRelease(valueDim1);
            break;
            
        case 14:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->spin.Set(valueDim1);    
            break;
            
        case 15:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->spinVariation.Set(valueDim1);
            break;
            
        case 16:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->spinOverLife.Set(valueDim1);     
            break;
            
        case 17:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandom.Set(valueDim1);    
            break;
            
        case 18:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomVariation.Set(valueDim1);     
            break;
            
        case 19:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->motionRandomOverLife.Set(valueDim1);      
            break;
            
        case 20:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->bounce.Set(valueDim1);       
            break;
            
        case 21:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceVariation.Set(valueDim1);      
            break;
            
        case 22:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->bounceOverLife.Set(valueDim1);    
            break;
            
        case 23:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim1);
            emitter->GetLayers().at(selectedEmitterElement-1)->colorRandom.Set(valueDim4);      
            break;
            
        case 24:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim4);
            emitter->GetLayers().at(selectedEmitterElement-1)->alphaOverLife.Set(valueDim1);      
            break;
            
        case 25:
            SafeRelease(valueDim2);
            SafeRelease(valueDim3);
            SafeRelease(valueDim1);
            emitter->GetLayers().at(selectedEmitterElement-1)->colorOverLife.Set(valueDim4);  
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
        PropertyLineKeyframes<Vector3> *vk = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(id).Get());
        PropertyLineValue<Vector3> *vv = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forces.at(id).Get());
        if(vv)
        {
            GetProp(vv, 11);
            forcePreview->SetValue(vv->GetValue(0));
        }
        else if(vk)
        {
            GetProp(vk, 11);
            forcePreview->SetValue(vk->GetValue(0));
        }    
    }
    if (selectedPropElement == 12)
    {
        PropertyLineKeyframes<Vector3> *vk = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(id).Get());
        PropertyLineValue<Vector3> *vv = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.at(id).Get());
        if(vv)
        {
            GetProp(vv, 12);
            forcePreview->SetValue(vv->GetValue(0));
        }
        else if(vk)
        {
            GetProp(vk, 12);
            forcePreview->SetValue(vk->GetValue(0));
        }   
    }
    if (selectedPropElement == 13)
    {
        PropertyLineKeyframes<float32> *vk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(id).Get());
        PropertyLineValue<float32> *vv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.at(id).Get());
        if(vv)
        {
            GetProp(vv, 13);
        }
        else if(vk)
        {
            GetProp(vk, 13);
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
            tfkf[i]->SetVisible(false);
            tfkfText[i]->SetVisible(false);
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
            tfkf[i]->SetVisible(true);
            tfkfText[i]->SetVisible(true);
        }
        
        tfkf[0]->SetText(Format(L"%.2f", value.x));
        tfkf[1]->SetText(Format(L"%.2f", value.y));
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
        kfValueText->SetText(Format(L" t = %.2f : %.2f", t, cur1DimProp->GetValue(t)));
    }
    
    if(cur2DimProp)
    {
        kfValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f)", t, cur2DimProp->GetValue(t).x, cur2DimProp->GetValue(t).y));
    }
    
    if(cur3DimProp)
    {
        kfValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f, %.2f)", t, cur3DimProp->GetValue(t).x, cur3DimProp->GetValue(t).y, cur3DimProp->GetValue(t).z));
    }
    
    if(curColorProp)
    {
        Color c = curColorProp->GetValue(t);
        kfValueText->SetText(Format(L" t = %.2f : (%.2f, %.2f, %.2f, %.2f)", t, c.r, c.g, c.b, c.a));
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
                RemoveControl(layers[i].curLayerTime);
                SafeRelease(layers[i].curLayerTime);
            }
            layers.clear();
            layers.push_back(Layer(emitterProps, emitterPropsCount, "", cellFont));
            layers.at(0).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, 0, buttonW, cellH));
            AddControl(layers.at(0).curLayerTime);
            selectedEmitterElement = 0;
            for(int i = 0; i < emitterPropsCount; i++)
            {
                GetEmitterPropValue(i, true);
            }
            for(int i = 0; i < emitter->GetLayers().size(); i++)
            {
                String spritePath = emitter->GetLayers()[i]->GetSprite()->GetName();
                layers.push_back(Layer(layerProps, layerPropsCount, spritePath.substr(0, spritePath.size()-4), cellFont));
                layers.at(layers.size()-1).curLayerTime->SetRect(Rect(GetScreenWidth() - buttonW, cellH*(layers.size()-1), buttonW, cellH));
                AddControl(layers.at(layers.size()-1).curLayerTime);
                for(int j = 0; j < layerPropsCount; j++)
                {
                    selectedEmitterElement = i + 1;
                    GetLayerPropValue(j, true);
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

        layers[selectedEmitterElement].spritePath = path;
        
        sprite = Sprite::Create(path);
        SetLayerPropValue(0);
        GetLayerPropValue(0);
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
        tfv[i]->SetVisible(false);
        valueText[i]->SetVisible(false);
        vSliders[i]->SetValue(0.0f);
        propEdit[i]->Reset();
    }
    tf[0]->SetVisible(false);
    tf[1]->SetVisible(false);
    KFBut->SetVisible(false);
    valueBut->SetVisible(false);
    
    spritePanel->SetVisible(false);
    
    colorView->SetVisible(false);
    kfValueText->SetVisible(false);
    tfkf[0]->SetVisible(false);
    tfkf[1]->SetVisible(false);
    tfkfText[0]->SetVisible(false);
    tfkfText[1]->SetVisible(false);
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
    cloneLayer->SetDisabled(true);
    disableLayer->SetDisabled(true);
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
    cloneLayer->SetDisabled(false);
    disableLayer->SetDisabled(false);
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
    SafeRelease(chooseProject);
    
    for(int i = 0 ;i < 4; i++)
    {
        SafeRelease(tfv[i]);
        SafeRelease(valueText[i]);
        SafeRelease(vSliders[i]);
        SafeRelease(propEdit[i]);
    }
    
    for(int i = 0; i < 2; i++)
    {
        SafeRelease(tfkfText[i]);
        SafeRelease(tfkf[i]);
        SafeRelease(tf[i]);
    }
    
    SafeRelease(spriteInfo);
    SafeRelease(particleCountText);
    SafeRelease(kfValueText);
    
    SafeRelease(emitterList);
    SafeRelease(propList);
    SafeRelease(addPropList);
    SafeRelease(forcesList);
    
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
                n = emitter->GetLayers().at(selectedEmitterElement-1)->forces.size();
                break;
                
            case 12:
                n = emitter->GetLayers().at(selectedEmitterElement-1)->forcesVariation.size();
                break;
                
            case 13:
                n = emitter->GetLayers().at(selectedEmitterElement-1)->forcesOverLife.size();
                break;
            default:
                break;
        }
    }
    return n;
}

int32 TestScreen::CellHeight(UIList *forList, int32 index)
{
    return cellH*2/3;
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
            if(layers[index].isDisabled)
            {
                c->SetStateText(UIListCell::STATE_NORMAL, Format(L"Layer %d /Disabled", index));
            }
            else
            {
                c->SetStateText(UIListCell::STATE_NORMAL, Format(L"Layer %d", index));
            }
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
        forcePreview->SetValue(Vector3(0, 0, 0));
        
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
        forcePreview->SetValue(Vector3(0, 0, 0));
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
    
//    layers[0].curLayerTime->SetText(Format(L"Emitter Time: %.2f", emitter->GetTime()));
//    for(int i = 1; i < layers.size(); i++)
//    {
//        if(emitter->GetLayers()[i])
//            layers[i].curLayerTime->SetText(Format(L"Layer %d Time: %.2f", i, emitter->GetLayers()[i-1]->GetLayerTime()));
//    }
//
//    particleCountText->SetText(Format(L"Particle Count: %d", emitter->GetParticleCount()));
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
    PropertyLineValue<Vector3> *v3v;
    PropertyLineKeyframes<Vector3> *v3k;
    PropertyLineValue<Color> *cv;
    PropertyLineKeyframes<Color> *ck;
    
    fprintf(file, "emitter:\n");
    v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->emissionAngle.Get());
    v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->emissionAngle.Get());
    if(v3k)
        PrintPropKFValue(file, emitterProps[0], v3k);
    else if(v3v)
        PrintPropValue(file, emitterProps[0], v3v);
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->emissionRange.Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->emissionRange.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[1], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[1], pv);
    
    pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->radius.Get());
    pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->radius.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[2], pk);
    else if(pv)
        PrintPropValue(file, emitterProps[2], pv);
    
    ck = dynamic_cast< PropertyLineKeyframes<Color> *>(emitter->colorOverLife.Get());
    cv = dynamic_cast< PropertyLineValue<Color> *>(emitter->colorOverLife.Get());
    if(pk)
        PrintPropKFValue(file, emitterProps[3], ck);
    else if(pv)
        PrintPropValue(file, emitterProps[3], cv);
    
    fprintf(file, "\n");
    
    for(int i = 0; i < emitter->GetLayers().size(); i++)
    {
        fprintf(file, "layer%d:\n    type: layer\n", i);
        
        int32 propIndex = 0;
        
        Vector<TestScreen::Layer> l = layers;
        
        fprintf(file, "    %s: \"%s\"\n", layerProps[propIndex].c_str(), layers[i+1].spritePath.c_str());
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
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
        int32 forceCount = emitter->GetLayers()[i]->forces.size();
        if(forceCount > 0)
            fprintf(file, "\n    forceCount: %d\n", forceCount);
        
        for(int j = 0; j < forceCount; j++)
        {
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers()[i]->forces[j].Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers()[i]->forces[j].Get());
            if(v3k)
                PrintPropKFValue(file, Format("force%d", j), v3k);
            else if(v3v)
                PrintPropValue(file, Format("force%d", j), v3v);
            
            
            v3k = dynamic_cast< PropertyLineKeyframes<Vector3> *>(emitter->GetLayers()[i]->forcesVariation[j].Get());
            v3v = dynamic_cast< PropertyLineValue<Vector3> *>(emitter->GetLayers()[i]->forcesVariation[j].Get());
            if(v3k)
                PrintPropKFValue(file, Format("forceVariation%d", j), v3k);
            else if(v3v)
                PrintPropValue(file, Format("forceVariation%d", j), v3v);
            
            
            pk = dynamic_cast< PropertyLineKeyframes<float32> *>(emitter->GetLayers()[i]->forcesOverLife[j].Get());
            pv = dynamic_cast< PropertyLineValue<float32> *>(emitter->GetLayers()[i]->forcesOverLife[j].Get());
            if(pk)
                PrintPropKFValue(file, Format("force%d", j), pk);
            else if(pv)
                PrintPropValue(file, Format("forceOverLife%d", j), pv);
            
            fprintf(file, "\n");
        }
        propIndex += 3;
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
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
        fprintf(file, "\n");
        
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

void TestScreen::PrintPropValue(FILE *file, String propName, PropertyLineValue<Vector3> *pv)
{
    Vector3 value = pv->GetValue(0);
    fprintf(file, "    %s: [%f, %f, %f]\n", propName.c_str(), value.x, value.y, value.z);
}
void TestScreen::PrintPropValue(FILE *file, String propName, PropertyLineValue<Color> *pv)
{
    Color value = pv->GetValue(0);
    fprintf(file, "    %s: [%d, %d, %d, %d]\n", propName.c_str(), (int32)(value.r*255), (int32)(value.g*255), (int32)(value.b*255), (int32)(value.a*255));
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
        fprintf(file, "%f, [%f, %f], ", pv->keys[i].t, pv->keys[i].value.x, pv->keys[i].value.y);
    }
    fpos_t pos;
    fgetpos(file, &pos);
    pos -= 2;
    fsetpos(file, &pos);
    fprintf(file, "]\n");
}

void TestScreen::PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<Vector3> *pv)
{
    fprintf(file, "    %s: [", propName.c_str());
    for(int i = 0; i < pv->keys.size(); i++)
    {
        fprintf(file, "%f, [%f, %f, %f], ", pv->keys[i].t, pv->keys[i].value.x, pv->keys[i].value.y, pv->keys[i].value.z);
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
        fprintf(file, "%f, [%d, %d, %d, %d], ", pv->keys[i].t, (int32)(pv->keys[i].value.r*255), (int32)(pv->keys[i].value.g*255), (int32)(pv->keys[i].value.b*255), (int32)(pv->keys[i].value.a*255));
    }
    fpos_t pos;
    fgetpos(file, &pos);
    pos -= 2;
    fsetpos(file, &pos);
    fprintf(file, "]\n");    
}