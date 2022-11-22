#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define ONE_WIRE_BUS 14

const int pinsalinitas = A0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27,16,2);

// float fuzifikasi
float data_suhu;
float data_awal;
float data_salinitas;
float salinitas;
float suhu_rendah; 
float suhu_sedang; 
float suhu_tinggi; 
float salinitas_tawar;
float salinitas_netral;
float salinitas_asin;
float suhu_dingin;
float suhu_normal;
float suhu_panas;
float lastSend = 0;
// float infersi
float minr[9];
float Rule[9];

// defuzifikasi 
float z1;
float z2;
float z1temp;
float z2temp;
float m1,m2,m3;
float a1,a2,a3;

// WiFi dan MQTT Broker
#define wifi_ssid "LAURENSIUS"
#define wifi_password "Alfa+Omega"
#define mqtt_server "test.mosquitto.org"
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastSampleTime = 0;


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i < length; i++) {
 Serial.print((char)payload[i]);
 }
 Serial.println();
 
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266Client")) {
 Serial.println("connected");
 client.subscribe("esp8266/temphum");
 } else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 // Wait 5 seconds before retrying
 delay(5000);
 }
 }
}

// fungsi membaca sensor suhu
float baca_sensor_suhu(){
  sensors.requestTemperatures(); 
  float nilai_suhu = sensors.getTempCByIndex(0);
  return nilai_suhu;
}

// fungsi membaca sensor salinitas
float baca_salinitas(){
  data_awal = analogRead(pinsalinitas);
  salinitas = (data_awal+61.62)/24.96;
  return salinitas;
}

// fuzyfikasi
unsigned char suhuRendah(){
  if (data_suhu <= 22)
  {
    suhu_rendah = 1; 
  }else if (data_suhu >= 22 && data_suhu <= 24)
  {
    suhu_rendah = (data_suhu - 22) / (24 - 22);
  }
  else if (data_suhu > 24){
    suhu_rendah = 0;
  } 
  return suhu_rendah;
}

unsigned char suhuSejuk(){
  if (data_suhu >= 22 && data_suhu <= 24)
  {
    suhu_sedang = (data_suhu -22 ) / (24 -22);
  }else if (data_suhu >= 24 && data_suhu <= 26)
  {
    suhu_sedang = 1;
  }
  else if (data_suhu >= 26 && data_suhu <= 30){
    suhu_sedang = (30 - data_suhu)/(30-26);
  } 
  return suhu_sedang;
}

unsigned char suhuDingin(){
  if (data_suhu >= 26 && data_suhu <= 30)
  {
    suhu_tinggi = (data_suhu - 26 )/ (30-26);
  }
  else if (data_suhu > 30){
    suhu_tinggi = 1;
  } 
  return suhu_tinggi;
}

unsigned char salinitasTawar(){
  if (baca_salinitas() <= 20)
  {
    salinitas_tawar = 1;
  }else if (baca_salinitas() >= 20 && baca_salinitas() <= 25)
  {
    salinitas_tawar = (baca_salinitas()-20)/(25-20);
  }else if (baca_salinitas( )> 25 )
  {
    salinitas_tawar = 0;
  }
  return salinitas_tawar;
}

unsigned char salinitasNetral(){
  if (baca_salinitas() >= 20 && baca_salinitas() <= 25)
  {
    salinitas_netral = (baca_salinitas()-20)/(25-20);
  }else if (baca_salinitas() >= 25 && baca_salinitas() <= 28)
  {
    salinitas_netral = 1;
  }else if (baca_salinitas() >= 28 && baca_salinitas() <= 30)
  {
    salinitas_netral = (30 - baca_salinitas())/(30-28);
  }
  return salinitas_netral;
}

unsigned char salinitasAsin(){
  if (baca_salinitas() < 25)
  {
    salinitas_asin = 0;
  }
  else if (baca_salinitas() >= 28 && baca_salinitas() <= 25)
  {
    salinitas_asin = (baca_salinitas()-28)/(30-28);
  }else if (baca_salinitas( )> 30 )
  {
    salinitas_asin = 1;
  }
  return salinitas_asin;
}

void fuzzyfikasi(){
  suhuRendah();
  suhuSejuk();
  suhuDingin();
  salinitasTawar();
  salinitasNetral();
  salinitasAsin();
}

//  fungsi min 
float Min(float a, float b){
  if (a < b) {
        return a;
    }
    else if (b < a) {
        return b;
    }
    else {
        return a;
    }
}

// komposisi aturan 
void rule(){
  fuzzyfikasi();
  minr[0] = Min(suhu_rendah,salinitas_tawar);   // cukup 
  minr[1] = Min(suhu_rendah,salinitas_netral); // cukup
  minr[2] = Min(suhu_rendah,salinitas_asin);  // buruk 
  minr[3] = Min(suhu_sedang,salinitas_tawar); // cukup 
  minr[4] = Min(suhu_sedang,salinitas_netral); // baik 
  minr[5] = Min(suhu_sedang,salinitas_asin); // cukup 
  minr[6] = Min(suhu_tinggi,salinitas_tawar); // cukup  
  minr[7] = Min(suhu_tinggi,salinitas_netral); // buruk 
  minr[8] = Min(suhu_tinggi,salinitas_asin); // cukup 
  Rule[0] = minr[0]; //cukup 
  Rule[1] = minr[1]; //cukup
  Rule[2] = minr[2]; //buruk
  Rule[3] = minr[3]; //cukup
  Rule[4] = minr[4]; //baik
  Rule[5] = minr[5]; //cukup
  Rule[6] = minr[6]; //cukup
  Rule[7] = minr[7]; //buruk
  Rule[8] = minr[8]; //cukup
}

void loop() {
  // data sensor suhu
  data_suhu=baca_sensor_suhu();
  data_salinitas=baca_salinitas();
  // tampilkan pada lcd
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu = ");
  lcd.print(data_suhu);
  Serial.println("Temperature is: ");
  Serial.println(data_suhu);
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Salt = ");
  lcd.print(data_salinitas);
  Serial.println("Salinitas is: ");
  Serial.println(data_salinitas);
  delay(2000);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}