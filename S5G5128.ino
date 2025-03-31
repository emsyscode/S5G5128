/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/

#define VFD_in 8    // 
#define VFD_clk 9   // 
#define VFD_stb 10  // 
#define LED1 11
#define LED2 12

#define AdjustPins PIND  // before is C, but I'm use port C to VFC Controle signals

uint8_t signalStrobe = 0x00;  //I use the to control the process to write 3 bytes and change only then the strobe signal.
unsigned char digit = 0;
unsigned char grid = 0;
unsigned char gridSegments = 0x0C;  // Here I define the number of GRIDs and Segments I'm using Only have option of x10 or x11.

boolean flag = true;
boolean flagSecs = false;

uint8_t word0, word1, word2, word3;
unsigned int segOR[14];
//........................76543210....FEDCBA98....76543210....FEDCBA98..//letter.. //Id of position.
uint16_t segASCII[150]={0b00101000, 0b11111000,	0b11001000, 0b00000000, //A //0x00
                        0b00100010, 0b10110010,	0b10110000, 0b00000010, //B //0x04
                        0b00101000, 0b00001000,	0b00110000, 0b00000000, //C //0x08
                        0b00100010, 0b10100010,	0b10110000, 0b00000010, //D //0x0C
                        0b00101000, 0b01111000,	0b00110000, 0b00000000, //E //0x10
                        0b00101000, 0b01111000,	0b00000000, 0b00000000, //F //0x14
                        0b00101000, 0b00111000,	0b10110000, 0b00000000, //G //0x18
                        0b00001000, 0b11111000,	0b11001000, 0b00000000, //H //0x1C
                        0b00100010, 0b00100010,	0b00110000, 0b00000000, //I //0x20
                        0b00000000, 0b10001000,	0b10110000, 0b00000000, //J //0x24
                        0b01011001, 0b01101001,	0b01001000, 0b00000000, //K //0x28
                        0b00001000, 0b00001000,	0b00110000, 0b00000000, //L //0x2C
                        0b00001101, 0b10101000,	0b10000000, 0b00000000, //M //0x30
                        0b00001100, 0b10101001, 0b10000000, 0b00000000, //N //0x34
                        0b00101000, 0b10001000,	0b11111000, 0b00000000, //O //0x38
                        0b00101000, 0b11111000,	0b01000000, 0b00000000, //P //0x3C
                        0b00101000, 0b10001001,	0b11111000, 0b00000000, //Q //0x40
                        0b00101000, 0b11111001,	0b01001000, 0b00000000, //R //0x44
                        0b00101000, 0b01110000,	0b10110000, 0b00000000, //S //0x48
                        0b00100010, 0b00100010,	0b00000000, 0b00000000, //T //0x4C
                        0b00001000, 0b10001000,	0b11111000, 0b00000000, //U //0x50
                        0b00001001, 0b00101100,	0b01000000, 0b00000000, //V //0x54
                        0b00000101, 0b00100101,	0b00000000, 0b00000000, //X //0x58
                        0b01010101, 0b00100010,	0b00000000, 0b00000000, //Y //0x5C
                        0b00100001, 0b00100100,	0b00110000, 0b00000000, //Z //0x60
                        0b00001000, 0b10101101,	0b11001000, 0b00000000, //W //0x64
                        0b00101000, 0b10001000,	0b10110000, 0b00000000, //0 //0x68
                        0b00000000, 0b10000000,	0b10000000, 0b00000000, //1 //0x6C
                        0b00100000, 0b11111000,	0b00110000, 0b00000000, //2 //0x70
                        0b00100000, 0b11110000,	0b10110000, 0b00000000, //3 //0x74
                        0b00001000, 0b11110000,	0b10000000, 0b00000000, //4 //0x78
                        0b00101000, 0b01110000,	0b10110000, 0b00000000, //5 //0x7C
                        0b00101000, 0b01111000,	0b10110000, 0b00000000, //6 //0x80
                        0b00100000, 0b10000000,	0b10000000, 0b00000000, //7 //0x84
                        0b00101000, 0b11111000,	0b10110000, 0b00000000, //8 //0x88
                        0b00101000, 0b11110000,	0b10001000, 0b00000000, //9 //0x8C
                        0b00000000, 0b00000000,	0b00000000, 0b00000000};//Empty //0x90


void send_cmd(uint8_t a){
  uint8_t data = 0; 
  uint8_t mask = 1; //our bitmask
  data=a;
  digitalWrite(VFD_stb, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(2);
  // I define the size of mask to send only 8 bits
  // Is necessary the first signal of Clock happen when the strobe is already LOW!!!
  // Strobe need go to HIGH value only after 24 clock signals happen!!!
  for (mask = 0b10000000; mask>0; mask >>= 1) { //iterate through bit mask
    //Serial.println(mask, BIN);
    digitalWrite(VFD_clk, LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(2);
        if (data & mask){ // if bitwise AND resolves to true
          digitalWrite(VFD_in, HIGH);
          Serial.print(1);
        }
        else{ //if bitwise and resolves to false
          digitalWrite(VFD_in, LOW);
          Serial.print(0);
        }
    digitalWrite(VFD_clk, HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(4);
  }
  Serial.println("; ");
  signalStrobe++;  // I used this counter to determine when Strobe go HIGH, it happen only after 24 bits.
  if(signalStrobe == 3){
  digitalWrite(VFD_stb, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(2);
  digitalWrite(VFD_stb, HIGH);
  digitalWrite(VFD_clk, LOW);
  signalStrobe = 0;
  Serial.println("SignalStrobe =0 ");
  }
  
}
void setup() {
  // put your setup code here, to run once:
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_stb, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}
void blockClean(){
  send_cmd(0x1B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x19);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x09);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x18);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x08);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x17);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x07);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x16);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x06);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x15);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x05);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x14);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x04);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x13);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x03);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x12);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x02);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x11);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x01);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);

  // //************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void block0(){
  send_cmd(0x1B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);
  send_cmd(0x28);
  send_cmd(0x88);

  send_cmd(0x0A);
  send_cmd(0xB0);
  send_cmd(0x00);

  send_cmd(0x19);
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x09);
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x18);
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x08);
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x17);
  send_cmd(0x38);
  send_cmd(0x78);

  send_cmd(0x07);
  send_cmd(0x38);
  send_cmd(0x00);

  send_cmd(0x16);
  send_cmd(0x58);
  send_cmd(0xF8);

  send_cmd(0x06);
  send_cmd(0xC8);
  send_cmd(0x00);

  send_cmd(0x15);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x05);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x14);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x04);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x13);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x03);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x12);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x02);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x11);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x01);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);


  // //**************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void block1(){
  send_cmd(0x1B);  //First grid at right with pre-defined symbols Firts Part
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B); //First grid at right with pre-defined symbols Second Part
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);  //Address of first grid 14 segments at right part 1
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0A); //Address of first grid 14 segments at right part 2
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x19);  //This write the letter "O".
  send_cmd(0x28);
  send_cmd(0x88);

  send_cmd(0x09); //This write the letter "O".
  send_cmd(0xB0);
  send_cmd(0x00);

  send_cmd(0x18); // Letter "L" First
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x08); // Letter "L" First
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x17); // Letter "L" second
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x07); // Letter "L" second
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x16); //Letter "E"
  send_cmd(0x38);
  send_cmd(0x78);

  send_cmd(0x06); //Letter "E"
  send_cmd(0x38);
  send_cmd(0x00);

  send_cmd(0x15); //Letter "H"
  send_cmd(0x58);
  send_cmd(0xF8);

  send_cmd(0x05); //Letter "H"
  send_cmd(0xC8);
  send_cmd(0x00);

  send_cmd(0x14);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x04);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x13);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x03);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x12);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x02);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x11);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x01);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);

  //**************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void block2(){
  send_cmd(0x1B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x19);
  send_cmd(0x28);
  send_cmd(0x88);

  send_cmd(0x09);
  send_cmd(0xB0);
  send_cmd(0x00);

  send_cmd(0x18);
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x08);
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x17);
  send_cmd(0x48);
  send_cmd(0x08);

  send_cmd(0x07);
  send_cmd(0x78);
  send_cmd(0x00);

  send_cmd(0x16);
  send_cmd(0x38);
  send_cmd(0x78);

  send_cmd(0x06);
  send_cmd(0x38);
  send_cmd(0x00);

  send_cmd(0x15);
  send_cmd(0x58);
  send_cmd(0xF8);

  send_cmd(0x05);
  send_cmd(0xC8);
  send_cmd(0x00);

  send_cmd(0x14);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x04);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x13);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x03);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x12);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x02);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x11);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x01);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);

  //**************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void block3(){
      send_cmd(0x1B);
      send_cmd(0x00);
      send_cmd(0x00);
    //0
      send_cmd(0x0B);
      send_cmd(0x00);
      send_cmd(0x00);
    //1
      send_cmd(0x1A);
      send_cmd(0x00);
      send_cmd(0x00);
    //2
      send_cmd(0x0A);
      send_cmd(0x00);
      send_cmd(0x00);
    //3
      send_cmd(0x19);
      send_cmd(0x00);
      send_cmd(0x00);
    //4
      send_cmd(0x09);
      send_cmd(0x00);
      send_cmd(0x00);
    //5
      send_cmd(0x18);  //"O"
      send_cmd(0x28);
      send_cmd(0x88);
    //6
      send_cmd(0x08); //"O"
      send_cmd(0xB0);
      send_cmd(0x00);
    //7
      send_cmd(0x17); //"L"
      send_cmd(0x48);
      send_cmd(0x08);
    //8
      send_cmd(0x07); //"L"
      send_cmd(0x78);
      send_cmd(0x00);
    //9
      send_cmd(0x16); //"L"
      send_cmd(0x48);
      send_cmd(0x08);
    //10
      send_cmd(0x06); //"L"
      send_cmd(0x78);
      send_cmd(0x00);
    //11
      send_cmd(0x15); //"E"
      send_cmd(0x38);
      send_cmd(0x78);
    //12
      send_cmd(0x05); //"E"
      send_cmd(0x38);
      send_cmd(0x00);
    //13
      send_cmd(0x14); //"H"
      send_cmd(0x58);
      send_cmd(0xF8);
    //14
      send_cmd(0x04); //"H"
      send_cmd(0xC8);
      send_cmd(0x00);
    //15
      send_cmd(0x13);
      send_cmd(0x00);
      send_cmd(0x00);
    //16
      send_cmd(0x03);
      send_cmd(0x00);
      send_cmd(0x00);
    //17
      send_cmd(0x12);
      send_cmd(0x00);
      send_cmd(0x00);
    //18
      send_cmd(0x02);
      send_cmd(0x00);
      send_cmd(0x00);
    //19
      send_cmd(0x11);
      send_cmd(0x00);
      send_cmd(0x00);
    //20
      send_cmd(0x01);
      send_cmd(0x00);
      send_cmd(0x00);
    //21
      send_cmd(0x10);
      send_cmd(0x00);
      send_cmd(0x00);
    //22
      send_cmd(0x00);
      send_cmd(0x00);
      send_cmd(0x00);
    //23
      //**************
      send_cmd(0x80);
      send_cmd(0x70);
      send_cmd(0x00);
    //24
      send_cmd(0xC0);
      send_cmd(0x00);
      send_cmd(0x47);   
    //25
}
void msgBlock(){
  uint8_t letter = 0x00;
  send_cmd(0x1B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x19);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x09);
  send_cmd(0x00);
  send_cmd(0x00);

  letter = 0x48;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x18);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x08);
  send_cmd(word2);
  send_cmd(word3);
  letter = 0x28;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x17);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x07);
  send_cmd(word2);
  send_cmd(word3);

  letter = 0x2C;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x16);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x06);
  send_cmd(word2);
  send_cmd(word3);

  letter = 0x38;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x15);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x05);
  send_cmd(word2);
  send_cmd(word3);

   letter = 0x14;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x14);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x04);
  send_cmd(word2);
  send_cmd(word3);

  letter = 0x90;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x13);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x03);
  send_cmd(word2);
  send_cmd(word3);

  letter = 0x20;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x12);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x02);
  send_cmd(word2);
  send_cmd(word3);

  letter = 0x1C;
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(0x11);
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x01);
  send_cmd(word2);
  send_cmd(word3);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);

  //**************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void block5(){
  send_cmd(0x1B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0B);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x1A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x0A);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x19);
  word0 = segASCII[0]; word1 = segASCII[1]; word2 = segASCII[2]; word3 = segASCII[3];
  send_cmd(word0);
  send_cmd(word1);

  send_cmd(0x09);
  send_cmd(word2);
  send_cmd(word3);

  send_cmd(0x18);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x08);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x17);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x07);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x16);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x06);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x15);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x05);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x14);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x04);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x13);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x03);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x12);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x02);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x11);
  send_cmd(0b00000000);
  send_cmd(0b00000000);

  send_cmd(0x01);
  send_cmd(0b00000000);
  send_cmd(0b00000000);

  send_cmd(0x10);
  send_cmd(0x00);
  send_cmd(0x00);

  send_cmd(0x00);
  send_cmd(0x00);
  send_cmd(0x00);

  // //************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void verteceDots(){
  send_cmd(0x11);
  send_cmd(0b01010000);
  send_cmd(0b01010000);

  send_cmd(0x01);
  send_cmd(0b01111000);
  send_cmd(0b00000000);

   // //************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void gridRef(uint8_t grid, uint8_t letter){
  send_cmd(grid);  //This is the ID of grid to be write
  word0 = segASCII[letter]; word1 = segASCII[letter+1]; word2 = segASCII[letter+2]; word3 = segASCII[letter+3];
  send_cmd(word0);  //set of segments
  send_cmd(word1);  //set of segments
  grid = (grid & 0x0F);
  send_cmd(grid);   //Complement of memory position.
  send_cmd(word2);  //set of segments
  send_cmd(word3);  //set of segments

  // //************
  send_cmd(0x80);
  send_cmd(0x70);
  send_cmd(0x00);

  send_cmd(0xC0);
  send_cmd(0x00);
  send_cmd(0x47);
}
void loop() {
  //Note I keep the commands allways as format binary to be possible change a bit and see the result
  //offcourse a array or a matrix array of bytes will turn the length of code very short and allow to 
  //use the function ADD or OR
  //Serial.println("Start: 0");
  signalStrobe = 0;
  blockClean();
  delay(500);
  verteceDots();
  delay(500);
  msgBlock();
  delay(1000);
  block0();  //HELLO at right.
  delay(500); 
  block1(); //HELLO moved
  delay(500);
  block2();
  delay(500);
  block3();
  delay(500); 
  blockClean();
  //The next for run alphabet on display
  for(uint8_t x = 0x00; x < 0x94; x=x+4){
    gridRef(0x1A, x);
    delay(300);
  digitalWrite(LED1, !digitalRead(LED1));
  digitalWrite(LED2, !digitalRead(LED2));
   }
  //Serial.println("Finish: ");
}
