-- Example Lua Script for Input

function update(dt)

    if isKeyDown("T") then
        print("hola")
    end

    if isKeyPressed("Y") then
        print("Adios")
    end

end

-- Available Keys:
-- Letters: A-Z
-- Numbers: 0-9
-- Arrow Keys: UP, DOWN, LEFT, RIGHT
-- Control Keys: ENTER, ESC, SPACE, BACKSPACE, TAB, CAPSLOCK,
--               INSERT, DELETE, HOME, END, PAGEUP, PAGEDOWN,
--               PRINTSCREEN, SCROLLLOCK, PAUSE
-- Modifier Keys: LCTRL, RCTRL, CTRL, LSHIFT, RSHIFT, SHIFT, LALT, RALT
-- Function Keys: F1 - F12
-- Numpad: NUM0 - NUM9, NUM+, NUM-, NUM*, NUM/, NUM., NUMENTER
-- Example: isKeyPressed("W"), isKeyDown("SPACE")
