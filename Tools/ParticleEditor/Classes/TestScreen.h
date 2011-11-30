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
#include "DAVAEngine.h"
#include "PropertyLineEditControl.h"
#include "PreviewControl.h"
#include "ForcePreviewControl.h"

using namespace DAVA;

class TestScreen : public UIScreen, public UIListDelegate, public UIFileSystemDialogDelegate, public PropertyLineEditControlDelegate, public UITextFieldDelegate
{
public:
    enum eProps
    {
        EMITTER_TYPE = 0,
        EMITTER_EMISSION_ANGLE,
        EMITTER_EMISSION_RANGE,
        EMITTER_EMISSION_VECTOR,
        EMITTER_RADIUS,
        EMITTER_COLOR_OVER_LIFE,
        EMITTER_SIZE, 
        EMITTER_LIFE,
    };
    enum lProps
    {
        LAYER_SPRITE = 0, 
        LAYER_LIFE, 
        LAYER_LIFE_VARIATION, 
        LAYER_NUMBER, 
        LAYER_NUMBER_VARIATION, 
        LAYER_SIZE,
        LAYER_SIZE_VARIATION, 
        LAYER_SIZE_OVER_LIFE, 
        LAYER_VELOCITY,
        LAYER_VELOCITY_VARIATION,
        LAYER_VELOCITY_OVER_LIFE, 
        LAYER_FORCES,
        LAYER_FORCES_VARIATION, 
        LAYER_FORCES_OVER_LIFE, 
        LAYER_SPIN, 
        LAYER_SPIN_VARIATION, 
        LAYER_SPIN_OVER_LIFE, 
        LAYER_MOTION_RANDOM,
        LAYER_MOTION_RANDOM_VARIATION, 
        LAYER_MOTION_RANDOM_OVER_LIFE, 
        LAYER_BOUNCE, 
        LAYER_BOUNCE_VARIATION, 
        LAYER_BOUNCE_OVER_LIFE,
        LAYER_COLOR_RANDOM,
        LAYER_ALPHA_OVER_LIFE,
        LAYER_COLOR_OVER_LIFE,
        LAYER_ALIGN_TO_MOTION,
        LAYER_START_TIME,
        LAYER_END_TIME,
    };
    class PropListCell : public UIListCell
    {
    public:
        PropListCell(const Rect &rect, const String &cellIdentifier)
        : UIListCell(rect, cellIdentifier)
        {
            SetStateDrawType(UIControl::STATE_SELECTED, UIControlBackground::DRAW_FILL);
            GetStateBackground(UIControl::STATE_SELECTED)->SetColor(Color(0.65, 0.65, 0.65, 0.65));
            name = new UIStaticText(Rect(0, 0, size.x, size.y));
            AddControl(name);
        }
        ~PropListCell()
        {
            SafeRelease(name);
        }
        void SetText(WideString nameStr)
        {
            name->SetText(nameStr);
        }
        void SetId(int32 _id)
        {
            id = _id;
        }
        int32 GetId()
        {
            return id;
        }
        void SetFront(Font *f)
        {
            name->SetFont(f);
        }
    protected:
        int32 id;
        UIStaticText *name;
    };
    
    struct Layer
    {
        struct Property
        {
            Property(const String *sName, bool isDef, int _id)
            {
                name = sName;
                isDefault = isDef;
                id = _id;
                minValue = 0;
                maxValue = 1;
                minT = 0;
                maxT = 1;

            }
            const String *name;
            int32 id;
            bool isDefault;
            int32 minValue;
            int32 maxValue;
            int32 minT;
            int32 maxT;
        };
        
        Vector<Property *> props;
        String spritePath;
        UIStaticText *curLayerTime;
        bool isDisabled;
        
        Layer()
        {
            curLayerTime = new UIStaticText();
        }
        ~Layer()
        {
            SafeRelease(curLayerTime);
            props.clear();
        }
        Layer(const Vector<String> &names, const String &_spritePath, Font *f)
        {
            for(int i = 0; i < names.size(); i++)
            {
                props.push_back(new Property(&names[i], true, i));
            }
            spritePath = _spritePath;
            
            curLayerTime = new UIStaticText();
            curLayerTime->SetAlign(DAVA::ALIGN_LEFT);
            curLayerTime->SetFont(f);
            
            isDisabled = false;
        }
        Layer * Clone()
        {
            Layer * l = new Layer();
            l->props = props;
            l->spritePath = spritePath;
            l->curLayerTime = curLayerTime->CloneStaticText();
            l->isDisabled = isDisabled;
            return l;
        }
    };
    
    TestScreen();
    
	virtual void LoadResources();
	virtual void UnloadResources();
	virtual void WillAppear();
	virtual void WillDisappear();

	virtual void Update(float32 timeElapsed);
	virtual void Draw(const UIGeometricData &geometricData);
	
	virtual void Input(UIEvent * touch);
    
protected:
    virtual int32 ElementsCount(UIList *forList);
	virtual UIListCell *CellAtIndex(UIList *forList, int32 index);
	virtual int32 CellWidth(UIList *forList, int32 index)
	{return 20;};
	virtual int32 CellHeight(UIList *forList, int32 index);
	virtual void OnCellSelected(UIList *forList, UIListCell *selectedCell);
    
    virtual void OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile);
    virtual void OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog);
    virtual void OnPointAdd(PropertyLineEditControl *forControl, float32 t, float32 value);
    virtual void OnPointDelete(PropertyLineEditControl *forControl, float32 t);
    virtual void OnPointMove(PropertyLineEditControl *forControl, float32 lastT, float32 newT, float32 newV);
    virtual void OnMouseMove(PropertyLineEditControl *forControl, float32 t);
    virtual void OnPointSelected(PropertyLineEditControl *forControl, int32 index, Vector2 value);
	virtual void TextFieldShouldReturn(UITextField * textField);
	virtual bool TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString);
    
    void ButtonPressed(BaseObject *obj, void *data, void *callerData);
    void ShowAddProps();
    void HideAddProps();
    
    void SliderChanged(BaseObject *obj, void *data, void *callerData);
    
    void GetEmitterPropValue(eProps id, bool getLimits = false);
    void SetEmitterPropValue(eProps id, bool def = 0);
    void GetLayerPropValue(lProps id, bool getLimits = false);
    void SetLayerPropValue(lProps id, bool def = 0);
    void ResetEmitterPropValue(eProps id);
    void ResetLayerPropValue(lProps id);
    void GetForcesValue(int32 id, bool getLimits = false);
    
    bool GetProp(PropertyLineValue<float32> *pv, int32 id, bool getLimits = false);
    bool GetProp(PropertyLineKeyframes<float32> *pk, int32 id, bool getLimits = false);
    bool GetProp(PropertyLineValue<Vector2> *vv, int32 id, bool getLimits = false);
    bool GetProp(PropertyLineKeyframes<Vector2> *vk, int32 id, bool getLimits = false); 
    bool GetProp(PropertyLineValue<Vector3> *vv, int32 id, bool getLimits = false);
    bool GetProp(PropertyLineKeyframes<Vector3> *vk, int32 id, bool getLimits = false);     
    bool GetProp(PropertyLineValue<Color> *cv, int32 id, bool getLimits = false);
    bool GetProp(PropertyLineKeyframes<Color> *cv, int32 id, bool getLimits = false);
    
    void ShowValueEditFields(int32 dim);
    void ShowKeyedEditFields(int32 dim);
    void HideAndResetEditFields();
    void HideForcesList();
    void ShowForcesList();
    
    void SaveToYaml(const String &pathToFile);
    
    void PrintPropValue(File *file, const String &propName, PropertyLineValue<float32> *pv);
    void PrintPropValue(File *file, const String &propName, PropertyLineValue<Vector2> *pv);
    void PrintPropValue(File *file, const String &propName, PropertyLineValue<Vector3> *pv);
    void PrintPropValue(File *file, const String &propName, PropertyLineValue<Color> *pv);
    void PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<float32> *pv);
    void PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Vector2> *pv);
    void PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Vector3> *pv);
    void PrintPropKFValue(File *file, const String &propName, PropertyLineKeyframes<Color> *pv);
    
    void AddSelectedProp();
    
    void ExecutePacker(const String &path);
    
    void SafeAddControl(UIControl *control);
    void SafeRemoveControl(UIControl *control);
    
    Vector<String> emitterProps;
    Vector<String>layerProps;
    Vector<String> emitterTypes;

    int32 deltaIndex;
    bool curPropType; //0 - value, 1 - Keyframed
    Font *cellFont, *f;
    int32 dblClickDelay;
    int32 activePropEdit;
    float32 curPropEditTime;
    float32 buttonW;
    Rect tfPosSlider[2], tfPosKFEdit[2];
    Rect colorViewPosSlider, colorViewPosKFEdit;
    Rect keysValueTextPos[4], tfKeysValuePos[4][2], tfKeysValueTextPos[4][2];
    PropertyLineKeyframes<Color> *curColorProp;
    PropertyLineKeyframes<float32> *cur1DimProp;
    PropertyLineKeyframes<Vector2> *cur2DimProp;
    PropertyLineKeyframes<Vector3> *cur3DimProp;
    int32 activeKFEdit;
    
    Vector<Layer *> layers;
    
    ParticleEmitter *emitter;
    int32 selectedEmitterElement;
    int32 selectedPropElement;
    int32 selectedAddPropElement;
    int32 selectedForceElement;
    int32 selectedEmitterTypeElement;
    
    float32 cellH;
    
    UIButton *loadEmitter;
    UIButton *saveEmitter;
    UIButton *newEmitter;
    UIButton *addLayer;
    UIButton *delLayer;
    UIButton *addProp;
    UIButton *delProp;
    UIButton *OKBut;
    UIButton *cancelBut;
    UIButton *KFBut;
    UIButton *valueBut;
    UIButton *spriteSelect;
    UIButton *addForce;
    UIButton *delForce;
    UIButton *chooseProject;
    UIButton *emitter3D;
    
    UIButton *cloneLayer;
    UIButton *disableLayer;
    
    UIStaticText *tfTimeLimitsText[2];
    UIStaticText *tfKeysValueText[2];
    UIStaticText *keysValueText;
    UIStaticText *valueText[4];
    UIStaticText *spriteInfo;
    UIStaticText *particleCountText;
    UIStaticText *tip;
    
    UIList *emitterList;
    UIList *propList;
    UIList *addPropList;
    UIList *forcesList;
    UIList *emitterTypeList;
    
    UITextField *tfKeysValue[2];
    UITextField *tfValue[4];
    UITextField *tfTimeLimits[2];
    UITextField *tfValueLimits[2];
    UISlider *vSliders[4];
    
    PropertyLineEditControl *propEdit[4];
    
    UIFileSystemDialog *fsDlg;
    UIFileSystemDialog *fsDlgSprite;
    UIFileSystemDialog *fsDlgProject;
    
    UIControl *spritePanel;
    UIControl *spriteControl;
    
    Sprite *sprite;
    
    UIControl *colorView;
    
    PreviewControl *preview;
    
    ForcePreviewControl *forcePreview;
};

