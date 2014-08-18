#ifndef __BASE_DIALOG_H__
#define __BASE_DIALOG_H__


#include "Core.h"

#if HAS_UI	// defined in Build.h, included from Core.h

#include "UnCore.h"					// for TArray and FString

#include "callback.hpp"

// forwards
class UIBaseDialog;


enum ETextAlign
{
	TA_Left,
	TA_Right,
	TA_Center,
};


/*-----------------------------------------------------------------------------
	UIElement
-----------------------------------------------------------------------------*/

class UIElement
{
	friend class UIGroup;
public:
	UIElement();
	virtual ~UIElement();

	void Enable(bool enable);
	FORCEINLINE bool IsEnabled() const       { return Enabled; }

	// Layout functions

	UIElement& SetRect(int x, int y, int width, int height);
	FORCEINLINE UIElement& SetWidth(int width)     { Width = width; return *this; }
	FORCEINLINE UIElement& SetHeight(int height)   { Height = height; return *this; }
	FORCEINLINE int  GetWidth() const        { return Width; }
	FORCEINLINE int  GetHeight() const       { return Height; }
	UIElement& SetParent(UIGroup* group);

	FORCEINLINE HWND GetWnd() const          { return Wnd; }

	static FORCEINLINE int EncodeWidth(float w)
	{
		w = bound(w, 0, 1);
		int iw = w * 255.0f;			// float -> int
		return 0xFFFF0000 | iw;
	}

	static FORCEINLINE float DecodeWidth(int w)
	{
#if MAX_DEBUG
		assert((w & 0xFFFFFF00) == 0xFFFF0000 || w == -1);
#endif
		return (w & 0xFF) / 255.0f;		// w=-1 -> 1.0f
	}

	void MeasureTextSize(const char* text, int* width, int* height = NULL, HWND wnd = 0);

	friend UIElement& operator+(UIElement& elem, UIElement& next);

protected:
	int			X;
	int			Y;
	int			Width;
	int			Height;
	bool		IsGroup;
	bool		Enabled;
	UIGroup*	Parent;
	UIElement*	NextChild;

	HWND		Wnd;
	int			Id;

	HWND Window(const char* className, const char* text, DWORD style, DWORD exstyle, UIBaseDialog* dialog,
		int id = -1, int x = -1, int y = -1, int w = -1, int h = -1);

	virtual void Create(UIBaseDialog* dialog) = 0;
	// Process WM_COMMAND message. 'id' is useless in most cases, useful for
	// groups only.
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam)
	{
		return false;
	}
	virtual void DialogClosed(bool cancel)
	{}
	virtual void UpdateEnabled();
};

// Declare some functions which could be useful for UE4-like declarative syntax:
//	Expose(var)			save pointer to control in variable
//	SetParent(parent)	attach control to parent

// Some functions exists in UIElement but overrided here to be able to chain them
// without falling back to UIElement class: UIElement's functions can't return
// 'this' of derived type, so we're redeclaring functions here.

// To add controls to a UIGroup object use the following syntax:
//		NewControl(UIGroup)
//		.Expose(GroupVar)
//		.SomeGroupFunc()
//		[
//			NewControl(ControlType1, args1)
//			.SomeControl1Func1()
//			.SomeControl1Func2()
//			+ NewControl(ControlType2, args2)
//			.Expose(Control2Var)
//			...
//		]
// This code is identical to:
//		GroupVar = new UIGroup();				// make a group
//		GroupVar->SomeGroupFunc();
//		tmpControl1 = new ControlType1(args1);	// make a control 1
//		tmpControl1->SomeControl1Func1();
//		tmpControl1->SomeControl1Func2();
//		ControlVar2 = new ControlType2(args2);	// make a control 2
//		GroupVar->Add(tmpControl1);				// add controls to group
//		GroupVar->Add(ControlVar2);

#define DECLARE_UI_CLASS(ClassName, ParentClass)	\
	typedef ClassName ThisClass;					\
	typedef ParentClass Super;						\
public:												\
	FORCEINLINE ThisClass& SetRect(int x, int y, int width, int height) \
	{ return (ThisClass&) Super::SetRect(x, y, width, height); } \
	FORCEINLINE ThisClass& SetWidth(int width)       { return (ThisClass&) Super::SetWidth(width); } \
	FORCEINLINE ThisClass& SetHeight(int height)     { return (ThisClass&) Super::SetHeight(height); } \
	FORCEINLINE ThisClass& Expose(ThisClass*& var)   { var = this; return *this; } \
	FORCEINLINE ThisClass& SetParent(UIGroup* group) { return (ThisClass&) Super::SetParent(group); } \
	FORCEINLINE ThisClass& SetParent(UIGroup& group) { return (ThisClass&) Super::SetParent(&group); } \
private:

// Use this macro to declare callback type, variable and SetCallback function. It will
// automatically add 'ThisClass' pointer as a first parameter for callback function.
#define DECLARE_CALLBACK(VarName, ...)				\
public:												\
	typedef util::Callback<void (ThisClass*, __VA_ARGS__)> VarName##_t; \
	FORCEINLINE ThisClass& SetCallback(VarName##_t& cb) \
	{												\
		VarName = cb; return *this;					\
	}												\
protected:											\
	VarName##_t		VarName;						\
private:


// Control creation helpers.
// Use these to receive 'Type&' value instead of 'Type*' available with 'new Type' call

#define NewControl(type, ...)	_NewControl<type>(__VA_ARGS__)

// C++11 offers Variadic Templates (http://en.wikipedia.org/wiki/Variadic_template), unfortunately
// Visual Studio prior to 2013 doesn't support them. GCC 4.3 has variadic template support.
template<class T>
FORCEINLINE T& _NewControl()
{
	return *new T();
}

template<class T, class P1>
FORCEINLINE T& _NewControl(P1 p1)
{
	return *new T(p1);
}

template<class T, class P1, class P2>
FORCEINLINE T& _NewControl(P1 p1, P2 p2)
{
	return *new T(p1, p2);
}

template<class T, class P1, class P2, class P3>
FORCEINLINE T& _NewControl(P1 p1, P2 p2, P3 p3)
{
	return *new T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3, class P4>
FORCEINLINE T& _NewControl(P1 p1, P2 p2, P3 p3, P4 p4)
{
	return *new T(p1, p2, p3, p4);
}


/*-----------------------------------------------------------------------------
	Controls
-----------------------------------------------------------------------------*/

class UILabel : public UIElement
{
	DECLARE_UI_CLASS(UILabel, UIElement);
public:
	UILabel(const char* text, ETextAlign align = TA_Left);

protected:
	FString		Label;
	ETextAlign	Align;

	virtual void Create(UIBaseDialog* dialog);
};


class UIButton : public UIElement
{
	DECLARE_UI_CLASS(UIButton, UIElement);
	DECLARE_CALLBACK(Callback);
public:
	UIButton(const char* text);

protected:
	FString		Label;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
};


class UICheckbox : public UIElement
{
	DECLARE_UI_CLASS(UICheckbox, UIElement);
	DECLARE_CALLBACK(Callback, bool);
public:
	UICheckbox(const char* text, bool value);
	UICheckbox(const char* text, bool* value);

protected:
	FString		Label;
	bool		bValue;			// local bool value
	bool*		pValue;			// pointer to editable value

	HWND		DlgWnd;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
};


class UITextEdit : public UIElement
{
	DECLARE_UI_CLASS(UITextEdit, UIElement);
	DECLARE_CALLBACK(Callback, const char*);
public:
	UITextEdit(const char* text);
	UITextEdit(FString* text);

	void SetText(const char* text = NULL);
	const char* GetText();

protected:
	FString		sValue;
	FString*	pValue;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
	// request edited text from UI
	void UpdateText();
	// request edited text only when dialog is closed
	virtual void DialogClosed(bool cancel);
};


class UICombobox : public UIElement
{
	DECLARE_UI_CLASS(UICombobox, UIElement);
	DECLARE_CALLBACK(Callback, int, const char*);
public:
	UICombobox();

	void AddItem(const char* item);
	void AddItems(const char** items);
	void RemoveAllItems();

	void SelectItem(int index);
	void SelectItem(const char* item);

	FORCEINLINE const char* GetItem(int index) const
	{
		return Items[index];
	}
	FORCEINLINE int GetSelectionIndex() const
	{
		return Value;
	}
	FORCEINLINE const char* GetSelectionText() const
	{
		return (Value >= 0) ? *Items[Value] : NULL;
	}

protected:
	TArray<FString> Items;
	int			Value;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
};


/*-----------------------------------------------------------------------------
	UI containers
-----------------------------------------------------------------------------*/

// Constants for setting some group properties
#define GROUP_NO_BORDER			1
#define GROUP_NO_AUTO_LAYOUT	2

#define GROUP_CUSTOM_LAYOUT		(GROUP_NO_BORDER|GROUP_NO_AUTO_LAYOUT)

class UIGroup : public UIElement
{
	DECLARE_UI_CLASS(UIGroup, UIElement);
public:
	UIGroup(const char* label, unsigned flags = 0);
	UIGroup(unsigned flags = 0);
	virtual ~UIGroup();

	void Add(UIElement* item);
	FORCEINLINE void Add(UIElement& item)
	{
		Add(&item);
	}
	void Remove(UIElement* item);
	void ReleaseControls();

	void AllocateUISpace(int& x, int& y, int& w, int& h);
	void AddVerticalSpace(int height = -1);

	void EnableAllControls(bool enabled);

	// Function for adding children in declarative syntax
	FORCEINLINE UIGroup& operator[](UIElement& item)
	{
		Add(item); return *this;
	}

protected:
	FString		Label;
	UIElement*	FirstChild;
	int			TopBorder;
	unsigned	Flags;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
	virtual void DialogClosed(bool cancel);
	virtual void UpdateEnabled();
	void CreateGroupControls(UIBaseDialog* dialog);

	virtual void AddCustomControls()
	{}
};


// Mix of UICheckbox and UIGroup: this control has a checkbox instead
// of simple title. When it is not checked, all controls are disabled.
class UICheckboxGroup : public UIGroup
{
	DECLARE_UI_CLASS(UICheckboxGroup, UIGroup);
	DECLARE_CALLBACK(Callback, bool);
public:
	UICheckboxGroup(const char* label, bool value, unsigned flags = 0);

protected:
	FString		Label;			// overrides Label of parent
	bool		Value;
	HWND		CheckboxWnd;	// checkbox window
	HWND		DlgWnd;

	virtual void Create(UIBaseDialog* dialog);
	virtual bool HandleCommand(int id, int cmd, LPARAM lParam);
};


class UIBaseDialog : public UIGroup
{
	DECLARE_UI_CLASS(UIBaseDialog, UIElement);
public:
	UIBaseDialog();
	virtual ~UIBaseDialog();

	virtual void InitUI()
	{}

	FORCEINLINE int GenerateDialogId()
	{
		return NextDialogId++;
	}

	bool ShowDialog(const char* title, int width, int height);
	void CloseDialog(bool cancel = false);

protected:
	int			NextDialogId;

	// dialog procedure
	static INT_PTR CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif // HAS_UI

#endif // __BASE_DIALOG_H__
