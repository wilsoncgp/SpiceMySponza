/**
 @file    Window.hpp
 @author  Tyrone Davison
 @date    September 2012
 */

#pragma once
#ifndef __TYGA_WINDOW__
#define __TYGA_WINDOW__

#include <memory>
#include <string>
#include <vector>

namespace tyga
{

class WindowViewDelegate;
class WindowControlDelegate;

/**
 Provides an OpenGL compatible rendering window for use with
 WindowViewDelegate and WindowControlDelegate objects.
 @remark    Currently it is only possible to have a single window. This
            single instance is accessible via the Window#mainWindow method.
 */
class Window : public std::enable_shared_from_this<Window>
{
public:
    ~Window();
        
    /**
     Obtains the application's primary Window object.
     */
    static std::shared_ptr<Window>
    mainWindow();

    /**
     Obtains the window's current view delegate.
     */
    std::shared_ptr<WindowViewDelegate>
    view() const;

    /**
     Assigns a view delegate for use by the window.
     */
    void
    setView(std::shared_ptr<WindowViewDelegate> view);

    /**
     Obtains the window's current control delegate.
     */
    std::shared_ptr<WindowControlDelegate>
    controller() const;

    /**
     Assigns a control delegate for use by the window.
     */
    void
    setController(std::shared_ptr<WindowControlDelegate> controller);

    /**
     Creates and makes visible an operating system window for this object.
     @param width        The preferred width of the window's usable area.
     @param height       The preferred height of the window's usable area.
     @param sample_count The preferred number of samples for OpenGL's MSAA.
     @param windowed     Boolean indicating if the window should open on the 
                         desktop or whether it should be fullscreen.
     @return             Boolean indicating success of the operation.
     */
    bool
    open(int width,
         int height,
         int sample_count,
         bool windowed);

    /**
     Determines if the operating system window is open.
     */
    bool
    isVisible() const;

    /**
     Performs a window redraw using the view delegate and dequeues any
     operating system events sending them to the control delegate.
     This method must be called regularly, usually within the runloop.
     This call is only valid once the window is open.
     */
    void
    update();

    /**
     Closes (hides and destroys) the operating system window.
     */
    void
    close();

    /**
     Assign a title to the window (if not fullscreen).
     This call is only valid once the window is open.
     */
    void
    setTitle(std::string newTitle);

    /**
     Resize the window's usable area to the given dimensions.
     This call is only valid once the window is open.
     */
    void
    resize(int width,
           int height);

    /**
     Change the mode of the window to/from fullscreen.
     Not currently implemented.
     This call is only valid once the window is open.
     */
    void
    setFullscreen(bool yes);

private:

    static void
    onResize(int width,
             int height);
    
    static int
    onClose();

    static void
    onMouseMove(int x,
                int y);

    static void
    onMouseWheel(int pos);

    static void
    onMouseButton(int button,
                  int action);

    static void
    onKeyEvent(int key,
               int action);

    static void
    pollGamepads();
    
    Window();

    static std::shared_ptr<Window> main_window_;
    static const int MAX_GAMEPADS = 2;
    static const int MAX_GAMEPAD_AXES = 8;
    static const int MAX_GAMEPAD_BUTTONS = 12;
    struct GamepadState
    {
        float axis[MAX_GAMEPAD_AXES];
        unsigned char button[MAX_GAMEPAD_BUTTONS];
        GamepadState();
    };
    static GamepadState gamepad_state_[MAX_GAMEPADS];
    std::shared_ptr<WindowViewDelegate> view_;
    std::shared_ptr<WindowControlDelegate> controller_;
};

} // end namespace tyga

#endif
