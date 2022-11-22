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

float airBersihMax;
float airBersihMin;

float airCukupMax;
float airCukupMin;

float airBurukMax;
float airBurukMin;

float maxAirBuruk;
float maxAirCukup;
float maxAirBersih;
float minAirBuruk;
float minAirCukup;
float minAirBersih;

// defuzifikasi 
float z1;
float z2;
float z1temp;
float z2temp;
float m1,m2,m3;
float a1,a2,a3;


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

// infersi 
float get_max(){
  rule();
  float air_buruk[2]  = {Rule[2],Rule[7]};
  float air_cukup[6]  = {Rule[0],Rule[1],Rule[3],Rule[5],Rule[6],Rule[8]};
  float air_bersih[1] = {Rule[2]};
  maxAirBuruk   = air_buruk[0];
  maxAirCukup   = air_cukup[0];
  maxAirBersih  = Rule[2];
  for (int i = 0; i < 2; i++)
  {
    if (air_buruk[0] > maxAirBuruk)
    {
      maxAirBuruk = air_bersih[1];
    }
  }
  for (int i = 0; i < 6; i++)
  {
      maxAirCukup = air_cukup[1];
  }
  return maxAirBersih,maxAirBuruk,maxAirCukup;
}

// hitung batas area output
float get_area(){
  get_max();
  if (maxAirBuruk == 1)
  {
    z1 = maxAirBuruk * (19.5 - 18.5) + 18.5;
    z2 = 0;
  }else if(maxAirCukup == 1){
    z1 = maxAirCukup * (19.5 - 18.5) + 18.5;
    z2 = maxAirCukup * (37.5 - 36.5) + 36.5;
  }else if (maxAirBersih == 1){
    z1 = maxAirBersih * (37.5 - 36.5) + 36.5;
    z2 = 0;
  }else if(maxAirBuruk > 0 && maxAirCukup > 0 || maxAirBuruk > 0){
    z1temp = maxAirBuruk * (18.5 - 0) + 0;
    z2temp = maxAirCukup * (19.5 - 18.5) + 18.5;
    z1 = min(z1temp,z2temp);
    z2 = max(z2temp,z1temp);
  }else if(maxAirCukup > 0 && maxAirBersih > 0 || maxAirCukup > 0){
    z1temp = maxAirCukup *  (37.5 - 18.5) + 18.5;
    z2temp = maxAirBersih * (37.5 - 36.5) + 36.5;
    z1 = min(z1temp,z2temp);
    z2 = max(z2temp,z1temp);
    }
  return z1,z2;
}

float luas(){
  if (maxAirBuruk == 1 ){
    a1 = 0;
    a2 = 0;
    a3 = 0;
  }else if (maxAirCukup == 1)
  {
    a1 = z1 * maxAirCukup / 2;
    a2 = 0;
    a3 = 0;
  }else if (maxAirBersih ==1)
  {
    a1 = z1 * maxAirBersih / 2;
    a2 = 0;
    a3 = 0;
  }else if (maxAirBuruk > 0 && maxAirCukup > 0 || maxAirBuruk > 0)
  {
    float max_air1 = min(maxAirBuruk,maxAirCukup);
    float max_air2 = max(maxAirBuruk,maxAirCukup);
    a1 = (z1 - 18.5) * max_air1;
    a2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    a3 = (18.5 - z2 ) * max_air2;
  }else if (maxAirCukup > 0 && maxAirBersih > 0 || maxAirCukup > 0)
  {
    float max_air1 = min(maxAirCukup,maxAirBersih);
    float max_air2 = max(maxAirCukup,maxAirBersih);
    a1 = (z1 - 36.5) * max_air1;
    a2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    a3 = (37.5 - z2 ) * max_air2;
  }else if (maxAirBersih > 0)
  {
    float max_air1 = min(maxAirCukup,maxAirBersih);
    float max_air2 = max(maxAirCukup,maxAirBersih);
    a1 = (z1 - 36.5) * max_air1;
    a2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    a3 = (37.5 - z2 ) * max_air2;
  }
  return a1,a2,a3;
}

float momentum(){
  if (maxAirBuruk == 1 ){
    m1 = 0;
    m2 = 0;
    m3 = 0;
  }else if (maxAirCukup == 1)
  {
    m1 = pow(19.5,2)/2;
    m2 = 0;
    m3 = 0;
  }else if (maxAirBersih ==1)
  {
    m1 = pow(37.5,2)/2 - pow(36.5,2)/2;
    m2 = 0;
    m3 = 0;
  }else if (maxAirBuruk > 0 && maxAirCukup > 0 || maxAirBuruk > 0)
  {
    float max_air1 = min(maxAirBuruk,maxAirCukup);
    float max_air2 = max(maxAirBuruk,maxAirCukup);
    m1 = max_air1 * (0.5 * pow(z1,2) - 0.5 * pow(18.5,2));
    float m2_a = pow(z2,3)
    float m2_b = 
    m2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    m3 = (18.5 - z2 ) * max_air2;
  }else if (maxAirCukup > 0 && maxAirBersih > 0 || maxAirCukup > 0)
  {
    float max_air1 = min(maxAirCukup,maxAirBersih);
    float max_air2 = max(maxAirCukup,maxAirBersih);
    a1 = (z1 - 36.5) * max_air1;
    a2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    a3 = (37.5 - z2 ) * max_air2;
  }else if (maxAirBersih > 0)
  {
    float max_air1 = min(maxAirCukup,maxAirBersih);
    float max_air2 = max(maxAirCukup,maxAirBersih);
    a1 = (z1 - 36.5) * max_air1;
    a2 = ((max_air1 + max_air2) * (z2 - z1))/2;
    a3 = (37.5 - z2 ) * max_air2;
  }
  return a1,a2,a3;
}

float defuzzy(){
  get_area();
  luas();
  momentum();
  float defuzzfikasi = (m1+m2+m3)/(a1+a2+a3);
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