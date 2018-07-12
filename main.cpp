#include "mbed.h"
#include "PololuLedStrip.h"

PololuLedStrip ledStrip(D6);

#define LED_COUNT 29
rgb_color colors[LED_COUNT];

#define INDEX0         0    // ??? ?? ???
#define INDEX1         7    // ??? ?? ???
#define INDEX2        15    // ??? ?? ???
#define INDEX3        22    // ??? ?? ??? 

Timer timer;

//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------

Serial pc(SERIAL_TX, SERIAL_RX);

AnalogIn button1(A0);
AnalogIn button2(A1);
AnalogIn button3(A2);
AnalogIn button4(A3);

DigitalOut led(LED1);

//                       ? ? ? ? ? ? ?
unsigned char Zero[7] = {0,1,1,1,1,1,1};
unsigned char One[7] = {0,1,0,0,0,0,1};
unsigned char Two[7] = {1,0,1,1,0,1,1};
unsigned char Three[7] = {1,1,1,0,0,1,1};
unsigned char Four[7] = {1,1,0,0,1,0,1};
unsigned char Five[7] = {1,1,1,0,1,1,0};
unsigned char Six[7] = {1,1,1,1,1,1,0};
unsigned char Seven[7] = {0,1,0,0,1,1,1};
unsigned char Eight[7] = {1,1,1,1,1,1,1};
unsigned char Nine[7] = {1,1,1,0,1,1,1};

rgb_color total[LED_COUNT] = { (rgb_color){0,0,0} };

unsigned char TeamA_Score = 0;
unsigned char TeamB_Score = 0;

bool flag = false;

unsigned char temp[4] = {0,0,0,0};

void LedStrip(void){
// Update the colors array.
    uint8_t time = timer.read_ms() >> 2;
  for(uint32_t i = 0; i < LED_COUNT; i++)
  {
            uint8_t x = time - 8*i;
      colors[i] = (rgb_color){ x, 255 - x, x };
  }
    
  // Send the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
  wait_ms(10); 
}

void LedReset(void){
// Update the colors array.
    //uint8_t time = timer.read_ms() >> 2;
  for(uint32_t i = 0; i < LED_COUNT; i++)
  {
            //uint8_t x = time - 8*i;
      colors[i] = (rgb_color){ 0, 0, 0};
  }
    
  // Send the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);
  wait_ms(10); 
}

int map(float x)
{
  int value = x * 10;
  
  if(value > 5)
    value = 1;
  else 
    value = 0;
  return value;
}

void TeamASet(void){
    if(temp[3] + !temp[2] > 1){           // A = 1, C = 0
        TeamA_Score++;
        wait_ms(100);
        return;
    } else if(temp[3] + !temp[2] == 1){   // A = 1, C = 1
        TeamA_Score = 0;
        wait_ms(100);
        return;
    } else if(temp[3] + !temp[2] < 1){    // A = 0, C = 1
        TeamA_Score--;
        wait_ms(100);
        return;
    }
}

void TeamBSet(void){
    if(temp[0] + !temp[1] > 1){           // B = 1, D = 0
        TeamB_Score++;
        wait_ms(100);
        return;
    } else if(temp[0] + !temp[1] == 1){   // B = 1, D = 1
        TeamB_Score = 0;
        wait_ms(100);
        return;
    } else if(temp[0] + !temp[1] < 1){    // B = 0, D = 1
        TeamB_Score--;
        wait_ms(100);
        return;
    }
}

void NumberDraw(int baseIndex, unsigned char number){
  unsigned char *numberArray;
  if(number > 100)
    pc.printf("?? ??!!\n");
  else if(number > 10 && (baseIndex == INDEX1 || baseIndex == INDEX3)){
      NumberDraw(baseIndex - 7, number / 10);
      number = number % 10;
  }
    
  switch(number){
    case 0:
      numberArray = Zero;
      break;
    case 1:
      numberArray = One;
      break;
    case 2:
      numberArray = Two;
      break;
    case 3:
      numberArray = Three;
      break;
    case 4:
      numberArray = Four;
      break;
    case 5:
      numberArray = Five;
      break;
    case 6:
      numberArray = Six;
      break;
    case 7:
      numberArray = Seven;
      break;
    case 8:
      numberArray = Eight;
      break;
    case 9:
      numberArray = Nine;
      break;
  }
  
  for(int i = 0; i < 7; i++){
    // Serial.print(numberArray[i]);
    if(numberArray[i] == 1)
      total[baseIndex + i] = (rgb_color){ 127, 127, 127 };
    else
      total[baseIndex + i] = (rgb_color){   0,   0,   0 }; 
  }
  //Serial.println();
  
  ledStrip.write(total, LED_COUNT);
}

// ? ????? LED 3? ? ??? ???? 7?
// 0  1  C 2  3
// 21 21 3 21 21 

void NumberSet(unsigned char team, unsigned char number){
  switch(team){
    case 0:
      NumberDraw(INDEX0,number);
      break;
      
    case 1:
      NumberDraw(INDEX1,number);
      break;

    case 2:
      NumberDraw(INDEX2,number);
      break;
      
    case 3:
      NumberDraw(INDEX3,number);
      break;
  }
}

void StripRefresh(){
    //for(int i = 0; i < LED_COUNT; i++){
//        if(total[i]){
//            
//        }    
//    }
}

int main()
{   
    pc.printf("Program Start!\r\n");

    while(1) {
        temp[0] = map(button1.read());
        
        temp[1] = map(button2.read());
        
        temp[2] = map(button3.read());
        
        temp[3] = map(button4.read());
        
        //pc.printf("%f",button4.read());
        
        if((temp[2] | temp[3]) && flag){            // ?? A? ?? C? ????,
            flag = !flag;                             // ???? ???? ?? flag ? ??
            TeamASet();                               // TeamA ??? ??
            pc.printf("Signal Received!\r\n");
            pc.printf("%d",TeamA_Score);
            pc.printf(" : ");
            pc.printf("%d",TeamB_Score);
            pc.printf("\n");
            NumberSet(1,TeamA_Score);
            //NumberCheck(TeamA_Score);
        } else if((temp[0] | temp[1]) && flag){     // ?? B? ?? D? ????,
            flag = !flag;                             // ???? ???? ?? flag ? ??
            TeamBSet();                               // TeamB ??? ??
            pc.printf("Signal Received!\r\n");
            pc.printf("%d",TeamA_Score);
            pc.printf(" : ");
            pc.printf("%d",TeamB_Score);
            pc.printf("\n");
            NumberSet(3,TeamB_Score);
            //NumberCheck(TeamB_Score);
        } else if(!flag)                            // flag ?? ?? ???? ???? ???
            flag = !flag;                             // ?? ?? ? ? ?? ?
        
        wait_ms(10);
    }
}