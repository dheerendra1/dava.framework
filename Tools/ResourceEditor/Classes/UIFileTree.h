/*
 *  TestScreen.h
 *  TemplateProjectMacOS
 *
 *  Created by Vitaliy  Borodovsky on 3/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __DAVAENGINE_UIFILETREE_H__
#define __DAVAENGINE_UIFILETREE_H__


#include "Base/BaseTypes.h"
#include "UI/UIList.h"
#include "UIFileTreeCell.h"

namespace DAVA 
{
	
class UIFileTree;
class UIFileTreeCell;
class UIFileTreeDelegate
{
public:
	//virtual UIFileTreeCell *CellAtIndex(UIFileTree * tree, int32 index) = 0;
	//virtual int32 CellHeight(UIFileTree * tree, int32 index) = 0;
	
	virtual void OnCellSelected(UIFileTree * tree, UIFileTreeCell *selectedCell) = 0;
};
	
class UIFileTree;
class UITreeItemInfo : public BaseObject
{
public:
	UITreeItemInfo(UIFileTree * _ownerTree)
	{
		ownerTree = _ownerTree;
		level = 0;
		isExpanded = false;
		isDirectory = false;
	}
	~UITreeItemInfo() 
	{
		RemoveChildren();
	};
	
	void Set(int32 _level, const String & _name, const String & _pathname, bool _isDirectory)
	{
		level = _level;
		name = _name;
		pathname = _pathname;
		isDirectory = _isDirectory;
		isExpanded = isDirectory ? (false) : (true);
	}
	
	void RemoveChildren();
	int32 GetLevel() { return level; };
	const String & GetName() {return name; };
	const String & GetPathname() { return pathname; };
	bool IsDirectory() { return isDirectory; };
	void ToggleExpanded();// { isExpanded = !isExpanded; };
	Vector<UITreeItemInfo*> & GetChildren() { return children; };
	void AddChild(UITreeItemInfo * t) { children.push_back(t); };
	
	
	int32 GetTotalCount();
	UITreeItemInfo * EntryByIndex(int32 index);
private:
	UIFileTree * ownerTree;
	int32  level;
	String name;
	String pathname;
	Vector<UITreeItemInfo*> children;
	bool isExpanded;
	bool isDirectory;
};
	
// comment: use namespace and standard prefix because probably this class can be moved to framework later
class UIFileTree: public UIList, public UIListDelegate
{
public:
	UIFileTree(const Rect &rect = Rect(), bool rectInAbsoluteCoordinates = FALSE);
	~UIFileTree();
	
	/**
		\brief Main function of the class. This function refresh the list with new path you've passed. 
		You can pass comma separated extension list to this function. Example: ".png; .txt; .jpg".
		\param[in] path path to directory you want to set as head directory of the file tree
		\param[in] extensions comma separated list of extensions you want to show, "" means that you accept all extensions
	 */
	void SetPath(const String & path, const String & extensions = "");
	/**
		\brief Set delegate to handle UIFileTree selections
	 */
	void SetDelegate(UIFileTreeDelegate * delegate);
	
	/**
		\brief Function to enable folder navigation
		\param[in] isEnabled true if you want to enable it, false if you want to disable it. 
	 */
	void SetFolderNavigation(bool isEnabled);
	

private:
	// Delegate functions
	virtual int32 ElementsCount(UIList *forList);
	virtual UIListCell *CellAtIndex(UIList *forList, int32 index);
	virtual int32 CellWidth(UIList *forList, int32 index);
	virtual int32 CellHeight(UIList *forList, int32 index);
	virtual void OnCellSelected(UIList *forList, UIListCell *selectedCell);
	
	void OnDirectoryChange(BaseObject * obj, void * userData, void * callerData);
private:
	
	void RecursiveTreeWalk(const String & path, UITreeItemInfo * current);
	
	UIFileTreeDelegate * delegate;
	UITreeItemInfo * treeHead;
	String path;	
	String originalExtensionsString;
	Vector<String> extensions;
	bool isFolderNavigationEnabled;
	
	friend class UITreeItemInfo;
};
	
};

#endif // #ifdef __DAVAENGINE_UIFILETREE_H__
