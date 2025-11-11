#include<LiquidCrystal.h>
#include<DFRobotDFPlayerMini.h>
#include<SoftwareSerial.h>
DFRobotDFPlayerMini DFP;
int count = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
String date;
String temp;
String timeStr;
String level;
String rainCha;
String alarm;
String soundControl;

#define SWITCH_PIN 11
#define LDR_PIN A5

# define LDR_THRESHOLD 800

int commaPlace[5];

void setup(){
    Serial.begin(9600);

    lcd.begin(16, 2);

    lcd.clear();

    SoftwareSerial DFPSerialSetting(2,3);
    DFPSerialSetting.begin(9600);

    
    DFP.begin(DFPSerialSetting);

    DFP.volume(25);
    DFP.play(1);
    delay(500);
    DFP.stop();

    pinMode(A0, INPUT);
    //センサーピンの初期化
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
}

void loop(){
  // --- 【フェーズ1：ボタン入力チェックとリクエスト送信】 ---
    char requestButton = 'N'; // 'N' = No Button (何も押されていない)
    int buttonValue = analogRead(A0);

    // ボタン処理とリクエストコードの決定
    if (buttonValue >= 700 && buttonValue < 1000) { // SELECT
        lcd.clear();
        lcd.print("SELECT");
        requestButton = 'S';
    }
    else if (buttonValue >= 500 && buttonValue < 700) { // LEFT
        lcd.clear();
        lcd.print("LEFT");
        requestButton = 'L';
    }
    else if (buttonValue >= 300 && buttonValue < 500) { // DOWN
        lcd.clear();
        lcd.print("DOWN");
        requestButton = 'D';
    }
    else if (buttonValue >= 50 && buttonValue < 300) { // UP
        lcd.clear();
        lcd.print("UP");
        requestButton = 'U';
    }
    else if (buttonValue < 50) { // RIGHT
        lcd.clear();
        lcd.print("RIGHT");
        requestButton = 'R';
    }

    // センサー値の読み取り
    int switchState = digitalRead(SWITCH_PIN);
    int ldrValue = analogRead(LDR_PIN);
    int ldrState = (ldrValue > LDR_THRESHOLD) ? 1 : 0; // スイッチの状態を 1 (ON) / 0 (OFF) に変換



    // リクエストをProcessingに送信 (ボタン情報、スイッチ状態、LDR値を含める)
    // Format: [ButtonCode],[SwitchState],[LDRState]\n
    Serial.print(requestButton);
    Serial.print(',');
    Serial.print(switchState); // スイッチ状態 (0 or 1)
    Serial.print(',');
    Serial.print(ldrState); //LDR状態 (0 or 1)
    Serial.print('\n');
    
    // 短い待ち時間（デバウンスと送信確保のため）
    delay(30);
    

    // --- 【フェーズ2：Processingからのデータ受信（レスポンス待ち）と表示】 ---
    if(Serial.available() > 0) {
        String serialdata = Serial.readStringUntil('\n');
        serialdata.trim();
        commaPlace[0] = serialdata.indexOf(',');
        for(int i = 1; i < 5; i++){
          commaPlace[i] = serialdata.indexOf(',', commaPlace[i - 1] + 1);
        }
        
        date = serialdata.substring(0, 5);
        timeStr = serialdata.substring(6, commaPlace[0]);
        temp = serialdata.substring(commaPlace[0] + 1, commaPlace[1]);
        level = serialdata.substring(commaPlace[1] + 1, commaPlace[2]);
        rainCha = serialdata.substring(commaPlace[2] + 1, commaPlace[3]);
        alarm = serialdata.substring(commaPlace[3] + 1, commaPlace[4]);
        soundControl = serialdata.substring(commaPlace[4] + 1);
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
      lcd.print("Unknown");
    } else if (level == "10"){
      lcd.print("Snow");
    }

    lcd.setCursor(0, 1);
    lcd.print(temp + "\xdf" + "C");
    lcd.print(" ");
    lcd.print(rainCha + "%");
    lcd.print(" ");
    lcd.print(date);

    if (alarm == "1"){ 
        // LCD表示を更新
        lcd.setCursor(0, 1); 
        lcd.print("                "); 
        lcd.setCursor(0, 1); 
        lcd.print("ALARM! / CHECK!"); 
        DFP.play(1); 
        delay(1000); 
        DFP.stop();
    }
    if (soundControl == "1") {
        lcd.setCursor(0, 1); 
        lcd.print("                "); 
        lcd.setCursor(0, 1); 
        lcd.print("ALARM! / CHECK!"); 
        DFP.play(1);    }
    if(soundControl == "2") {
        DFP.stop();
    }
    delay(500);
}
