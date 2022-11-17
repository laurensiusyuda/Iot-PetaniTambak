#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 14

const int pinsalinitas = A0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27,16,2);

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
unsigned char suhuSejuk(){
  if (data_suhu >= 26 && data_suhu <= 30)
  {
    suhu_tinggi = (data_suhu - 26 )/ (30-26);
  }
  else if (data_suhu > 30){
    suhu_tinggi = 1;
  } 
  return suhu_tinggi;
}
unsigned char salinitastawar(){
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
}
unsigned char salinitasAsin(){
  if (baca_salinitas() >= 28 && baca_salinitas() <= 25)
  {
    salinitas_asin = (baca_salinitas()-28)/(30-28);
  }else if (baca_salinitas( )> 30 )
  {
    salinitas_asin = 0;
  }
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
