import processing.serial.*;         // ← Arduinoとの通信に必要
import processing.data.*;           // JSON/XML/CSVなどのデータ処理機能
import java.time.*;                 // 日時の取得・計算・変換に使う
import java.time.format.*;          // 日時を指定の形式に整える
import java.util.Locale;            // 言語設定（英語・日本語など）
import http.requests.*;

Serial serial;

String url = "https://api.open-meteo.com/v1/forecast?latitude=43.0757&longitude=141.3407&daily=weather_code,precipitation_probability_max&current=temperature_2m&timezone=Asia%2FTokyo";

// JSONデータ格納用変数
JSONObject jsondata;                    // 全体の天気データ

// --- 現在の情報 ---
float currentTemp;                  // 現在の気温（摂氏）
String todayWeatherCode;                    // 今日の天気コード
String todayWeather;              // 天気コードを英語の説明に変換した文字列（例: "Rain"）
String currentTime;                 // 現在の時刻（例: "2025-10-07 15:42:18"）
int todayChanceOfRain;
int count = 0;

int alarm = 0;
int alarmSoundControl = 0; // 0: 標準アラーム, 1: 連続音再生指示 2:連続音停止指示
String alarmTime = "08:27";
int editMode = 0;
int alarmHour = 8;
int alarmMinute = 27;
int alarmMode = 1;
int alarmState = 0;
int last_alarmState = 0;
int flag = 0;

// Arduinoから受け取るセンサー情報と制御フラグ
char button;
char last_button;
int switchState = 0; // 1: OFF (スマホなし), 0: ON (スマホあり)
int ldrState = 0; //0(明)or1(暗)

void setup(){
    size(800, 600);
    //シリアル通信の初期設定
    serial = new Serial(this, Serial.list()[2], 9600);
    println(Serial.list()[2]);
}

void draw(){

    //==毎フレーム処理する==
    //zoneDataライブラリからMM-dd HH:mm:ss形式で現在時刻を取得する
    ZonedDateTime jstTime = ZonedDateTime.now(ZoneId.of("Asia/Tokyo"));
    DateTimeFormatter fmt = DateTimeFormatter.ofPattern("MM-dd HH:mm:ss", Locale.ENGLISH); //フォーマットの設定オブジェクト
    currentTime = jstTime.format(fmt); //指定したフォーマットでjstTimeを書き換える。currentTimeに保存する

    //==一定時間ごとに処理する==
    //気候情報をurlから取得する
    //取得したjson構造のデータから、必要な数値を取得して変数に代入する
    if (count == 0){
        try {
            jsondata = loadJSONObject(url);
            if (jsondata == null) {
                text("Failed to load weather data.", 20, 40);
                return;
            }

            JSONObject current = jsondata.getJSONObject("current"); // 現在の情報
            float currentTempFloat = current.getFloat("temperature_2m");       // 現在の気温
            currentTemp = round(currentTempFloat * 10) / 10;

            JSONObject daily = jsondata.getJSONObject("daily");     // 1日ごとの情報
            JSONArray dailyWeatherCode = daily.getJSONArray("weather_code");   // 各日の天気コード
            todayWeatherCode = getWeatherDescription(dailyWeatherCode.getInt(0));        // 今日の天気コード
            
            JSONArray dailyChanceOfRain = daily.getJSONArray("precipitation_probability_max");
            todayChanceOfRain = dailyChanceOfRain.getInt(0);

        } catch (Exception e){ //エラー処理
            println("Failed to load weather data: " + e.getMessage());
            jsondata = null;                    // nullにして描画側で処理を中止させる
            return;
        }
    }
   
   //==イベントトリガーにより処理する==
   // 現在時刻とアラーム設定時刻の一致を監視する変数(alarmState)を設定する。その状態変化によってアラームを制御する指示(alarm, alarmSoundControl)を書く
  //alarmMode = 0 → 一定時間アラーム音を鳴らす。(arduinoへ渡す変数はalarm)
  //alarmMode = 2 → アラーム音を(switchState = 0) && (ldrState = 1) を満たすまで鳴らす(arduinoへ渡す変数はalarmSoundControl = 1)
  
   //現在時刻とアラーム設定時刻の一致を監視する変数(alarmState)を更新する。
   if(editMode == 0){
      String time = currentTime.substring(6, 11); //currentTimeから時刻部分のみ切り抜く
      if(time.equals(alarmTime)){ 
          alarmState = 1;
      }else{
          alarmState = 0;
      }
      
      //変数alarmStateが変化し、現在時刻がアラーム設定時刻と一致するなら一定時間アラームを鳴らす。
      if(alarmMode == 0){
          if ((last_alarmState != alarmState) && (alarmState == 1)) {//alarmStateが変化している、かつ現在がアラーム時刻である場合はアラームを鳴らす指示を書く
              println("指定した時刻です！");
              sendLineMessage("It's time!");
              alarm = 1;
          }else {//alarmStateが変化している場合、または時刻がアラーム設定時刻と一致しない場合は初期値に戻す
              alarm = 0;
          }
  
          last_alarmState = alarmState;
      }
      
      //変数alarmStateが変化し、現在時刻がアラーム設定時刻と一致するならアラームを鳴らす。
      //switchState や ldrState の値によってラインにメッセージを送る
      //アラーム音を鳴らしていて、(flag = 1) (switchState = 0) && (ldrState = 1) が満たされた場合にアラーム音を止める
      if(alarmMode == 1){
          if ((last_alarmState != alarmState) && (alarmState == 1)) {
              println("指定した時刻です！");
              sendLineMessage("It's time!");
              alarmSoundControl = 0; //とりあえずゼロに設定する
              if (switchState == 1) { // スイッチがオフ（スマホが置かれていない）の場合
                  // 1. LINEで通知する
                  sendLineMessage("Your phone is not in place!"); 
                  // 2. 連続音再生を指示する
                  alarmSoundControl = 1; 
              }
              if (ldrState == 0){ //部屋の明るさが閾値以上の場合 
                  // 1. LINEで通知する
                  sendLineMessage("The room lights are on!"); 
                  // 2. 連続音再生を指示する
                  alarmSoundControl = 1;
              }
              flag = 1;
          }else if(flag == 1){
              alarmSoundControl = 0;
              if ((switchState == 0) && (ldrState == 1)){
                  alarmSoundControl = 2;
                  flag = 0;
              }
          }else{
              alarmSoundControl = 0;
          }
  
          last_alarmState = alarmState;
      }
   }

    //==arduinoの要求によって処理する==
    //arduinoから送信されたデータがあれば、センサーの値を読み取り処理したあと、データを結合して送信する
    if(serial.available() > 0){ 
      
        String request = serial.readStringUntil('\n');// Arduinoから送られてくるリクエスト文字列（例: "S\n" or "N\n"）を受信する
        
        if (request != null) {
            request = request.trim();
            
            
            if (request.length() > 0) {// リクエスト文字列が空でないことを確認する
                String[] parts = split(request, ',');
                if (parts.length >= 3) {
                    button = parts[0].charAt(0); 
                    switchState = Integer.parseInt(parts[1].trim()); // スイッチ状態を取得
                    ldrState = Integer.parseInt(parts[2].trim());    // LDR値を取得
                }
                if(last_button != button){
                // --- ボタン処理 ---
                //Selectbutton editModeの変更
                //Upbutton, Downbutton editMode = 1 時刻の変更(時) editMode = 2 時刻の変更(分) editMode = 3 alarmModeの変更
                  switch(button) { 
                      case 'S':  // SELECT editModeの変更
                      editMode = (editMode + 1) % 4;
                      break; 
                      
                      case 'U':  // UP
                      if (editMode == 1) alarmHour = (alarmHour + 1) % 24;
                      else if (editMode == 2) alarmMinute = (alarmMinute + 1) % 60;
                      else if (editMode == 3) alarmMode = (alarmMode + 1) % 2;
                      break;
                      
                      case 'D':  // DOWN
                      if (editMode == 1) alarmHour = (alarmHour - 1 + 24) % 24;
                      else if (editMode == 2) alarmMinute = (alarmMinute - 1 + 60) % 60;
                      else if (editMode == 3) alarmMode = (alarmMode - 1 + 2) % 2;
                      break;
                      
                      // 'L', 'R', 'N' の処理はここでは特に記述しない
                  }
                }
                last_button = button;
                // editModeが有効ならアラーム設定時刻をcurrentTimeに上書きする（Arduinoに設定時刻を表示させるため）
                String serialsenddata;
                if (editMode > 0) { 
                    alarmTime = nf(alarmHour, 2) + ":" + nf(alarmMinute, 2);
                    currentTime = alarmTime + " EM" + editMode +" AM" + alarmMode; //currentTime "MM-dd HH:mm:ss"
                    serialsenddata = currentTime + "," + "," + "," + "," + alarm + "," + alarmSoundControl + "\n";
                } else {
                    // editMode == 0 の場合、currentTimeはdraw()の最初で取得したリアルタイム時刻のまま変更しない
                    serialsenddata = currentTime + "," + currentTemp + "," + todayWeatherCode + "," + todayChanceOfRain + "," + alarm + "," + alarmSoundControl + "\n";
                }
                
                // --- データ送信（レスポンス） ---
                // リクエストを受信したときだけ、データをArduinoに返す
                
                serial.write(serialsenddata); 
                println("Sent to Arduino: " + serialsenddata + " (on request: " + request + ")");
                    // 背景の描画 (毎フレームリフレッシュ)
                background(0); // 黒背景
                
                // 時刻の描画
                fill(255); // 文字色を白に設定
                textSize(20); // 文字サイズを48に設定
                // 画面中央上に時刻を描画
                textAlign(CENTER);
                text("Sent to Arduino: " + serialsenddata + " (on request: " + request + ")", width / 2, height / 4);
                text(alarmTime + " EM" + editMode +" AM" + alarmMode, width / 2, height * 3 / 4);
            }
        }
    }



    delay(500);

   
      
    
    
    if(count >= 3600){
        count = 0;
    } else {
        count++;
    }
    
    
}

String getWeatherDescription(int code) { //天気コードを種類を抑えた天気コードにマージする関数
  switch(code) {
    case 0:                                 return "1"; //"Clear sky";
    case 1: case 2: case 3:                 return "2"; //"Partly cloudy";
    case 45: case 48:                       return "2"; //"Fog";                    
    case 51: case 53: case 55:              return "3"; //"Drizzle";
    case 61: case 63: case 65:              return "5"; //"Rain";
    case 71: case 73: case 75:              return "6"; //"Snow";
    case 80: case 81: case 82:              return "5"; //"Showers";
    case 95:                                return "7"; //"Thunderstorm";
    case 96: case 99:                       return "8"; //"Thunderstorm (hail)";
    case 85: case 86:                       return "10";//"にわか雪"
    default:                                return "9";//"Unknown"; // 該当しないコード
  }
}

void sendLineMessage(String message) { //lineにメッセージ(英数字)を送る関数
    PostRequest post = new PostRequest("https://api.line.me/v2/bot/message/push");
    
    // ヘッダーを追加
    post.addHeader("Content-Type", "application/json");
    post.addHeader("Authorization", "Bearer dRXja72YLNNPGc2STErN2QlPVjakGIIz9cZcuu3htPFRKo0e58fqJoRRPpg9fIlaS4bZEiU2SKhFqq7S4P7i9M040gEaw0U+RmVNAR8Yv/5vqfbBVD8OfHh71gyVYPTZzdF+AOJqSAY4tnkKJ/CiOQdB04t89/1O/w1cDnyilFU=");
    
    // JSONデータを作成
    String json = "{"
        + "\"to\":\"U9b58894d0fd8b30b954b28d4ff8bfa0b\","
        + "\"messages\":[{\"type\":\"text\",\"text\":\"" + message + "\"}]"
        + "}";
    
    post.addData(json);  // または post.addJson(json); かも
    post.send();
    
    println("Response: " + post.getContent());
}
