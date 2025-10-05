// ALICI

#include "LoRa_E22.h"
#include <SoftwareSerial.h>

// LoRa bağlantısı
SoftwareSerial mySerial(10, 11); // Arduino RX <-- e22 TX, Arduino TX --> e22 RX
LoRa_E22 E22(&mySerial);

// Mod pinleri
#define M0 7
#define M1 6

// Ek TX için SoftwareSerial (TX = 8 kullanılacak, RX = 9 boş)
SoftwareSerial outSerial(9, 8);  // RX=9 (boşta), TX=8 (JSON çıkışı)

// --- Gaz isimleri ---
const char* gasNames[6] = {"CH4", "CO", "LPG", "H2", "NH3", "C3H8"};

// --- Veri yapısı ---
typedef struct {
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
  float gps_speed;
  int gps_satellites;
  int gps_fix_type;
  
  // DURUM BİLGİLERİ
  bool gas_detected;
  unsigned long timestamp;
} veriler;

void setup() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  Serial.begin(9600);      // USB Serial Monitor için
  outSerial.begin(9600);   // Ek çıkış (TX=8) JSON verileri için
  E22.begin();
  delay(500);
}

void loop() {
  while (E22.available() > 1) {
    // Gelen mesaj okunuyor
    ResponseStructContainer rsc = E22.receiveMessage(sizeof(veriler));
    veriler data = *(veriler*) rsc.data;

    // Debug için okunabilir çıktı
    printReadable(data, Serial);

    // JSON formatında TX=8 pinine gönder
    printJSON(data, outSerial);

    rsc.close();
  }
}

// --- Debug çıktısı (Serial Monitor) ---
void printReadable(veriler data, Stream &out) {
  out.println("Gelen Mesaj: ");
  
  out.print("ZAMAN: ");
  out.println(data.timestamp);

  out.print("MQ4 Analog: ");
  out.println(data.mq4_analog);

  out.print("MQ4 Voltaj: ");
  out.print(data.mq4_voltage, 2);
  out.println(" V");

  out.print("MQ4 Oran: ");
  out.println(data.mq4_ratio, 3);

  out.print("MQ4 Dijital: ");
  out.println(data.mq4_digital);

  out.print("Gaz Durumu: ");
  out.println(data.gas_detected ? "UYARI" : "Güvenli");

  out.println("--- Gaz Konsantrasyonları (ppm) ---");
  for (int i = 0; i < 6; i++) {
    out.print(gasNames[i]);
    out.print(": ");
    out.println(data.gas_concentrations[i], 1);
  }

  out.print("Sıcaklık: ");
  out.print(data.temperature, 2);
  out.println(" °C");

  out.print("Basınç: ");
  out.print(data.pressure, 2);
  out.println(" hPa");

  out.print("İrtifa: ");
  out.print(data.altitude, 2);
  out.println(" m");

  out.print("GPS: ");
  out.print(data.gps_latitude, 7);
  out.print(", ");
  out.println(data.gps_longitude, 7);

  out.print("GPS Yükseklik: ");
  out.print(data.gps_altitude, 2);
  out.println(" m");

  out.print("GPS Hız: ");
  out.print(data.gps_speed, 2);
  out.println(" km/h");

  out.print("Uydu Sayısı: ");
  out.println(data.gps_satellites);

  out.print("GPS Fix Tipi: ");
  out.println(data.gps_fix_type);

  out.println("-------------------------");
}

// --- JSON çıktısı (TX=8 pininden) ---
void printJSON(veriler data, Stream &out) {
  out.print("{");

  out.print("\"timestamp\":"); out.print(data.timestamp); out.print(",");

  out.print("\"mq4_analog\":"); out.print(data.mq4_analog); out.print(",");
  out.print("\"mq4_voltage\":"); out.print(data.mq4_voltage, 2); out.print(",");
  out.print("\"mq4_ratio\":"); out.print(data.mq4_ratio, 3); out.print(",");
  out.print("\"mq4_digital\":"); out.print(data.mq4_digital); out.print(",");

  out.print("\"gas_concentrations\":{");
  for (int i = 0; i < 6; i++) {
    out.print("\""); out.print(gasNames[i]); out.print("\":");
    out.print(data.gas_concentrations[i], 1);
    if (i < 5) out.print(",");
  }
  out.print("},");

  out.print("\"temperature\":"); out.print(data.temperature, 2); out.print(",");
  out.print("\"pressure\":"); out.print(data.pressure, 2); out.print(",");
  out.print("\"altitude\":"); out.print(data.altitude, 2); out.print(",");

  out.print("\"gps_latitude\":"); out.print(data.gps_latitude, 7); out.print(",");
  out.print("\"gps_longitude\":"); out.print(data.gps_longitude, 7); out.print(",");
  out.print("\"gps_altitude\":"); out.print(data.gps_altitude, 2); out.print(",");
  out.print("\"gps_speed\":"); out.print(data.gps_speed, 2); out.print(",");
  out.print("\"gps_satellites\":"); out.print(data.gps_satellites); out.print(",");
  out.print("\"gps_fix_type\":"); out.print(data.gps_fix_type); out.print(",");

  out.print("\"gas_detected\":"); out.print(data.gas_detected ? "true" : "false");

  out.println("}");
}
