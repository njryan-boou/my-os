#include "Keyboard.hpp"

#include <input/Input.hpp>
#include <interrupts/PIC.hpp>
#include <io/Print.hpp>

#include <cstdint>

namespace {

bool left_shift_pressed = false;
bool right_shift_pressed = false;
bool caps_lock_enabled = false;


// -------------------------
// Port I/O
// -------------------------

std::uint8_t inb(std::uint16_t port)
{
    std::uint8_t value;

    asm volatile(
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port));

    return value;
}


// -------------------------
// Keyboard state
// -------------------------

bool shift_pressed()
{
    return left_shift_pressed ||
           right_shift_pressed;
}


// -------------------------
// Scancode translation
// -------------------------

char translate_scancode(
    std::uint8_t scancode,
    bool shift,
    bool caps_lock)
{
    const bool uppercase =
        shift != caps_lock;

    switch (scancode)
    {
        // Numbers
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';

        // Number-row punctuation
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';

        // Top row
        case 0x10: return uppercase ? 'Q' : 'q';
        case 0x11: return uppercase ? 'W' : 'w';
        case 0x12: return uppercase ? 'E' : 'e';
        case 0x13: return uppercase ? 'R' : 'r';
        case 0x14: return uppercase ? 'T' : 't';
        case 0x15: return uppercase ? 'Y' : 'y';
        case 0x16: return uppercase ? 'U' : 'u';
        case 0x17: return uppercase ? 'I' : 'i';
        case 0x18: return uppercase ? 'O' : 'o';
        case 0x19: return uppercase ? 'P' : 'p';

        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';

        // Enter
        case 0x1C: return '\n';

        // Home row
        case 0x1E: return uppercase ? 'A' : 'a';
        case 0x1F: return uppercase ? 'S' : 's';
        case 0x20: return uppercase ? 'D' : 'd';
        case 0x21: return uppercase ? 'F' : 'f';
        case 0x22: return uppercase ? 'G' : 'g';
        case 0x23: return uppercase ? 'H' : 'h';
        case 0x24: return uppercase ? 'J' : 'j';
        case 0x25: return uppercase ? 'K' : 'k';
        case 0x26: return uppercase ? 'L' : 'l';

        // Home-row punctuation
        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';
        case 0x29: return shift ? '~' : '`';

        // Backslash
        case 0x2B: return shift ? '|' : '\\';

        // Bottom row
        case 0x2C: return uppercase ? 'Z' : 'z';
        case 0x2D: return uppercase ? 'X' : 'x';
        case 0x2E: return uppercase ? 'C' : 'c';
        case 0x2F: return uppercase ? 'V' : 'v';
        case 0x30: return uppercase ? 'B' : 'b';
        case 0x31: return uppercase ? 'N' : 'n';
        case 0x32: return uppercase ? 'M' : 'm';

        // Bottom-row punctuation
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';

        // Space
        case 0x39: return ' ';

        default:
            return '\0';
    }
}

}


namespace kernel::drivers::keyboard {

void handle_interrupt()
{
    const std::uint8_t scancode =
        inb(0x60);


    // -------------------------
    // Left Shift
    // -------------------------

    if (scancode == 0x2A)
    {
        left_shift_pressed = true;
    }
    else if (scancode == 0xAA)
    {
        left_shift_pressed = false;
    }


    // -------------------------
    // Right Shift
    // -------------------------

    else if (scancode == 0x36)
    {
        right_shift_pressed = true;
    }
    else if (scancode == 0xB6)
    {
        right_shift_pressed = false;
    }


    // -------------------------
    // Caps Lock
    // -------------------------

    else if (scancode == 0x3A)
    {
        caps_lock_enabled =
            !caps_lock_enabled;
    }


    // -------------------------
    // Backspace
    // -------------------------

    else if (scancode == 0x0E)
    {
        kernel::input::backspace();
        io::backspace();
    }


    // -------------------------
    // Tab
    // -------------------------

    else if (scancode == 0x0F)
    {
        io::tab();
    }


    // -------------------------
    // Normal key press
    // -------------------------

    else if ((scancode & 0x80) == 0)
    {
        const char character =
            translate_scancode(
                scancode,
                shift_pressed(),
                caps_lock_enabled);

        if (character != '\0')
        {
            kernel::input::push(character);
            io::print(character);
        }
    }


    // -------------------------
    // Finish IRQ
    // -------------------------

    kernel::interrupts::send_eoi();
}

}