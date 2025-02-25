//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _GUIPOPUPCTRL_H_
#define _GUIPOPUPCTRL_H_

#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif
#ifndef _GUITEXTLISTCTRL_H_
#include "gui/controls/guiTextListCtrl.h"
#endif
#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _GUIBACKGROUNDCTRL_H_
#include "gui/controls/guiBackgroundCtrl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

#include "T3D/assets/ImageAsset.h"
class GuiPopUpMenuCtrl;
class GuiPopupTextListCtrl;

class GuiPopUpBackgroundCtrl : public GuiControl
{
protected:
   GuiPopUpMenuCtrl *mPopUpCtrl;
   GuiPopupTextListCtrl *mTextList; 
public:
   GuiPopUpBackgroundCtrl(GuiPopUpMenuCtrl *ctrl, GuiPopupTextListCtrl* textList);
   void onMouseDown(const GuiEvent &event) override;
};

class GuiPopupTextListCtrl : public GuiTextListCtrl
{
   private:
   typedef GuiTextListCtrl Parent;

protected:
   GuiPopUpMenuCtrl *mPopUpCtrl;

public:
   GuiPopupTextListCtrl(); // for inheritance
   GuiPopupTextListCtrl(GuiPopUpMenuCtrl *ctrl);

   // GuiArrayCtrl overload:
   void onCellSelected(Point2I cell) override;

   // GuiControl overloads:
   bool onKeyDown(const GuiEvent &event) override;
   void onMouseUp(const GuiEvent &event) override;
   void onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver) override;
};

class GuiPopUpMenuCtrl : public GuiTextCtrl
{
   typedef GuiTextCtrl Parent;

public:
   struct Entry
   {
      char buf[256];
      S32 id;
      U16 ascii;
      U16 scheme;
      bool usesColorBox;	//  Added
      ColorI colorbox;		//  Added
   };

   struct Scheme
   {
      U32      id;
      ColorI   fontColor;
      ColorI   fontColorHL;
      ColorI   fontColorSEL;
   };

   bool mBackgroundCancel; //  Added

protected:
   GuiPopupTextListCtrl *mTl;
   GuiScrollCtrl *mSc;
   GuiPopUpBackgroundCtrl *mBackground;
   Vector<Entry> mEntries;
   Vector<Scheme> mSchemes;
   S32 mSelIndex;
   S32 mMaxPopupHeight;
   F32 mIncValue;
   F32 mScrollCount;
   S32 mLastYvalue;
   GuiEvent mEventSave;
   S32 mRevNum;
   bool mInAction;
   bool mReplaceText;
   bool mMouseOver; //  Added
   bool mRenderScrollInNA; //  Added
   bool mReverseTextList;	//  Added - Should we reverse the text list if we display up?

   enum BitmapModes
   {
      Normal,
      Depressed,

      NumBitmapModes = 2
   };

   DECLARE_IMAGEASSET_ARRAY(GuiPopUpMenuCtrl, Bitmap, NumBitmapModes);
   DECLARE_IMAGEASSET_ARRAY_SETGET(GuiPopUpMenuCtrl, Bitmap);

   Point2I mBitmapBounds; //  Added
	S32 mIdMax;

   virtual void addChildren();
   virtual void repositionPopup();

   static bool _setBitmaps(void* obj, const char* index, const char* data);

public:
   GuiPopUpMenuCtrl(void);
   ~GuiPopUpMenuCtrl();   
   GuiScrollCtrl::Region mScrollDir;
   bool onWake() override; //  Added
   bool onAdd() override;
   void onSleep() override;
   void setBitmap(const char *name); //  Added
   void sort();
   void sortID(); //  Added
   void addEntry(const char *buf, S32 id = -1, U32 scheme = 0);
   void addScheme(U32 id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL);
   void onRender(Point2I offset, const RectI &updateRect) override;
   void onAction() override;
   virtual void closePopUp();
   void clear() override;
	void clearEntry( S32 entry ); //  Added
   void onMouseDown(const GuiEvent &event) override;
   void onMouseUp(const GuiEvent &event) override;
   void onMouseEnter(const GuiEvent &event) override; //  Added
   void onMouseLeave(const GuiEvent &) override; //  Added
   void setupAutoScroll(const GuiEvent &event);
   void autoScroll();
   bool onKeyDown(const GuiEvent &event) override;
   void reverseTextList();
   bool getFontColor(ColorI &fontColor, S32 id, bool selected, bool mouseOver);
   bool getColoredBox(ColorI &boxColor, S32 id); //  Added
   bool setEntryText( S32 id, const char* buf );

   S32 getSelected();
   void setSelected(S32 id, bool bNotifyScript = true);
   void setFirstSelected(bool bNotifyScript = true); //  Added
   void setNoneSelected();	//  Added
   const char *getScriptValue() override;
   const char *getTextById(S32 id);
   S32 findText( const char* text );
   S32 getNumEntries()   { return( mEntries.size() ); }
   void replaceText(S32);

   DECLARE_CONOBJECT( GuiPopUpMenuCtrl );
   DECLARE_CATEGORY( "Gui Lists" );
   DECLARE_DESCRIPTION( "A control that allows to select a value from a drop-down list." );
   
   static void initPersistFields(void);

};

#endif //_GUI_POPUPMENU_CTRL_H
