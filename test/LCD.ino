#include<LiquidCrystal.h>
int x = 0;

void setup(){
    Serial.begin(9600);

    LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
    lcd.begin(16, 2);

    lcd.clear();
}

void loop(){
    x++;

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    lcd.setCursor(0, 0);
    lcd.print("Hello");
    lcd.print(x)
    delay(1000);
}
