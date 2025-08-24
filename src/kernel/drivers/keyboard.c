// UK Layout for now..

#include <stdbool.h>
#include <drivers/keyboard.h>
#include <drivers/ports.h>
#include <cpu/isr.h>
#include <drivers/display.h>
#include <kernel/util.h>
#include <kernel/kernel.h>

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define RSHIFT 0x36

static char key_buffer[256];
static bool shift_pressed = false;

#define SC_MAX 57

const char *sc_name[] = {"ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                         "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                         "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                         "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                         "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                         "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

const char sc_ascii_lower[] = {
    '?','?','1','2','3','4','5','6',
    '7','8','9','0','-','=','?','?',
    'q','w','e','r','t','y','u','i',
    'o','p','[',']','?','?','a','s',
    'd','f','g','h','j','k','l',';',
    '\'','`','?','\\','z','x','c','v',
    'b','n','m',',','.','/','?','?','?',' '
};


const char sc_ascii_upper[] = {
    '?','?','!','"','\x9C','$','%','^',
    '&','*','(',')','_','+','?','?',
    'Q','W','E','R','T','Y','U','I',
    'O','P','{','}','?','?','A','S',
    'D','F','G','H','J','K','L',':',
    '@','~','?','|','Z','X','C','V',
    'B','N','M','<','>','?','?','?',' '
};


static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = port_byte_in(0x60);

    // Shift pressed/released
    if (scancode == LSHIFT || scancode == RSHIFT) {
        shift_pressed = true;
        return;
    } 
    if (scancode == (LSHIFT | 0x80) || scancode == (RSHIFT | 0x80)) {
        shift_pressed = false;
        return;
    }

    if (scancode > SC_MAX) return;

    if (scancode == BACKSPACE) {
        if (backspace(key_buffer)) {
            print_backspace();
        }
    } else if (scancode == ENTER) {
        print_nl();
        execute_command(key_buffer);
        key_buffer[0] = '\0';
    } else {
        char letter;
        if (shift_pressed)
            letter = sc_ascii_upper[scancode];
        else
            letter = sc_ascii_lower[scancode];

        append(key_buffer, letter);
        char str[2] = {letter, '\0'};
        print_string(str);
    }
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}

