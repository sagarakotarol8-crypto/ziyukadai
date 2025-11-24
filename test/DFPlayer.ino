#include<DFRobotDFPlayerMini.h>
#include<SoftwareSerial.h>

void setup(){
    Serial.begin(9600);

    
    SoftwareSerial DFPSerialSetting(10,11);
    DFPSerialSetting.begin(9600);

    DFRpbptDFPlayerMini DFP;
    DFP.begin(DFPSerialSetting);

    DFP.volume(25);
}

void loop(){
    if (){
        DFp.play(1);
    }
}
