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

#ifndef _WINDOW_INPUTGENERATOR_H_
#define _WINDOW_INPUTGENERATOR_H_

#ifndef _PLATFORMINPUT_H_
   #include "platform/platformInput.h"
#endif
#ifndef _MPOINT2_H_
   #include "math/mPoint2.h"
#endif


class IProcessInput;
class PlatformWindow;


class WindowInputGenerator
{
   protected:

      PlatformWindow *mWindow;
      IProcessInput  *mInputController;
      Point2I         mLastCursorPos;
      bool            mClampToWindow;
      bool            mFocused; ///< We store this off to avoid polling the OS constantly

      ///  This is the scale factor which relates  mouse movement in pixels
      /// (one unit of mouse movement is a mickey) to units in the GUI.
      F32             mPixelsPerMickey;

      /// This tells us if the last key we pressed was used from the global action map.
      bool mLastPressWasGlobalActionMap;

      // Event Handlers
      void handleMouseButton(WindowId did, U32 modifier,  U32 action, U16 button);
      void handleMouseWheel (WindowId did, U32 modifier,  S32 wheelDeltaX, S32 wheelDeltaY);
      void handleMouseMove  (WindowId did, U32 modifier,  S32 x,      S32 y, bool isRelative);
      void handleKeyboard   (WindowId did, U32 modifier,  U32 action, U16 key);
      void handleCharInput  (WindowId did, U32 modifier,  U16 key);
      void handleAppEvent   (WindowId did, S32 event);
      void handleInputEvent (U32 deviceInst, F32 fValue, F32 fValue2, F32 fValue3, F32 fValue4, S32 iValue, U16 deviceType, U16 objType, U16 ascii, U16 objInst, U8 action, U8 modifier);

      void generateInputEvent( InputEventInfo &inputEvent );

      /// Accelerator key map
       struct AccKeyMap
       {
          void *hnd = NULL;
          String cmd;
          U32 keyCode = 0x000;
          U32 modifier = 0x000;
       };
       Vector <AccKeyMap> mAcceleratorMap;
      
   public:
   
      WindowInputGenerator( PlatformWindow *window );
      virtual ~WindowInputGenerator();

      void setInputController( IProcessInput *inputController ) { mInputController = inputController; };
      
      /// Returns true if the given keypress event should be send as a raw keyboard
      /// event even if it maps to a character input event.
      bool wantAsKeyboardEvent( U32 modifiers, U32 key );

      /// Tells us if the last key was used within the global action map.
      /// @return true if the key was a global action map key, false otherwise.
      /// @note Useful and currently used to tell if we just opened the console 
      ///  by using the console key. Currently this is used to fix a bug in SDL
      ///  but it is not limited to that use.
      bool lastKeyWasGlobalActionMap() const { return mLastPressWasGlobalActionMap; }

    void addAcceleratorKey( void *hnd, const String &cmd, U32 keycode, U32 modifier)
    {
        AccKeyMap acc;
        acc.hnd = hnd;
        acc.cmd = cmd;
        acc.keyCode = keycode;
        acc.modifier = modifier;
        mAcceleratorMap.push_back(acc);
    }

    void removeAcceleratorKeys( void *hnd )
    {
         for( int i = 0; i < mAcceleratorMap.size(); )
         {
            if( mAcceleratorMap[i].hnd == hnd )
            {
                mAcceleratorMap.erase( i, 1 );
                continue;
            }

             ++i;
         }
    }

    void removeAcceleratorKey( void *hnd, U32 keycode, U32 modifier )
    {
         for( int i = 0; i < mAcceleratorMap.size(); ++i )
         {
            if( mAcceleratorMap[i].hnd == hnd && mAcceleratorMap[i].keyCode == keycode && mAcceleratorMap[i].modifier == modifier )
            {
                mAcceleratorMap.erase( i, 1 );
                return;
            }
         }
    }
};

#endif // _WINDOW_INPUTGENERATOR_H_
