/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "InputManager.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace dit {

    //-----------------------------------//

    /**
     * Input actions abstract an action triggered by an input binding.
     * This allows the code that handles the action to be unaware of the
     * specific combination that triggered the action, and provides
     * support for remapping of actions by the users of the application.
     */

    class InputAction
    {
    public:

        // Adds a new binding for the action.
        void AddBinding();

        // Name of the action.
        String name;
    };

    //-----------------------------------//

    /**
     * Input schemas provide grouping for input actions. This can be useful
     * when you have distinct modes in your application, for example a 'Menu'
     * schema, a 'Driving' schema aswell as a 'Flying' schema.
     */

    class InputSchema
    {
    public:
        InputAction* CreateAction(const String& name);

        String name;
    };

    //-----------------------------------//

    /**
     * Manages all the input schemas, bindings and keeps track of
     * the current schema aswell as matching the input events to
     * the specific input actions in the schemas.
     */

    class InputMap
    {
    public:
        InputMap(InputBackend& input);
        ~InputMap();

        // Creates a new input schema.
        InputSchema* CreateSchema(const String& name);

        // Sets the current input schema.
        void SetSchema(InputSchema* schema);

    protected:

        // Input callbacks.
        void OnKeyPress(const KeyEvent& event);
        void OnMousePress(const MouseButtonEvent& event);
    };

}