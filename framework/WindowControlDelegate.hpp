/**
 @file      WindowControlDelegate.hpp
 @author    Tyrone Davison
 @date      October 2012
 */

#pragma once
#ifndef __TYGA_WINDOW_CONTROL_DELEGATE__
#define __TYGA_WINDOW_CONTROL_DELEGATE__

#include <memory>

namespace tyga
{

class Window;

/**
 Enumeration of non-printable keyboard keys.
 Printable keys are represented by their capital ASCII value.
 */
enum WindowKeyIndex
{
    kWindowKeySpace = 32,
    kWindowKeySpecial = 256,
    kWindowKeyEsc,
    kWindowKeyF1,
    kWindowKeyF2,
    kWindowKeyF3,
    kWindowKeyF4,
    kWindowKeyF5,
    kWindowKeyF6,
    kWindowKeyF7,
    kWindowKeyF8,
    kWindowKeyF9,
    kWindowKeyF10,
    kWindowKeyF11,
    kWindowKeyF12,
    kWindowKeyF13,
    kWindowKeyF14,
    kWindowKeyF15,
    kWindowKeyF16,
    kWindowKeyF17,
    kWindowKeyF18,
    kWindowKeyF19,
    kWindowKeyF20,
    kWindowKeyF21,
    kWindowKeyF22,
    kWindowKeyF23,
    kWindowKeyF24,
    kWindowKeyF25,
    kWindowKeyUp,
    kWindowKeyDown,
    kWindowKeyLeft,
    kWindowKeyRight,
    kWindowKeyLShift,
    kWindowKeyRShift,
    kWindowKeyLCtrl,
    kWindowKeyRCtrl,
    kWindowKeyLAlt,
    kWindowKeyRAlt,
    kWindowKeyTab,
    kWindowKeyEnter,
    kWindowKeyBackspace,
    kWindowKeyInsert,
    kWindowKeyDel,
    kWindowKeyPageUp,
    kWindowKeyPageDown,
    kWindowKeyHome,
    kWindowKeyEnd,
    kWindowKeyPad0,
    kWindowKeyPad1,
    kWindowKeyPad2,
    kWindowKeyPad3,
    kWindowKeyPad4,
    kWindowKeyPad5,
    kWindowKeyPad6,
    kWindowKeyPad7,
    kWindowKeyPad8,
    kWindowKeyPad9,
    kWindowKeyPadDivide,
    kWindowKeyPadMultiply,
    kWindowKeyPadSubtract,
    kWindowKeyPadAdd,
    kWindowKeyPadDecimal,
    kWindowKeyPadEqual,
    kWindowKeyPadEnter,
    kWindowKeyPadNumLock,
    kWindowKeyCapsLock,
    kWindowKeyScrollLock,
    kWindowKeyPause,
    kWindowKeyLSuper,
    kWindowKeyRSuper
};

/**
 Enumeration of mouse button indices.
 */
enum WindowMouseButtonIndex
{
    kWindowMouseButtonLeft,
    kWindowMouseButtonRight,
    kWindowMouseButtonMiddle
};

/**
 Enumeration of gamepad axis.
 */
enum WindowGamepadAxisIndex
{
    kWindowGamepadAxisLeftThumbX,
    kWindowGamepadAxisLeftThumbY,
    kWindowGamepadAxisLeftTrigger,
    kWindowGamepadAxisRightThumbY,
    kWindowGamepadAxisRightThumbX,
    kWindowGamepadAxisReserved0,
    kWindowGamepadAxisReserved1,
    kWindowGamepadAxisRightTrigger
};

/**
 Enumeration of gamepad buttons.
 */
enum WindowGamepadButtonIndex
{
    kWindowGamepadButtonA,
    kWindowGamepadButtonB,
    kWindowGamepadButtonX,
    kWindowGamepadButtonY,
    kWindowGamepadButtonLeftShoulder,
    kWindowGamepadButtonRightShoulder,
    kWindowGamepadButtonBack,
    kWindowGamepadButtonStart,
    kWindowGamepadButtonLeftThumb,
    kWindowGamepadButtonRightThumb,
    kWindowGamepadButtonReserved0,
    kWindowGamepadButtonReserved1
};

/**
 A window object can use a delegate object that implements this interface
 to define how the window should respond to user and operating system events.
 */
class WindowControlDelegate
{
public:
    /**
     Tells the delegate that it will start to be used to control the window.
     Optional method.
     @param window  The window object using the delegate.
     */
    virtual void
    windowControlWillStart(std::shared_ptr<tyga::Window> window) {}

    /**
     Tells the delegate that it is no longer being used to control the window.
     Optional method.
     @param window  The window object using the delegate.
     */
    virtual void
    windowControlDidStop(std::shared_ptr<tyga::Window> window) {}

    /**
     Tells the delegate that the window's view delegate is about to draw.
     Optional method.
     @param window  The window object using the delegate.
     */
    virtual void
    windowControlViewWillRender(std::shared_ptr<tyga::Window> window) {}

    /**
     Tells the delegate that the mouse cursor has moved.
     Optional method.
     @param window  The window object using the delegate.
     @param x       The new window-based x-coordinate of the cursor.
     @param y       The new window-based y-coordinate of the cursor.
     @remark        The y-axis for the window coordinate system is downwards.
     */
    virtual void
    windowControlMouseMoved(std::shared_ptr<tyga::Window> window,
                            int x,
                            int y) {}		

    /**
     Tells the delegate that a mouse button has been pressed or released.
     Optional method.
     @param window          The window object using the delegate.
     @param button_index    Zero-based index of the button triggering the event.
     @param down            Boolean true if the button is pressed down.
     */
    virtual void
    windowControlMouseButtonChanged(std::shared_ptr<tyga::Window> window,
                                    int button_index,
                                    bool down) {}

    /**
     Tells the delegate that the mouse wheel has moved.
     Optional method.
     @param window  The window object using the delegate.
     @param pos     The integer position of the mouse wheel.
     */
    virtual void
    windowControlMouseWheelMoved(std::shared_ptr<tyga::Window> window,
                                 int position) {}

    /**
     Tells the delegate that a keyboard key has been pressed or released.
     Optional method.
     @param window      The window object using the delegate.
     @param key_index   Zero-based index of the key triggering the event.
     @param down        Boolean true if the key is pressed down.
     */
    virtual void
    windowControlKeyboardChanged(std::shared_ptr<tyga::Window> window,
                                 int key_index,
                                 bool down) {}

    /**
     Tells the delegate that an analogue input on a gamepad has moved.
     Optional method.
     @param window         The window object using the delegate.
     @param gamepad_index  Zero-based index of the gamepad triggering the event.
     @param axis_index     Zero-based index of the axis triggering the event.
     @param pos            Normalized [-1,1] position of the axiz.
     */
    virtual void
    windowControlGamepadAxisMoved(std::shared_ptr<tyga::Window> window,
                                  int gamepad_index,
                                  int axis_index,
                                  float pos) {}

    /**
     Tells the delegate that a button on a gamepad has been pressed or released.
     Optional method.
     @param window         The window object using the delegate.
     @param gamepad_index  Zero-based index of the gamepad triggering the event.
     @param button_index   Zero-based index of the button triggering the event.
     @param down           Boolean true if the button is pressed down.
     */
    virtual void
    windowControlGamepadButtonChanged(std::shared_ptr<tyga::Window> window,
                                      int gamepad_index,
                                      int button_index,
                                      bool down) {}
};

} // end namespace tyga

#endif
