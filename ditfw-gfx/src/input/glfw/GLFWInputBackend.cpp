/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "GLFWInputManager.h"

namespace dit {

    Keys GLFWInputManager::ConvertFromGLFWKey(int32 key)
    {
        switch(key)
        {
        case GLFW_KEY_A: return Keys::A;
        case GLFW_KEY_B: return Keys::B;
        case GLFW_KEY_C: return Keys::C;
        case GLFW_KEY_D: return Keys::D;
        case GLFW_KEY_E: return Keys::E;
        case GLFW_KEY_F: return Keys::F;
        case GLFW_KEY_G: return Keys::G;
        case GLFW_KEY_H: return Keys::H;
        case GLFW_KEY_I: return Keys::I;
        case GLFW_KEY_J: return Keys::J;
        case GLFW_KEY_K: return Keys::K;
        case GLFW_KEY_L: return Keys::L;
        case GLFW_KEY_M: return Keys::M;
        case GLFW_KEY_N: return Keys::N;
        case GLFW_KEY_O: return Keys::O;
        case GLFW_KEY_P: return Keys::P;
        case GLFW_KEY_Q: return Keys::Q;
        case GLFW_KEY_R: return Keys::R;
        case GLFW_KEY_S: return Keys::S;
        case GLFW_KEY_T: return Keys::T;
        case GLFW_KEY_U: return Keys::U;
        case GLFW_KEY_V: return Keys::V;
        case GLFW_KEY_W: return Keys::W;
        case GLFW_KEY_X: return Keys::X;
        case GLFW_KEY_Y: return Keys::Y;
        case GLFW_KEY_Z: return Keys::Z;

        case GLFW_KEY_0: return Keys::Num0;
        case GLFW_KEY_1: return Keys::Num1;
        case GLFW_KEY_2: return Keys::Num2;
        case GLFW_KEY_3: return Keys::Num3;
        case GLFW_KEY_4: return Keys::Num4;
        case GLFW_KEY_5: return Keys::Num5;
        case GLFW_KEY_6: return Keys::Num6;
        case GLFW_KEY_7: return Keys::Num7;
        case GLFW_KEY_8: return Keys::Num8;
        case GLFW_KEY_9: return Keys::Num9;

        case GLFW_KEY_ESCAPE:   return Keys::Escape;
        case GLFW_KEY_LEFT_CONTROL: return Keys::LControl;
        case GLFW_KEY_LEFT_SHIFT:   return Keys::LShift;
        case GLFW_KEY_LEFT_ALT:     return Keys::LAlt;
        case GLFW_KEY_LEFT_SUPER:  return Keys::LSuper;
        case GLFW_KEY_RIGHT_CONTROL: return Keys::RControl;
        case GLFW_KEY_RIGHT_SHIFT:   return Keys::RShift;
        case GLFW_KEY_RIGHT_ALT:     return Keys::RAlt;
        case GLFW_KEY_RIGHT_SUPER:  return Keys::RSuper;
        case GLFW_KEY_MENU:     return Keys::Menu;

        case GLFW_KEY_LEFT_BRACKET:  return Keys::LBracket;
        case GLFW_KEY_RIGHT_BRACKET:  return Keys::RBracket;
        case GLFW_KEY_SEMICOLON: return Keys::SemiColon;
        case GLFW_KEY_COMMA:     return Keys::Comma;
        case GLFW_KEY_PERIOD:    return Keys::Period;
        //case GLFW_KEY_:     return Keys::Quote;
        case GLFW_KEY_SLASH:     return Keys::Slash;
        case GLFW_KEY_BACKSLASH: return Keys::BackSlash;
        //case GLFW_KEY_TILDE:     return Keys::Tilde;
        case GLFW_KEY_EQUAL:     return Keys::Equal;
        case GLFW_KEY_MINUS:      return Keys::Dash;

        case GLFW_KEY_SPACE:  return Keys::Space;
        case GLFW_KEY_ENTER: return Keys::Return;
        case GLFW_KEY_BACKSPACE:  return Keys::Back;
        case GLFW_KEY_TAB:    return Keys::Tab;

        case GLFW_KEY_PAGE_UP:   return Keys::PageUp;
        case GLFW_KEY_PAGE_DOWN: return Keys::PageDown;
        case GLFW_KEY_END:      return Keys::End;
        case GLFW_KEY_HOME:     return Keys::Home;
        case GLFW_KEY_INSERT:   return Keys::Insert;
        case GLFW_KEY_DELETE:   return Keys::Delete;

        //case GLFW_KEY_:      return Keys::Add;
        //case sf::Keyboard::Subtract: return Keys::Subtract;
        //case sf::Keyboard::Multiply: return Keys::Multiply;
        //case sf::Keyboard::Divide:   return Keys::Divide;

        case GLFW_KEY_LEFT:  return Keys::Left;
        case GLFW_KEY_RIGHT: return Keys::Right;
        case GLFW_KEY_UP:    return Keys::Up;
        case GLFW_KEY_DOWN:  return Keys::Down;

        case GLFW_KEY_KP_0: return Keys::Numpad0;
        case GLFW_KEY_KP_1: return Keys::Numpad1;
        case GLFW_KEY_KP_2: return Keys::Numpad2;
        case GLFW_KEY_KP_3: return Keys::Numpad3;
        case GLFW_KEY_KP_4: return Keys::Numpad4;
        case GLFW_KEY_KP_5: return Keys::Numpad5;
        case GLFW_KEY_KP_6: return Keys::Numpad6;
        case GLFW_KEY_KP_7: return Keys::Numpad7;
        case GLFW_KEY_KP_8: return Keys::Numpad8;
        case GLFW_KEY_KP_9: return Keys::Numpad9;

        case GLFW_KEY_F1: return Keys::F1;
        case GLFW_KEY_F2: return Keys::F2;
        case GLFW_KEY_F3: return Keys::F3;
        case GLFW_KEY_F4: return Keys::F4;
        case GLFW_KEY_F5: return Keys::F5;
        case GLFW_KEY_F6: return Keys::F6;
        case GLFW_KEY_F7: return Keys::F7;
        case GLFW_KEY_F8: return Keys::F8;
        case GLFW_KEY_F9: return Keys::F9;
        case GLFW_KEY_F10: return Keys::F10;
        case GLFW_KEY_F11: return Keys::F11;
        case GLFW_KEY_F12: return Keys::F12;
        case GLFW_KEY_F13: return Keys::F13;
        case GLFW_KEY_F14: return Keys::F14;
        case GLFW_KEY_F15: return Keys::F15;

        case GLFW_KEY_PAUSE: return Keys::Pause;

        default:
            LogWarn( "Could not map GLFW keycode: Invalid keycode" );
            return Keys::Escape;
        }
    }

    MouseButton GLFWInputManager::ConvertFromGLFWMouseButton(int32 button)
    {
        switch(button)
        {
        case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        case GLFW_MOUSE_BUTTON_4: return MouseButton::Mouse4;
        case GLFW_MOUSE_BUTTON_5: return MouseButton::Mouse5;
        }

        fld_unreachable("Could not map GLFW mouse button: Invalid button");
    }

#if 0
    //-----------------------------------//

    static bool isMouseEvent(sf::Event::EventType eventType)
    {
        return eventType == sf::Event::MouseButtonPressed 
            || eventType == sf::Event::MouseButtonReleased 
            || eventType == sf::Event::MouseMoved
            || eventType == sf::Event::MouseWheelMoved 
            || eventType == sf::Event::MouseEntered 
            || eventType == sf::Event::MouseLeft;
    }

    //-----------------------------------//

    static bool isKeyboardEvent(sf::Event::EventType eventType)
    {
        return eventType == sf::Event::KeyPressed 
            || eventType == sf::Event::KeyReleased
            || eventType == sf::Event::TextEntered;
    }

    //-----------------------------------//

    static bool isJoystickEvent(sf::Event::EventType eventType)
    {
        return eventType == sf::Event::JoystickButtonPressed 
            || eventType == sf::Event::JoystickButtonReleased 
            || eventType == sf::Event::JoystickConnected
            || eventType == sf::Event::JoystickDisconnected
            || eventType == sf::Event::JoystickMoved;
    }

    //-----------------------------------//

    static JoystickAxis ConvertJoystickAxisEnum(sf::Joystick::Axis axis)
    {
        switch(axis)
        {
        case sf::Joystick::X: return JoystickAxis::AxisX;
        case sf::Joystick::Y: return JoystickAxis::AxisY;
        case sf::Joystick::Z: return JoystickAxis::AxisZ;
        case sf::Joystick::R: return JoystickAxis::AxisR;
        case sf::Joystick::U: return JoystickAxis::AxisU;
        case sf::Joystick::V: return JoystickAxis::AxisV;
        case sf::Joystick::PovX: return JoystickAxis::AxisPOV;
        case sf::Joystick::PovY: return JoystickAxis::AxisPOV;
        }

        fld_unreachable("Could not map GLFW joystick axis: Invalid axis");
    }

    //-----------------------------------//

    void GLFWInputManager::processGLFWEvent( const sf::Event& event )
    {
        if( isMouseEvent(event.type) )
        {
            processMouseEvent(event);
        }
        else if ( isKeyboardEvent(event.type) )
        {
            processKeyboardEvent(event);
        }
        else if ( isJoystickEvent(event.type) )
        {
            processJoystickEvent(event);
        }
    }

    //-----------------------------------//

    void GLFWInputManager::processMouseEvent(const sf::Event& event)
    {
        short mbX = static_cast<short>(event.mouseButton.x); 
        short mbY = static_cast<short>(event.mouseButton.y);

        switch(event.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            MouseButtonEvent mbe( MouseEventType::MousePress );
        
            mbe.x = mbX;
            mbe.y = mbY;
            mbe.button = ConvertMouseButtonEnum( event.mouseButton.button );
        
            processEvent( mbe );
            break;
        }	

        case sf::Event::MouseButtonReleased:
        {
            MouseButtonEvent mbe( MouseEventType::MouseRelease );
        
            mbe.x = mbX;
            mbe.y = mbY;
            mbe.button = ConvertMouseButtonEnum( event.mouseButton.button );

            processEvent( mbe );
            break;
        }

        case sf::Event::MouseMoved:
        {
            short mmX = static_cast<short>(event.mouseMove.x);
            short mmY = static_cast<short>(event.mouseMove.y);
        
            MouseMoveEvent mme;
            mme.x = mmX;
            mme.y = mmY;

            processEvent( mme );
            break;
        }

        case sf::Event::MouseWheelMoved:
        {
            MouseWheelEvent mwe;
            mwe.delta = short(event.mouseWheel.delta);
            processEvent( mwe );

            break;
        }

        case sf::Event::MouseEntered:
        {
            MouseEvent me( MouseEventType::MouseEnter );
            processEvent( me );

            break;
        }

        case sf::Event::MouseLeft:
        {
            MouseEvent me( MouseEventType::MouseExit );
            processEvent( me );

            break;
        } }
    }

    //-----------------------------------//

    void GLFWInputManager::processKeyboardEvent(const sf::Event& event)
    {
        switch( event.type )
        {
        case sf::Event::KeyPressed:
        {
            KeyEvent ke(KeyboardEventType::KeyPressed);
            ke.keyCode = ConvertKeyEnum( event.key.code );
            ke.altPressed = event.key.alt;
            ke.shiftPressed = event.key.shift;
            ke.ctrlPressed = event.key.control;
            processEvent( ke );
            break;
        }
    
        case sf::Event::KeyReleased:
        {
            KeyEvent ke(KeyboardEventType::KeyReleased);
            ke.keyCode = ConvertKeyEnum( event.key.code );
            ke.altPressed = event.key.alt;
            ke.shiftPressed = event.key.shift;
            ke.ctrlPressed = event.key.control;
            processEvent( ke );
            break;
        }

        case sf::Event::TextEntered:
        {
            KeyEvent ke(KeyboardEventType::KeyText);
            ke.unicode = event.text.unicode;
            processEvent(ke);
            break;
        } }
    }

    //-----------------------------------//

    void GLFWInputManager::processJoystickEvent(const sf::Event& event)
    {
        switch( event.type )
        {
        case sf::Event::JoystickButtonPressed:
        {
            JoyButtonEvent jbe( event.joystickButton.joystickId, 
                event.joystickButton.button, JoystickEventType::JoystickPress );
            processEvent( jbe );
            break;
        }

        case sf::Event::JoystickButtonReleased:
        {
            JoyButtonEvent jbe( event.joystickButton.joystickId, 
                event.joystickButton.button, JoystickEventType::JoystickRelease );
            processEvent( jbe );
            break;
        }

        case sf::Event::JoystickMoved:
        {
            JoyMoveEvent jme( event.joystickMove.joystickId, 
            ConvertJoystickAxisEnum( event.joystickMove.axis ), 
            event.joystickMove.position );
            processEvent( jme ) ;
            break;
        } }
    }
    //-----------------------------------//
#endif
}