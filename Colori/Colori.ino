#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> 
#include <Adafruit_ST7789.h> 
#include <stdint.h>

#define MAKE_RGB565(r, g, b) ((r << 11) | (g << 5) | (b))

//ST7735 PINS
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
// SDA 11 e SCK 13 IMPORTANT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//buttons pins
#define BUTTON_RED 7
#define BUTTON_GREEN 12
#define BUTTON_BLUE 4
#define BUTTON_YELLOW 3
#define BUTTON_BLACK 2
#define BUTTON_WHITE 1
#define BUTTON_PLUS 6
#define BUTTON_CANC 5

uint16_t col1, col2, result; //variables to store colors troughout the operations
bool newOp = false; //if true the operation go on
bool startOp = false; //if true you can start an operation

//a kind of associative array
char colors[] = {'R', 'G', 'B', 'Y', 'N', 'W'}; //color array
uint16_t colorsCode[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0x0000, 0XFFFF}; //RGB565 arrays

void setup() {

  //screen initialization (the screen is BGR)
  tft.initR(INITR_BLACKTAB);

  //shows a sort of starting screen
  tft.fillScreen(0x0000);
  tft.setCursor(10,50);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1.5);

  tft.println("COLOR CALCULATOR");

  //button initialization
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_PLUS, INPUT_PULLUP);
  pinMode(BUTTON_CANC, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_BLACK, INPUT_PULLUP);
  pinMode(BUTTON_WHITE, INPUT_PULLUP);

  delay(4000);

  //prints a message
  printInit("inserisci colore");

}

void loop() {

  //maybe a bit spaghetti but i have a limited amount of buttons
  if(digitalRead(BUTTON_RED) == LOW){
    handle('R');
  }else if(digitalRead(BUTTON_GREEN) == LOW){
    handle('G');
  }else if(digitalRead(BUTTON_PLUS) == LOW){
    handle('+');
  }else if(digitalRead(BUTTON_CANC) == LOW){
    handle('C');
  }else if(digitalRead(BUTTON_BLUE) == LOW){
    handle('B');
  }else if(digitalRead(BUTTON_YELLOW) == LOW){
    handle('Y');
  }else if(digitalRead(BUTTON_BLACK) == LOW){
    handle('N');
  }else if(digitalRead(BUTTON_WHITE) == LOW){
    handle('W');
  }

  delay(200);
}


void printInit(String b){
      tft.fillScreen(ST77XX_BLACK);

      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(1,0);
      tft.println(b);
}

//function to blend two rgb565 colors
uint16_t blend_rgb565(uint16_t a, uint16_t b)
{

    uint16_t A_r = a >> 11;
    uint16_t A_g = (a >> 5) & 0x3f;
    uint16_t A_b = a & 0x1f;
                                      //first convert in rgb and sum them
    uint16_t B_r = b >> 11;
    uint16_t B_g = (b >> 5) & 0x3f;
    uint16_t B_b = b & 0x1f;

    uint32_t C_r = (A_r + B_r)/2;
    uint32_t C_g = (A_g + B_g)/2;
    uint32_t C_b = (A_b + B_b)/2;
                                      //then constrain them in the possible values
    C_r = constrain(C_r, 0, 31);
    C_g = constrain(C_g, 0, 63);
    C_b = constrain(C_b, 0, 31);
                                      //lastly reconvert them in rgb565
    return MAKE_RGB565(C_r, C_g, C_b);

}

//handles the operation
void handle(char key){
  if(isInThere(key, colors)){
    //search for the color in the associative array
    for(int i=0; i<6; i++){
      //if it's the first color
      if(key == colors[i] && newOp == false){
        //we can continue with the operation now
        startOp=true;
        //save the color
        col1 = colorsCode[i];
        //fill the screen with this color
        tft.fillScreen(col1);
      //if it's the second color
      }else if(key == colors[i] && newOp == true){
        //save it
        col2 = colorsCode[i];
        //save the result in a different variable
        result = blend_rgb565(col1, col2);
        //fill the screen with the result
        tft.fillScreen(result);
        //we need a new operation now
        newOp=false;
        //use the result as the first color in a new operation
        col1=result;
        col2=0xFFFF;
      }
    }
  //if you pressed canc
  }else if(key == 'C'){
    //reset the colors
    col1=col2=0xFFFF;
    //reset everything
    printInit("inserisci colore");
    newOp=false;
    startOp=false;
    delay(1000);
  //if it's plus button and we can have an operation
  }else if(key == '+' && startOp == true){
    if(newOp == false){
      newOp = true;
    }
  }
}

bool isInThere(char needle, char haystack[]){
  bool result=false;

  for(int i=0; i<6; i++){
    if(needle == haystack[i]){
      result=true;
    }
  }
  return result;
}