
#include <commonDefs.h>
#include <stringUtil.h>
#include <term.h>

#define KB_DATA_PORT 0x60
#define KB_CONTROL_PORT 0x64

/* US Keyboard Layout.  */
unsigned char kbdus[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
   '9', '0', '-', '=', '\b',  /* Backspace */
   '\t',       /* Tab */
   'q', 'w', 'e', 'r',  /* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   /* Enter key */
   0,       /* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  /* 39 */
   '\'', '`',   0,      /* Left shift */
   '\\', 'z', 'x', 'c', 'v', 'b', 'n',       /* 49 */
   'm', ',', '.', '/',   0,            /* Right shift */
   '*',
   0, /* Alt */
   ' ',  /* Space bar */
   0, /* Caps lock */
   0, /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,
   0, /* < ... F10 */
   0, /* 69 - Num lock*/
   0, /* Scroll Lock */
   0, /* Home key */
   0, /* Up Arrow */
   0, /* Page Up */
   '-',
   0, /* Left Arrow */
   0,
   0, /* Right Arrow */
   '+',
   0, /* 79 - End key*/
   0, /* Down Arrow */
   0, /* Page Down */
   0, /* Insert Key */
   0, /* Delete Key */
   0,   0,   '\\',
   0, /* F11 Key */
   0, /* F12 Key */
   0, /* All other keys are undefined */
};

unsigned char kbdus_up[128] =
{
   0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
   '(', ')', '_', '+', '\b',  /* Backspace */
   '\t',       /* Tab */
   'Q', 'W', 'E', 'R',  /* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   /* Enter key */
   0,       /* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  /* 39 */
   '\"', '~',   0,      /* Left shift */
   '|', 'Z', 'X', 'C', 'V', 'B', 'N',       /* 49 */
   'M', '<', '>', '?',   0,            /* Right shift */
   '*',
   0, /* Alt */
   ' ',  /* Space bar */
   0, /* Caps lock */
   0, /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,
   0, /* < ... F10 */
   0, /* 69 - Num lock*/
   0, /* Scroll Lock */
   0, /* Home key */
   0, /* Up Arrow */
   0, /* Page Up */
   '-',
   0, /* Left Arrow */
   0,
   0, /* Right Arrow */
   '+',
   0, /* 79 - End key*/
   0, /* Down Arrow */
   0, /* Page Down */
   0, /* Insert Key */
   0, /* Delete Key */
   0,   0,  '|',
   0, /* F11 Key */
   0, /* F12 Key */
   0, /* All other keys are undefined */
};

unsigned char *us_kb_layouts[2] = {
   kbdus, kbdus_up
};

uint8_t numkey[128] = {
   [71] = '7', '8', '9',
   [75] = '4', '5', '6',
   [79] = '1', '2', '3',
   [82] = '0', '.'
};

#define KEY_L_SHIFT 42
#define KEY_R_SHIFT 54
#define NUM_LOCK 69
#define CAPS_LOCK 58
#define KEY_PAGE_UP 0x49
#define KEY_PAGE_DOWN 0x51
#define KEY_LEFT 0x4b
#define KEY_RIGHT 0x4d
#define KEY_UP 0x48
#define KEY_DOWN 0x50

bool pkeys[128] = { false };
bool e0pkeys[128] = { false };

bool *pkeysArrays[2] = { pkeys, e0pkeys };

bool numLock = true;
bool capsLock = false;
bool lastWasE0 = false;

uint8_t next_kb_interrupts_to_ignore = 0;

void kbd_wait()
{
   uint8_t al;

   do {

      while ((al = inb(KB_CONTROL_PORT)) & 1) {
         // drain input data..
         (void) inb(KB_DATA_PORT);
      }

   } while (al & 2);
}

void kb_led_set(uint8_t val)
{
   kbd_wait();

   outb(KB_DATA_PORT, 0xED);
   kbd_wait();

   outb(KB_DATA_PORT, val & 7);
   kbd_wait();
}

void num_lock_switch(bool val)
{
   kb_led_set(capsLock << 2 | val << 1);
}

void caps_lock_switch(bool val)
{
   kb_led_set(numLock << 1 | val << 2);
}

void init_kb()
{
   num_lock_switch(numLock);
   caps_lock_switch(capsLock);
}

void handle_key_pressed(uint8_t scancode)
{
   switch(scancode) {

   case NUM_LOCK:
      numLock = !numLock;
      num_lock_switch(numLock);
      printk("\nNUM LOCK is %s\n", numLock ? "ON" : "OFF");
      return;

   case CAPS_LOCK:
      capsLock = !capsLock;
      caps_lock_switch(capsLock);
      printk("\nCAPS LOCK is %s\n", capsLock ? "ON" : "OFF");
      return;

   case KEY_L_SHIFT:
   case KEY_R_SHIFT:
      return;

   default:
      break;
   }

   uint8_t *layout = us_kb_layouts[pkeys[KEY_L_SHIFT] || pkeys[KEY_R_SHIFT]];
   uint8_t c = layout[scancode];

   if (numLock) {
      c |= numkey[scancode];
   }

   if (capsLock) {
      c = upper(c);
   }

   if (c) {
      term_write_char(c);
   } else {
      printk("PRESSED scancode: 0x%x (%i)\n", scancode, scancode);
   }
}

void handle_E0_key_pressed(uint8_t scancode)
{
   switch (scancode) {

   case KEY_PAGE_UP:
      term_scroll(term_get_scroll_value() + 5);
      break;

   case KEY_PAGE_DOWN:
      term_scroll(term_get_scroll_value() - 5);
      break;

   default:
      printk("PRESSED E0 scancode: 0x%x (%i)\n", scancode, scancode);
      break;
   }
}

void (*keyPressHandlers[2])(uint8_t) = {
   handle_key_pressed, handle_E0_key_pressed
};

void keyboard_handler()
{
   uint8_t scancode;

   while (inb(KB_CONTROL_PORT) & 2) {
      //check if scancode is ready
      //this is useful since sometimes the IRQ is triggered before
      //the data is available.
   }

   /* Read from the keyboard's data buffer */
   scancode = inb(KB_DATA_PORT);

   if (next_kb_interrupts_to_ignore) {
      next_kb_interrupts_to_ignore--;
      return;
   }

   if (scancode == 0xE1) {
      next_kb_interrupts_to_ignore = 2;
      return;
   }


   if (scancode == 0xE0) {
      lastWasE0 = true;
      return;
   }


   if (lastWasE0) {
      // Fake lshift pressed (2A) or released (AA)
      if (scancode == 0x2A || scancode == 0xAA) {
         goto end;
      }
   }

   if (scancode & 0x80) {

      pkeysArrays[lastWasE0][scancode & ~0x80] = false;

   } else {

      pkeysArrays[lastWasE0][scancode] = true;
      keyPressHandlers[lastWasE0](scancode);
   }

end:
   lastWasE0 = false;
}

