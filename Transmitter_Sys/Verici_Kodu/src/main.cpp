#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>  // BMP180 için kütüphane
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>  // SparkFun GPS kütüphanesi
#include <SoftwareSerial.h>
#include <LoRa_E22.h>

// PIN TANIMLARI
// MQ-4 Gaz Sensörü Pin Bağlantıları
const int MQ4_ANALOG_PIN = A0;    // Analog okuma pini
const int MQ4_DIGITAL_PIN = 8;    // Digital okuma pini (7'den 8'e değiştirildi)
const int LED_PIN = 13;           // Dahili LED

// LoRa E22 Pin Bağlantıları
const int LORA_M0_PIN = 7;        // Mode 0 kontrol pini
const int LORA_M1_PIN = 6;        // Mode 1 kontrol pini  
const int LORA_AUX_PIN = 2;       // Auxiliary pini

// Sensör objeleri
Adafruit_BMP085 bmp;
SFE_UBLOX_GNSS myGPS;
LoRa_E22 e22ttl(&Serial1, LORA_AUX_PIN, LORA_M0_PIN, LORA_M1_PIN);

// Kalibasyon değerleri
float Ro = 10000.0;               // Temiz havadaki sensör direnci
float RL = 10000.0;               // Yük direnci (10kΩ)

// Gaz türleri için kalibasyon sabitleri
struct GasData {
  String name;
  float a;      // Eğri sabiti
  float b;      // Eğim sabiti
  int minPPM;   // Minimum algılama seviyesi
  int maxPPM;   // Maksimum algılama seviyesi
};

GasData gases[] = {
  {"Metan (CH4)",     658.71,  -2.53,  200,   10000},
  {"Doğalgaz",        658.71,  -2.53,  200,   10000},
  {"LPG",             1011.2,  -2.75,  300,   10000},
  {"Alkol",           0.3934,  -1.504, 10,    500},
  {"Hidrojen (H2)",   987.99,  -2.162, 100,   10000},
  {"Smoke",           23.943,  -1.11,  10,    1000}
};

// LoRa ile gönderilecek veri yapısı
struct SensorData {
  // MQ-4 GAZ SENSÖRÜ VERİLERİ
  int mq4_analog;
  float mq4_voltage;
  float mq4_ratio;
  int mq4_digital;
  float gas_concentrations[6];  // 6 farklı gaz konsantrasyonu
  
  // BMP180 SENSÖR VERİLERİ
  float temperature;
  float pressure;
  float altitude;
  
  // GPS VERİLERİ
  double gps_latitude;
  double gps_longitude;
  float gps_altitude;
  int gps_satellites;
  
  // DURUM BİLGİLERİ
  bool gas_detected;
  unsigned long timestamp;
};

SensorData sensorData;

void readSensors() {
  // Zaman damgası
  sensorData.timestamp = millis();
  
  // MQ-4 GAZ SENSÖRÜ VERİLERİ
  sensorData.mq4_analog = analogRead(MQ4_ANALOG_PIN);
  sensorData.mq4_digital = digitalRead(MQ4_DIGITAL_PIN);
  sensorData.mq4_voltage = (sensorData.mq4_analog / 1023.0) * 5.0;
  
  // Sensör direncini hesapla
  float Rs = ((5.0 - sensorData.mq4_voltage) / sensorData.mq4_voltage) * RL;
  sensorData.mq4_ratio = Rs / Ro;
  
  // Gaz konsantrasyonlarını hesapla
  for(int i = 0; i < 6; i++) {
    if(sensorData.mq4_ratio > 0 && sensorData.mq4_ratio <= 10) {
      sensorData.gas_concentrations[i] = gases[i].a * pow(sensorData.mq4_ratio, gases[i].b);
      
      if(sensorData.gas_concentrations[i] < gases[i].minPPM) sensorData.gas_concentrations[i] = 0;
      if(sensorData.gas_concentrations[i] > gases[i].maxPPM) sensorData.gas_concentrations[i] = gases[i].maxPPM;
    } else {
      sensorData.gas_concentrations[i] = 0;
    }
  }
  
  // Gaz algılandı mı?
  sensorData.gas_detected = (sensorData.mq4_digital == LOW);
  
  // LED kontrolü
  digitalWrite(LED_PIN, sensorData.gas_detected ? HIGH : LOW);
  
  // BMP180 SENSÖR VERİLERİ
  sensorData.temperature = bmp.readTemperature();
  sensorData.pressure = bmp.readPressure() / 100.0;  // Pascal'dan hPa'ya çevir
  sensorData.altitude = bmp.readAltitude();
  
  // GPS VERİLERİ
  if (myGPS.getFixType() > 0) {
    sensorData.gps_latitude = myGPS.getLatitude() / 10000000.0;
    sensorData.gps_longitude = myGPS.getLongitude() / 10000000.0;
    sensorData.gps_altitude = myGPS.getAltitude() / 1000.0;
    sensorData.gps_satellites = myGPS.getSIV();
  } else {
    sensorData.gps_latitude = 0;
    sensorData.gps_longitude = 0;
    sensorData.gps_altitude = 0;
    sensorData.gps_satellites = 0;
  }
}

void printSensorData() {
  Serial.println("=============== SENSÖR VERİLERİ ===============");
  
  Serial.print("Zaman: ");
  Serial.print(sensorData.timestamp);
  Serial.println(" ms");
  
  Serial.println("\n--- MQ-4 GAZ SENSÖRÜ ---");
  Serial.print("Analog Değer: ");
  Serial.println(sensorData.mq4_analog);
  
  Serial.print("Voltaj: ");
  Serial.print(sensorData.mq4_voltage, 3);
  Serial.println(" V");
  
  Serial.print("Rs/Ro Oranı: ");
  Serial.println(sensorData.mq4_ratio, 4);
  
  Serial.print("Digital Durum: ");
  Serial.println(sensorData.mq4_digital == LOW ? "GAZ ALGILANDI!" : "Normal");
  
  Serial.println("\n--- GAZ KONSANTRASYONLARİ (PPM) ---");
  for(int i = 0; i < 6; i++) {
    Serial.print(gases[i].name);
    Serial.print(": ");
    if(sensorData.gas_concentrations[i] > 0) {
      Serial.print(sensorData.gas_concentrations[i], 2);
      Serial.println(" ppm");
    } else {
      Serial.println("Algılanmadı");
    }
  }
  
  Serial.println("\n--- BMP180 ÇEVRE SENSÖRÜ ---");
  Serial.print("Sıcaklık: ");
  Serial.print(sensorData.temperature, 2);
  Serial.println(" °C");
  
  Serial.print("Basınç: ");
  Serial.print(sensorData.pressure, 2);
  Serial.println(" hPa");
  
  Serial.print("İrtifa: ");
  Serial.print(sensorData.altitude, 2);
  Serial.println(" metre");
  
  Serial.println("\n--- GPS KONUM BİLGİLERİ ---");
  if (sensorData.gps_satellites > 0) {
    Serial.print("Enlem: ");
    Serial.print(sensorData.gps_latitude, 7);
    Serial.println("°");
    
    Serial.print("Boylam: ");
    Serial.print(sensorData.gps_longitude, 7);
    Serial.println("°");
    
    Serial.print("GPS Yükseklik: ");
    Serial.print(sensorData.gps_altitude, 2);
    Serial.println(" m");
    
    Serial.print("Uydu Sayısı: ");
    Serial.println(sensorData.gps_satellites);
  } else {
    Serial.println("GPS Fix alınamadı - Uydu sinyali bekleniyor...");
  }
  
  Serial.println("==========================================");
}

void sendDataViaLoRa() {
  Serial.println("\n>>> LoRa ile veri gönderiliyor...");
  
  ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(23, &sensorData, sizeof(SensorData));
  
  Serial.print("LoRa Gönderim Durumu: ");
  if (rs.code == 1) {
    Serial.println("✅ BAŞARILI");
    Serial.print("Gönderilen Veri Boyutu: ");
    Serial.print(sizeof(SensorData));
    Serial.println(" byte");
  } else {
    Serial.print("❌ HATA - Kod: ");
    Serial.println(rs.code);
  }
  Serial.println("<<< LoRa gönderimi tamamlandı\n");
}

void setup() {
  Serial.begin(9600);    // USB Serial
  Serial1.begin(9600);   // LoRa Serial
  Wire.begin();          // I2C başlat
  
  pinMode(MQ4_DIGITAL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  

  
  // BMP180 sensörünü başlat
  if (!bmp.begin()) {
    Serial.println("❌ BMP180 sensörü bulunamadı! Bağlantıları kontrol edin.");
    while (1);
  }
  Serial.println("✅ BMP180 sensörü başarıyla başlatıldı!");
  
  // GPS başlatma
  Serial3.begin(9600);
  if (myGPS.begin(Serial3) == false) {
    Serial.println("❌ u-blox GPS bulunamadı: Kontrol edin bağlantıları.");
  } else {
    Serial.println("✅ u-blox GPS başarıyla başlatıldı!");
  }
  
  // LoRa başlatma
  e22ttl.begin();
  Serial.println("✅ LoRa E22 modülü başlatıldı!");
  
  // Sensör ısınma süresi
  Serial.println("\n🔥 Sensör ısınıyor... (20 saniye)");
  for(int i = 20; i > 0; i--) {
    Serial.print(i);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println("\n✅ Sensör hazır!\n");
  
  Serial.print("Veri Paketi Boyutu: ");
  Serial.print(sizeof(SensorData));
  Serial.println(" byte");
  Serial.println("=== SİSTEM ÇALIŞIYOR ===\n");
}

void loop() {
  // Tüm sensörlerden veri oku
  readSensors();
  
  // Verileri Serial Monitor'e yazdır
  printSensorData();
  
  // LoRa ile gönder
  sendDataViaLoRa();
  
  // En yüksek gaz konsantrasyonu uyarısı
  float maxConcentration = 0;
  int maxIndex = -1;
  for(int i = 0; i < 6; i++) {
    if(sensorData.gas_concentrations[i] > maxConcentration) {
      maxConcentration = sensorData.gas_concentrations[i];
      maxIndex = i;
    }
  }
  
  if(sensorData.gas_detected && maxIndex >= 0) {
    Serial.println("\n🚨 KRITIK UYARI! GAZ ALGILANDI! 🚨");
    Serial.print("En yüksek konsantrasyon: ");
    Serial.print(gases[maxIndex].name);
    Serial.print(" = ");
    Serial.print(maxConcentration, 2);
    Serial.println(" ppm");
    Serial.println("🚨 ACİL DURUM PROTOKOLÜ AKTİF! 🚨\n");
  }
  
  Serial.println("⏳ 1 saniye bekleniyor...\n");
  delay(1000);  // 1 saniye bekle (5000'den 1000'e değiştirildi)
}