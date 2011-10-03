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

#ifndef __DAVAENGINE_FILE_SYSTEM_DIALOG__
#define __DAVAENGINE_FILE_SYSTEM_DIALOG__

#include "UI/UIControl.h"
#include "UI/UIList.h"
#include "UI/UITextField.h"

namespace DAVA 
{
class FileList;
class UIStaticText;
class UITextField;
class UIFileSystemDialog;
class UIFileSystemDialogDelegate
{
public:
    virtual void OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile) = 0;
    virtual void OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog) = 0;
};

/** 
    \ingroup controlsystem
    \brief This class is dialog that allow to select something on your device filesystem. 
    Main purpose of this class is file & directory selection dialogs in our tools with UI. 
 */
class UIFileSystemDialog : public UIControl, public UIListDelegate, public UITextFieldDelegate
{
public:
    enum eDialogOperation
    {
        OPERATION_LOAD = 0,
        OPERATION_SAVE,
        OPERATION_CHOOSE_DIR
    };
    
    enum eFileUnitType 
    {
        FUNIT_FILE = 0,
        FUNIT_DIR_INSIDE,
        FUNIT_DIR_OUTSIDE
    };
    
    class DialogFileUnit 
    {
    public:
        String name;
        int32 indexInFileList;
        int32 type;
    };
    
    UIFileSystemDialog(const String &_fontPath);
//	virtual void WillAppear();
    
    void Show(UIControl *parentControl);
    
    
    void SetOperationType(UIFileSystemDialog::eDialogOperation newOperationType)
    {
        DVASSERT(!GetParent());
        operationType = newOperationType;
    }
    
    int32 GetOperationType()
    {
        return operationType;
    }
    
    void SetDelegate(UIFileSystemDialogDelegate *newDelegate)
    {
        delegate = newDelegate;
    }
    
    void SetTitle(const WideString &newTitle)
    {
        title->SetText(newTitle);
    }
    
    
    void SetCurrentDir(const String &newDirPath);
    const String &GetCurrentDir();
    
    /**
        \brief Set extension filter from string variable. Each extension should be separated by semicolon(;).
        All extensions automatically transformed to lower-case, so you do not need to set all cases of extensions.
        \param[in] extensionFilter example ".png;.jpeg;.jpg" 
     */
    void SetExtensionFilter(const String & extensionFilter);
    /** 
        \brief Set extension filter from string vector. 
        All extensions automatically transformed to lower-case, so you do not need to set all cases of extensions.
        \param[in] newExtensionFilter Vector of String variables with extensions you want to set
     */
    void SetExtensionFilter(const Vector<String> &newExtensionFilter);
    const Vector<String> &GetExtensionFilter();
    
    
    virtual int32 ElementsCount(UIList *forList);
	virtual UIListCell *CellAtIndex(UIList *forList, int32 index);
	virtual int32 CellWidth(UIList *forList, int32 index)//calls only for horizontal orientation
	{return 20;};
	virtual int32 CellHeight(UIList *forList, int32 index);//calls only for vertical orientation
	virtual void OnCellSelected(UIList *forList, UIListCell *selectedCell);
    
    void ButtonPressed(BaseObject *obj, void *data, void *callerData);

    virtual void TextFieldShouldReturn(UITextField * textField);
	virtual bool TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString);
    
    
    
protected:
    void RefreshList();
    
    void OnIndexSelected(int32 index);
    void SaveFinishing();
    
    String fontPath;
    int32 operationType;
    UIFileSystemDialogDelegate *delegate;
    Vector<String> extensionFilter;
    String currentDir;
    int32 cellH;
    
    UIList *fileListView;
    FileList *files;
    Vector<DialogFileUnit> fileUnits;
    
    UIListCell *lastSelected;
    int32 lastSelectedIndex;
    uint64 lastSelectionTime;
    
    UIStaticText *title;
    UIStaticText *workingPath;
    
    UITextField *textField;
    
    UIButton *positiveButton;
    UIButton *negativeButton;
    
};
};

#endif // __DAVAENGINE_FILE_SYSTEM_DIALOG__
