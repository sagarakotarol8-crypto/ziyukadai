#include <LiquidCrystal.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// ピン定義
#define SWITCH_PIN 1
#define LDR_PIN A5
#define BUTTON_PIN A0

// LCD初期化
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// DFPlayer初期化
SoftwareSerial dfpSerial(2, 3);
DFRobotDFPlayerMini dfp;

byte testNum = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(F("Test Start"));
  
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  
  Serial.println(F("=Test Start="));
  delay(1000);
}

void loop() {
  testNum++;
  if (testNum > 6) testNum = 1;
  
  switch(testNum) {
    case 1: testLCD(); break;
    case 2: testButtons(); break;
    case 3: testSwitch(); break;
    case 4: testLDR(); break;
    case 5: testDFP(); break;
    case 6: testSerial(); break;
  }
  
  delay(3000);
}

// Test 1: LCD
void testLCD() {
  Serial.println(F("\n[1]LCD"));
  lcd.clear();
  lcd.print(F("Test1:LCD"));
  lcd.setCursor(0, 1);
  lcd.print(F("0123456789ABCDEF"));
  Serial.println(F("OK"));
}

// Test 2: Buttons
void testButtons() {
  Serial.println(F("\n[2]Button"));
  lcd.clear();
  lcd.print(F("Test2:Button"));
  
  for(int i = 0; i < 20; i++) {
    int val = analogRead(BUTTON_PIN);
    
    lcd.setCursor(0, 1);
    lcd.print(F("V:"));
    lcd.print(val);
    lcd.print(F("   "));
    
    if (val >= 700) lcd.print(F("SEL"));
    else if (val >= 500) lcd.print(F("LFT"));
    else if (val >= 300) lcd.print(F("DWN"));
    else if (val >= 50) lcd.print(F("UP "));
    else if (val < 50) lcd.print(F("RGT"));
    
    Serial.print(val);
    Serial.print(F(" "));
    delay(200);
  }
  Serial.println(F("\nOK"));
}

// Test 3: Switch
void testSwitch() {
  Serial.println(F("\n[3]Switch"));
  lcd.clear();
  lcd.print(F("Test3:Switch"));
  
  for(int i = 0; i < 10; i++) {
    int sw = digitalRead(SWITCH_PIN);
    
    lcd.setCursor(0, 1);
    if (sw) {
      lcd.print(F("OFF(HIGH)"));
      Serial.print(F("H "));
    } else {
      lcd.print(F("ON (LOW) "));
      Serial.print(F("L "));
    }
    delay(500);
  }
  Serial.println(F("\nOK"));
}

// Test 4: LDR
void testLDR() {
  Serial.println(F("\n[4]LDR"));
  lcd.clear();
  lcd.print(F("Test4:LDR"));
  
  for(int i = 0; i < 10; i++) {
    int val = analogRead(LDR_PIN);
    
    lcd.setCursor(0, 1);
    lcd.print(F("V:"));
    lcd.print(val);
    lcd.print(F(" "));
    
    if (val > 500) {
      lcd.print(F("BRT"));
      Serial.print(F("B"));
    } else {
      lcd.print(F("DRK"));
      Serial.print(F("D"));
    }
    
    Serial.print(val);
    Serial.print(F(" "));
    delay(500);
  }
  Serial.println(F("\nOK"));
}

// Test 5: DFPlayer
void testDFP() {
  Serial.println(F("\n[5]DFP"));
  lcd.clear();
  lcd.print(F("Test5:DFPlayer"));
  
  dfpSerial.begin(9600);
  
  if (!dfp.begin(dfpSerial)) {
    Serial.println(F("ERR"));
    lcd.setCursor(0, 1);
    lcd.print(F("ERROR!"));
    delay(2000);
    return;
  }
  
  lcd.setCursor(0, 1);
  lcd.print(F("Init OK"));
  dfp.volume(25);
  delay(500);
  
  lcd.setCursor(0, 1);
  lcd.print(F("Playing..."));
  dfp.play(1);
  delay(1500);
  dfp.stop();
  
  lcd.setCursor(0, 1);
  lcd.print(F("Stopped"));
  Serial.println(F("OK"));
}

// Test 6: Serial
void testSerial() {
  Serial.println(F("\n[6]Serial"));
  lcd.clear();
  lcd.print(F("Test6:Serial"));
  
  for(int i = 0; i < 5; i++) {
    Serial.println(F("N,0,1"));
    lcd.setCursor(0, 1);
    lcd.print(F("Sent:N,0,1"));
    delay(500);
  }
  Serial.println(F("OK"));
}
