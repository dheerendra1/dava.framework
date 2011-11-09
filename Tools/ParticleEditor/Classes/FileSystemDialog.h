/*
 *  FileSystemDialog.h
 *  SniperEditorMacOS
 *
 *  Created by Alexey Prosin on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FILE_SYSTEM_DIALOG
#define FILE_SYSTEM_DIALOG

#include "DAVAEngine.h"

using namespace DAVA;

class FileSystemDialog;
class FileSystemDialogDelegate
{
public:
    virtual void OnFileSelected(FileSystemDialog *forDialog, const String &pathToFile) = 0;
    virtual void OnFileSytemDialogCanceled(FileSystemDialog *forDialog) = 0;
};


class FileSystemDialog : public UIControl, public UIListDelegate, public UITextFieldDelegate
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
    
    FileSystemDialog(const String &_fontPath);
//	virtual void WillAppear();
    
    void Show(UIControl *parentControl);
    
    
    void SetOperationType(FileSystemDialog::eDialogOperation newOperationType)
    {
        DVASSERT(!GetParent());
        operationType = newOperationType;
    }
    
    int32 GetOperationType()
    {
        return operationType;
    }
    
    void SetDelegate(FileSystemDialogDelegate *newDelegate)
    {
        delegate = newDelegate;
    }
    
    void SetTitle(const WideString &newTitle)
    {
        title->SetText(newTitle);
    }
    
    
    void SetCurrentDir(const String &newDirPath);
    const String &GetCurrentDir();
    
    void SetExtensionFilter(const Vector<String> &newExtensionFilter);//You need to set all cases of extension (for axample [".png", ".PNG"])
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
    FileSystemDialogDelegate *delegate;
    Vector<String> extensionFilter;
    String currentDir;
    int32 cellH;
    
    UIList *fileListView;
    UIScrollBar *fileListScroll;
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


#endif
