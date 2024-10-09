#include <Keypad.h>
#define ROWS  4 
#define COLS  4 

class myKeyboard{
private:
  const char kp4x4Keys[ROWS][COLS]  = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'E', '0', 'F', 'D'}
  };
  byte rowKp4x4Pin [4] = {27, 14, 12, 13};
  byte colKp4x4Pin [4] = {32, 33, 25, 26};
  Keypad keypad = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);
  char key_pressed = '\0';
  //uint8_t kcode;

public:
   void scan_keyboard();
   void key_reset() { key_pressed = '\0'; }
   uint8_t pressed_key() { return key_pressed; }
};

void myKeyboard::scan_keyboard(){  
  char znak = keypad.getKey();
  if(znak) key_pressed = znak;  
}



