

#define ROWS 64
#define COLS 16

class Vram{
private:
    uint8_t zero_row[COLS] = { 0x00 };
    uint8_t vram[ROWS][COLS] = { 0x00 };
public:
    void vram_clear();
    bool write_byte(uint8_t y, uint8_t x, uint8_t data1);
    void dump();
    void scroll_down(uint8_t d);
    void scroll_left();
    void scroll_right();
    uint8_t (*getVram())[16]{ return vram; }

};

void Vram::dump(){
  for(auto &row : vram){
    for(uint8_t data1 : row){
      uint8_t hodnota = data1;
      for(uint8_t i = 0; i < 8; i++){
        uint8_t _bit = ((hodnota << i) & 0x80) >> 7;
        switch(_bit){
          case 1: {
            Serial.print("*");
            break;
          }
          case 0: {
            Serial.print(" ");
          }break;
        }
      }      
    }
    Serial.println();
  }
  Serial.println("konec");
}

bool Vram::write_byte(uint8_t y, uint8_t x, uint8_t data1){
  uint8_t index, row, posuv, _bit, prev_bit, cur_bit;
  index = x;
  row = y;
  bool flip_flag = false;
  //Serial.println(data1);
  for(uint8_t i = 0; i < 8; i++){
    _bit = ((data1 << i) & 0x80) >> 0x07;
    posuv = 7 - (index & 0x07);
    prev_bit = (vram[row][index / 8] & (1 << posuv)) >> posuv;
    vram[row][index >> 3] ^= _bit << posuv;
  
    //Set_bit(_bit);
    //SetPixel(index, row);
    cur_bit = (vram[row][index / 8] & (1 << posuv)) >> posuv;

    if(!flip_flag && prev_bit == 1 && cur_bit == 0){
        flip_flag = true;
    }
    index += 1; 
  }
  
  return flip_flag;
}

void Vram::vram_clear(){
  memset(vram, 0x00, sizeof(vram));
  
}

void Vram::scroll_down(uint8_t d){
  while(d-- > 0){
    for(uint8_t i = ROWS - 1; i > 0; i--){
      for(uint8_t j = 0; j < COLS; j++){
        vram[i][j] = vram[i - 1][j];
      }
    }
    for(uint8_t i = 0; i < COLS; i++){
       vram[0][i] = zero_row[i];
    }
  }
}
void Vram::scroll_left(){
  uint8_t prev_carry, carry;
  prev_carry = carry = 0;

  for(auto &item : vram){
    for(uint8_t i = COLS - 1; i >= 0; i--){
       carry = (item[i] & 0x80) >> 0x07;
       item[i] = item[i] << 1;
       if(prev_carry) item[i] |= 0x1;

       prev_carry = carry;
    }
    prev_carry = carry = 0;
  }  
}

void Vram::scroll_right(){
  uint8_t prev_carry, carry;
  prev_carry = carry = 0;

  for(auto &item : vram){
    for(uint8_t i = 0; i < COLS; i++){
       carry = item[i] & 0x01;
       item[i] = item[i] >> 1;
       if(prev_carry) item[i] |= 0x80;

       prev_carry = carry;
    }
    prev_carry = carry = 0;
  }      
}
