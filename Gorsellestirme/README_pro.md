# 🚀 Profesyonel Sensör İzleme Sistemi v3.0

## 📋 Genel Bakış

Bu uygulama, Arduino/ESP32 tabanlı sensör sistemlerinden gelen JSON formatındaki verileri gerçek zamanlı olarak görselleştiren profesyonel bir izleme sistemidir. Harita entegrasyonu, gelişmiş grafik görselleştirme ve kullanıcı dostu arayüz ile donatılmıştır.

## ✨ Özellikler

### 🔌 Seri Port Bağlantısı
- Otomatik port algılama
- Çoklu baudrate desteği (9600-230400)
- Güvenli bağlantı yönetimi
- Gerçek zamanlı bağlantı durumu göstergesi

### 📊 Gelişmiş Veri Görselleştirme
- **Çevresel Sensörler**: Sıcaklık, basınç, yükseklik grafikleri
- **Gaz Sensörleri**: 6 farklı gaz konsantrasyonu izleme
  - Metan (CH4)
  - Hidrojen (H2) 
  - Alkol (C2H5OH)
  - Bütan (C4H10)
  - Karbonmonoksit (CO)
  - Amonyak (NH3)
- **GPS Verileri**: Hız, konum ve uydu sayısı takibi

### 🗺️ İnteraktif Harita Sistemi
- **Gerçek zamanlı konum izleme**
- **Otomatik merkezleme** özelliği
- **Rota görselleştirme** (son 100 konum)
- **Gaz algılama işaretçileri** (kırmızı: tehlike, yeşil: güvenli)
- **Çoklu harita katmanları** (OpenStreetMap, Terrain, Toner)
- **Hassasiyet göstergesi** (50m çember)
- **Tam ekran** desteği

### 📈 İstatistiksel Analiz
- Anlık değerler
- Minimum/maksimum değerler
- Ortalama hesaplama
- Geçmiş veri analizi (1000 veri noktası)

### 🎨 Modern Kullanıcı Arayüzü
- **Karanlık tema** tasarımı
- **Gradyan efektleri** ve animasyonlar
- **LCD göstergeler** kritik veriler için
- **Durum LED'leri** bağlantı durumu için
- **Tab sistemi** organize görünüm için

## 🔧 Kurulum

### Gereksinimler
```bash
Python >= 3.8
PyQt5 >= 5.15.10
pyserial >= 3.5
pyqtgraph >= 0.13.3
numpy >= 1.24.3
folium >= 0.14.0
PyQtWebEngine >= 5.15.6
```

### Otomatik Kurulum
Uygulama gerekli bağımlılıkları otomatik olarak kurar (PEP 723 desteği):

```bash
python Gorsellestirme_pro.py
```

### Manuel Kurulum
```bash
pip install PyQt5 pyserial pyqtgraph numpy folium PyQtWebEngine
```

## 📡 JSON Veri Formatı

Uygulama aşağıdaki JSON formatını beklemektedir:

```json
{
  "timestamp": 1697123456,
  "mq4_analog": 512,
  "mq4_voltage": 2.45,
  "mq4_ratio": 3.125,
  "mq4_digital": 1,
  "gas_concentrations": {
    "CH4": 45.2,
    "H2": 12.8,
    "C2H5OH": 89.1,
    "C4H10": 23.7,
    "CO": 15.3,
    "NH3": 8.9
  },
  "temperature": 24.5,
  "pressure": 101325.0,
  "altitude": 157.2,
  "gps_latitude": 39.9334,
  "gps_longitude": 32.8597,
  "gps_altitude": 850.0,
  "gps_speed": 65.2,
  "gps_satellites": 8,
  "gps_fix_type": 3,
  "gas_detected": false
}
```

## 🖥️ Kullanım

### 1. Uygulamayı Başlatma
```bash
python Gorsellestirme_pro.py
```

### 2. Seri Port Bağlantısı
1. **Port Seçimi**: Dropdown menüsünden uygun portu seçin
2. **Baudrate Ayarı**: Genellikle 115200 (varsayılan)
3. **Bağlan**: Yeşil "🔗 Bağlan" butonuna tıklayın
4. **Durum Kontrolü**: LED göstergesi yeşil olmalı

### 3. Veri İzleme
- **Grafikler Sekmesi**: Gerçek zamanlı sensör grafikleri
- **Harita Sekmesi**: GPS konum takibi ve rota görselleştirme

### 4. Harita Kullanımı
- **Otomatik Merkezleme**: GPS konumunu otomatik takip eder
- **Zoom Kontrolü**: Yakınlaştırma seviyesini ayarlayın
- **Katman Değiştirme**: Sağ üst köşedeki kontrol ile farklı haritalar
- **Tam Ekran**: Harita üzerindeki buton ile tam ekran modu

## 🔍 Özellik Detayları

### Gaz Algılama Sistemi
- **Normal Durum**: Yeşil gösterge, güvenli mesaj
- **Gaz Algılandı**: Kırmızı gösterge, uyarı mesajı
- **Haritada İşaretleme**: Gaz algılanan konumlar kırmızı pin ile işaretlenir

### GPS Sistemi
- **Fix Türleri**:
  - 0: GPS yok
  - 1: GPS fix yok
  - 2: 2D fix
  - 3: 3D fix (en iyi)
- **Minimum Gereksinimler**: En az 4 uydu için geçerli konum
- **Hassasiyet**: ±50 metre (haritada çember ile gösterilir)

### Veri Saklama
- **Geçmiş**: Son 1000 veri noktası bellekte tutulur
- **Grafik**: Son 200 veri noktası grafiklerde gösterilir
- **Harita**: Son 100 konum haritada gösterilir

## 🚨 Sorun Giderme

### Bağlantı Sorunları
1. **Port Bulunamıyor**: "🔄" butonuna basarak portları yenileyin
2. **Bağlantı Başarısız**: Baudrate'i kontrol edin, cihazı yeniden bağlayın
3. **Veri Gelmiyor**: JSON formatının doğru olduğundan emin olun

### Harita Sorunları
1. **Harita Yüklenmiyor**: İnternet bağlantınızı kontrol edin
2. **Konum Gösterilmiyor**: GPS fix durumunu kontrol edin
3. **Yavaş Çalışma**: Zoom seviyesini düşürün

### Performans Optimizasyonu
- **CPU Kullanımı**: Veri gelme sıklığını azaltın
- **Bellek Kullanımı**: Uygulamayı periyodik olarak yeniden başlatın
- **Grafik Performansı**: Gösterilen veri noktası sayısını azaltın

## 📊 İstatistikler

Uygulama aşağıdaki istatistikleri hesaplar:
- **Minimum/Maksimum değerler**
- **Ortalama değerler**
- **Mevcut değerler**
- **Veri paketi sayısı**

## 🔐 Güvenlik

- **Veri Doğrulama**: Gelen JSON verisi doğrulanır
- **Hata Yönetimi**: Geçersiz veriler güvenle atlanır
- **Bellek Yönetimi**: Otomatik veri temizleme
- **Thread Güvenliği**: Çoklu thread güvenli operasyonlar

## 🆕 Sürüm Geçmişi

### v3.0 - Profesyonel Sürüm
- ✅ JSON formatı desteği
- ✅ İnteraktif harita entegrasyonu
- ✅ Gelişmiş UI/UX tasarımı
- ✅ Çoklu gaz sensörü desteği
- ✅ GPS tracking sistemi
- ✅ İstatistiksel analiz

### v2.0 - Modern Sürüm
- ✅ Temel grafik sistemi
- ✅ Seri port yönetimi
- ✅ Karanlık tema

## 📞 Destek

Sorunlar ve öneriler için:
- GitHub Issues
- E-posta desteği
- Dokümantasyon

## 📄 Lisans

Bu proje MIT lisansı altında dağıtılmaktadır.

---

**🎯 Not**: Bu uygulama profesyonel endüstriyel kullanım için tasarlanmıştır. Güvenlik kritik uygulamalarda ek doğrulama gerekebilir.