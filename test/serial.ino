#include<LiquidCrystal.h>


void setup(){
    Serial.begin(9600);

    LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
    lcd.begin(16, 2);

    lcd.clear();
}

void loop(){
    if(Serial.available() > 0) {
        string serialdeta = Serial.readStringUntil('\n');
        serialdata.trim();

        int firstCommaPlace = serialdata.indexOf(',');
        int secondCommaPlace = serialdata.indexOf(',', firstCommaPlace + 1);
        int thirdCommaPlace = serialdata.indexOf(',', secondCommaPlace + 1);

        temp = serialdata.substring(0, firstCommaPlace);
        timeStr = serialdata.substring(firstCommaPlace + 1, secondCommaPlace);
        level = serialdata.substring(secondCommaPlace + 1, thirdCommaPlace);
        rainCha = serialdata.substring(thirdCommaPlace + 1);
    }

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    lcd.setCursor(0, 0);
    lcd.print(timeStr);
    lcd.print(" ");
    if (level == "1"){
      lcd.print("Clear sky");
    } else if (level == "2"){
      lcd.print("cloudy");
    } else if (level == "3"){
      lcd.print("Drizzle");
    } else if (level == "4"){
      lcd.print("Showers");
    } else if (level == "5"){
      lcd.print("Rain");
    } else if (level == "6"){
      lcd.print("Snow");
    } else if (level == "7"){
      lcd.print("Storm");
    } else if (level == "8"){
      lcd.print("Hailstorm");
    } else if (level == "9"){
      lcd.print("Unknown")
    }

    lcd.setCursor(0, 1);
    lcd.print(temp + "\xdf" + "C");
    lcd.print(" ");
    lcd.print(rainCha + "%");

    if(timeStr == "2025")

    delay(1000);
}
