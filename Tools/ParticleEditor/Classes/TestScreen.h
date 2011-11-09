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
#include "FileSystemDialog.h"
#include "PreviewControl.h"
#include "ForcePreviewControl.h"

using namespace DAVA;

class TestScreen : public UIScreen, public UIListDelegate, public FileSystemDialogDelegate, public PropertyLineEditControlDelegate
{
public:
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
        Layer(String names[], int32 count, String spritePath)
        {
            for(int i = 0; i < count; i++)
            {
                props.push_back(Property(names[i], true, i));
            }
        }
        struct Property
        {
            Property(String sName, bool isDef, int _id)
            {
                name = sName;
                isDefault = isDef;
                id = _id;
            }
            String name;
            int32 id;
            bool isDefault;
        };
        Vector<Property> props;
        String spritePath;
    };
    
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
	virtual int32 CellWidth(UIList *forList, int32 index)//calls only for horizontal orientation
	{return 20;};
	virtual int32 CellHeight(UIList *forList, int32 index);//calls only for vertical orientation
	virtual void OnCellSelected(UIList *forList, UIListCell *selectedCell);
    
    virtual void OnFileSelected(FileSystemDialog *forDialog, const String &pathToFile);
    virtual void OnFileSytemDialogCanceled(FileSystemDialog *forDialog);
    virtual void OnPointAdd(PropertyLineEditControl *forControl, float32 t, float32 value);
    virtual void OnPointDelete(PropertyLineEditControl *forControl, float32 t);
    virtual void OnPointMove(PropertyLineEditControl *forControl, float32, float32, float32);
    virtual void OnMouseMove(float32 t);
    
    void ButtonPressed(BaseObject *obj, void *data, void *callerData);
    void SetLayerProps(Layer *layerProps, ParticleLayer *layer);
    void ShowAddProps();
    void HideAddProps();
    
    void SliderChanged(BaseObject *obj, void *data, void *callerData);
    
    void GetEmitterPropValue(int32 id);
    void SetEmitterPropValue(int32 id, bool def = 0);
    void GetLayerPropValue(int32 id);
    void SetLayerPropValue(int32 id, bool def = 0);
    void ResetEmitterPropValue(int32 id);
    void ResetLayerPropValue(int32 id);
    void GetForcesValue(int32 id);
    
    bool GetProp(PropertyLineValue<float32> *pv);
    bool GetProp(PropertyLineKeyframes<float32> *pk);
    bool GetProp(PropertyLineValue<Vector2> *vv);
    bool GetProp(PropertyLineKeyframes<Vector2> *vk);    
    bool GetProp(PropertyLineValue<Color> *cv);
    bool GetProp(PropertyLineKeyframes<Color> *cv);
    
    void HideAndResetEditFields();
    void HideForcesList();
    void ShowForcesList();
    
    void SaveToYaml(const String &pathToFile);
    
    void PrintPropValue(FILE *file, String propName, PropertyLineValue<float32> *pv);
    void PrintPropValue(FILE *file, String propName, PropertyLineValue<Vector2> *pv);
    void PrintPropValue(FILE *file, String propName, PropertyLineValue<Color> *pv);
    void PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<float32> *pv);
    void PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<Vector2> *pv);
    void PrintPropKFValue(FILE *file, String propName, PropertyLineKeyframes<Color> *pv);
    
    ParticleEmitter *emitter;
    int32 selectedEmitterElement;
    int32 selectedPropElement;
    int32 selectedAddPropElement;
    int32 selectedForceElement;
    
    float32 cellH;
    
    String defSpriteFile;
    
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
    
    UIStaticText *valueText[4];
    UIStaticText *spriteInfo;
    
    UIList *emitterList;
    UIList *propList;
    UIList *addPropList;
    UIList *forcesList;
    
    UISlider *vSliders[4];
    
    PropertyLineEditControl *propEdit[4];
    
    FileSystemDialog *fsDlg;
    FileSystemDialog *fsDlgSprite;
    
    UIControl *spritePanel;
    UIControl *spriteControl;
    
    Sprite *sprite;
    
    PreviewControl *preview;
    
    ForcePreviewControl *forcePreview;
};

