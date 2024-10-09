#include "Arduino.h"
#include <SPI.h>
#include <TFT_eSPI.h> 
#include "bus.h"
#include "stack.h"
#include "keyboard.h"



class Cpu{
private:
    
    TFT_eSPI tft = TFT_eSPI();
    Bus* bus;
    myKeyboard* my_keyboard; 
    uint8_t vx_registers[16];
    uint16_t pc;
    Stack<uint16_t> stack;
    uint16_t i_register;
    uint8_t delay_timer_register;
    uint32_t last_time_update_disp = 0;
    //uint32_t last_time_update_key = 0;       
    
public:
    Cpu(Bus* bus_adr, myKeyboard* keyboard_adr);
    void run_ins();
    void display_sprite(uint8_t x, uint8_t y, uint8_t h);
    void display_sprite_big(uint8_t x, uint8_t y);
    void display_clear();
    void set_delay_timer(uint8_t val);
    uint8_t get_delay_timer();
    void tick();
};

void Cpu::set_delay_timer(uint8_t val){
  delay_timer_register = val;
}
uint8_t Cpu::get_delay_timer(){
  return delay_timer_register;
}
  

void Cpu::display_clear(){
  bus->vram_clear();
}

Cpu::Cpu(Bus* bus_adr, myKeyboard* keyboard_adr){
  
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  bus = bus_adr;
  my_keyboard = keyboard_adr;
  memset(vx_registers, 0x00, sizeof(vx_registers));
  pc = 0x200;
  i_register = 0x00;
  delay_timer_register = 0x00;
}

void Cpu::tick(){
  if(delay_timer_register > 0) delay_timer_register--;
}

void Cpu::run_ins(){
  
  uint8_t hi = bus->ram_read_byte(pc);
  uint8_t lo = bus->ram_read_byte(pc + 1);
  uint16_t ins = (hi << 8) | lo;
  //Serial.println(ins, HEX);
  uint16_t nnn = ins & 0xfff;
  uint8_t n = ins & 0x0f;
  uint8_t x = (ins & 0x0f00) >> 8;
  uint8_t y = (ins & 0x00f0) >> 4;
  uint8_t kk = ins & 0x0ff;
  pc += 2;

  switch((ins & 0xf000) >> 12){
    case 0x0:{
      switch(kk){
        case 0xC0:{
                  //sroll down by n
                  bus->vram_scroll_down(n);                  
        break;
        }
        case 0xFB:{        
          //scroll right by 4p
                  for(uint8_t i = 4; i > 0; i--){
                      bus->vram_scroll_right();                  
                  }
        break;
        }    
        case 0xFC:{        
          //scroll left by 4p
                  for(uint8_t i = 4; i > 0; i--){
                      bus->vram_scroll_left();                  
                  }     
        break;
        }    
        case 0xE0:{
                  bus->vram_clear();
        break;
        }
        case 0xEE:{
                  Option<uint16_t> adr = stack.pop();
                  if(adr.je) pc = adr.val;                                     
        break;                         
        }
        case 0xFF:{        
          //enable hr     
        break;
        }
        case 0xFE:{        
          //disable hr     
        break;
        }                    
      }
    break;
   }
   case 0x01:{
              pc = nnn;    
   break; 
   }
   case 0x02:{
              stack.push(pc);
              pc = nnn;              
   break;
   }
   case 0x03:{
              if(vx_registers[x] == kk)
                 pc += 2;
   break;
   }
   case 0x04:{
              if(vx_registers[x] != kk)
                 pc += 2;
   break;
   }
   case 0x05:{
              if(vx_registers[x] == vx_registers[y])
                 pc += 2;             
   break;
   }
   case 0x06:{
              vx_registers[x] = kk;               
   break;
   }
   case 0x07:{
              vx_registers[x] += kk;            
   break;
   }
   case 0x08:{
            switch(n){
                case 0x00:{
                          vx_registers[x] = vx_registers[y];                  
                break;  
                }
                case 0x01:{
                          vx_registers[x] |= vx_registers[y];                  
                break;  
                }
                case 0x02:{
                          vx_registers[x] &= vx_registers[y];
                break;
                }
                case 0x03:{
                          vx_registers[x] ^= vx_registers[y];
                break;
                }
                case 0x04:{
                          uint16_t res = vx_registers[x] + vx_registers[y];
                          vx_registers[x] = res & 0xff;
                          if(res > 255) vx_registers[15] = 1;                                          
                break;
                }
                case 0x05:{
                          int8_t res = vx_registers[x] - vx_registers[y];
                          vx_registers[x] = res;
                          vx_registers[15] = (res < 0) ? 1 : 0;                   
                break;  
                }
                case 0x06:{
                          vx_registers[15] = vx_registers[x] & 0x01;
                          vx_registers[x] = vx_registers[x] >> 1;
                break;  
                }
                case 0x07:{
                          int8_t res = vx_registers[y] - vx_registers[x];
                          vx_registers[x] = res;
                          vx_registers[15] = (res < 0) ? 1 : 0;                        
                break;  
                }
                case 0x0E:{
                          vx_registers[15] = vx_registers[x] >> 7;
                          vx_registers[x] = vx_registers[x] << 1;                  
                break;  
                }              
            }
   break;         
   }
   case 0x09:{
             if(vx_registers[x] != vx_registers[y]) pc += 2;        
   break; 
   }
   case 0x0A:{
             i_register = nnn;    
   break; 
   }
   case 0x0B:{
             pc = vx_registers[0] + nnn;    
   break; 
   }
   case 0x0C:{
              vx_registers[x] = 111 & kk;
   }break;
   case 0x0D:{
             uint8_t x_cord = vx_registers[x];
             uint8_t y_cord = vx_registers[y];
             while(millis() < last_time_update_disp + 20);                                                     
             uint32_t last_time_update_disp = millis();  
          
             if(n == 0){
                display_sprite_big(x_cord, y_cord);
             }else{
                display_sprite(x_cord, y_cord, n); 
             }                                                                                         
   break; 
   }
   case 0x0E:{
             switch(kk){               
                case 0xA1:{
                          //Skip next instruction if key with the value of Vx is not pressed.;
                          uint8_t key_code = vx_registers[x];
                          uint8_t key = my_keyboard->pressed_key();
                          if(key_code == key) pc += 2;
                          my_keyboard->key_reset();                                                                                                                                                                                                                                         
                break;  
                }
                case 0x9E:{
                   //Skip next instruction if key with the value of Vx is pressed.;
                          uint8_t key_code = vx_registers[x];
                          uint8_t key = my_keyboard->pressed_key();
                          if(key_code != key) pc += 2;
                          my_keyboard->key_reset();                                                                                                                                                                                     
                break;  
                }
             }
   break;
   }
   case 0x0F:{
             switch(kk){
                case 0x07:{
                          vx_registers[x] = get_delay_timer();                                                      
                break;  
                }
                case 0x0A:{
                           //Wait for a key press, store the value of the key in Vx.All execution stops until a key is pressed, then the value of that key is stored in Vx.
                           uint8_t key;
                           do{
                              key = my_keyboard->pressed_key();
                           }while(key == '\0');
                           vx_registers[x] = key;                           
                break;  
                }              
                case 0x1E:{
                         i_register += vx_registers[x];
                break;  
                }
                case 0x15:{
                          set_delay_timer(vx_registers[x]);
                break;  
                }
                case 0x18:{
                         //fold
                break;  
                }
                case 0x29:{
                         i_register = vx_registers[x] * 5;
                break;  
                }
                case 0x30:{
                          i_register = vx_registers[x];                          
                break;  
                }
                case 0x33:{
                         uint8_t x_reg = vx_registers[x];
                         bus->ram_write_byte(i_register, x_reg / 100);
                         bus->ram_write_byte(i_register + 1, (x_reg % 100) / 10);
                         bus->ram_write_byte(i_register + 2, x_reg % 10);
                break;         
                }
                case 0x55:{
                          for(uint8_t i = 0; i < x + 1; i++){
                            bus->ram_write_byte(i_register + i, vx_registers[i]);
                          }
                break;  
                }
                case 0x65:{
                          for(uint8_t i = 0; i < x + 1; i++){
                            vx_registers[i] = bus->ram_read_byte(i_register + i);
                          }
                break;  
                }
                case 0x75:{
                          vx_registers[15] =  vx_registers[x];
                break;                                  
                }
                case 0x85:{
                          vx_registers[x] =  vx_registers[15];
                break;
                }     
             } 
   }
 }
 tick();
 my_keyboard->scan_keyboard();
}

void Cpu::display_sprite_big(uint8_t x, uint8_t y){
  uint8_t x_cord = x;
  uint8_t y_cord = y;  
  bool flip_flag = false;
  uint8_t left_byte, right_byte;

  for(uint8_t i = 0; i < 31; i += 2){
    left_byte = bus->ram_read_byte(i_register + i);
    right_byte = bus->ram_read_byte(i_register + i + 1);
    if(bus->vram_write_byte(y_cord, x_cord, left_byte) || bus->vram_write_byte(y_cord, x_cord + 8, right_byte)){
      flip_flag = true;
    }
    y_cord++;
  }
  vx_registers[15] = (flip_flag == true)? 1 : 0;  
  //gdram_dump();
  bus->vram_dump();
}

void Cpu::display_sprite(uint8_t x, uint8_t y, uint8_t h){
  uint8_t x_cord = x;
  uint8_t y_cord = y;
  uint8_t height = h;
  bool flip_flag = false;
  uint8_t sprite_byte;
  
  for(uint8_t i = 0; i < height; i++){
    sprite_byte = bus->ram_read_byte(i_register + i);
    if(bus->vram_write_byte(y_cord, x_cord, sprite_byte)){
      flip_flag = true;
    }
    y_cord++;
  }
  vx_registers[15] = (flip_flag == true)? 1 : 0;  
  bus->vram_dump();
  uint8_t* ptr = bus->vram_get();
}
