#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display1(-1);
Adafruit_SSD1306 display2(-1);
// this is the default address of the display(0x78) on back of display
#define OLED1 0x3C
// this is after switching over the jumper.
#define OLED2 0x3D
int i, j;

void setup()   {
  display1.begin(SSD1306_SWITCHCAPVCC, OLED1);
  display1.clearDisplay();
  display1.display();
  display2.begin(SSD1306_SWITCHCAPVCC, OLED2);
  display2.clearDisplay();
  display2.display();
}


void loop() {

// display 1
  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 0 );
  display1.println("Display 1");
  display1.setCursor(0, 8); 
  display1.print("Temperature 1: 30.2");
  display1.print((char)247);
  display1.println("C");
  display1.setCursor(0, 16);
  display1.println("Humidity 1: 50%");
  display1.setCursor(0, 24);
  display1.println("Flame 1: Not detect");
  display1.display();


// display 2
    display2.setTextSize(1);
    display2.setTextColor(WHITE);
    display2.setCursor(0, 0);
    display2.println("Display 2");
    display2.setCursor(0, 8);
    display2.println("Temperature 2");
    display2.setCursor(0, 16);
    display2.println("Humidity 2");
    display2.setCursor(0, 24);
    display2.println("Flame 2: No detect");
    display2.display();
}