#include "vram.h"
#include "ram.h"

class Bus{
  private:
      Ram *ram;
      Vram *vram;
          
  public:
      Bus(Ram* ram_adr, Vram* vram_adr);
      void ram_write_byte(uint16_t adr, uint8_t val);
      uint8_t ram_read_byte(uint16_t adr);
      bool vram_write_byte(uint8_t y, uint8_t x, uint8_t dat);
      void vram_clear();
      void load_rom(const uint8_t* rom, uint16_t n);
      void vram_scroll_down(uint8_t d);
      void vram_dump();
      void vram_scroll_left();
      void vram_scroll_right();
      uint8_t* vram_get();    
                        
};

uint8_t* Bus::vram_get(){
  uint8_t (*ptr)[16] = vram->getVram();
  return ((uint8_t*)ptr);
}


void Bus::vram_scroll_down(uint8_t d){
  vram->scroll_down(d);
}

void Bus::vram_scroll_left(){
  vram->scroll_left();
}

void Bus::vram_scroll_right(){
  vram->scroll_right();
}

void Bus::vram_dump(){
  vram->dump();
}

void Bus::load_rom(const uint8_t* rom, uint16_t n){
  for(uint16_t i = 0; i < n; i++){
    ram->write_byte(0x200 + i, rom[i]);
  }
}

void Bus::ram_write_byte(uint16_t adr, uint8_t val){
    ram->write_byte(adr, val);
}

uint8_t Bus::ram_read_byte(uint16_t adr){
    return ram->read_byte(adr);
}

bool Bus::vram_write_byte(uint8_t y, uint8_t x, uint8_t dat){
    return vram->write_byte(y, x, dat);
}

void Bus::vram_clear(){
  vram->vram_clear();
}

Bus::Bus(Ram* ram_adr, Vram* vram_adr){
  ram = ram_adr;
  vram = vram_adr;
}
