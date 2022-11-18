#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 14

const int pinsalinitas = A0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27,16,2);

float minr[9];
float Rule[9];

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


void setup() {
  // Start the Serial Monitor
  Serial.begin(9600);
  // inisialiasi lcd dan tampilkan pesan pada lcd 
  lcd.init();
  lcd.backlight();
  sensors.begin();
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
  }else if (baca_salinitas() >= 20 && baca_salinitas() <= 25)
  {
    salinitas_netral = 1;
  }else if (baca_salinitas() >= 28 && baca_salinitas() <= 30)
  {
    salinitas_netral = (30 - baca_salinitas())/(30-28);
  }
  return salinitas_netral;
}

unsigned char salinitasAsin(){
  if (baca_salinitas() >= 28 && baca_salinitas() <= 25)
  {
    salinitas_asin = (baca_salinitas()-28)/(30-28);
  }else if (baca_salinitas( )> 30 )
  {
    salinitas_asin = 0;
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

void get_max(){
  rule();
  float air_buruk[2]  = {Rule[2],Rule[7]};
  float air_cukup[6]  = {Rule[0],Rule[1],Rule[3],Rule[5],Rule[6],Rule[8]};
  float air_bersih[1] = {Rule[2]};
  float maxAirBuruk = air_buruk[2];

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
  }
