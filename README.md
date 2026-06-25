# Akıllı Tarım Otomasyonu ve Gömülü Kontrol Sistemi (MSP430)

Bu proje, bir bitkinin çevresel ihtiyaçlarını (nem, sıcaklık, ışık) sürekli olarak analiz edip gerekli fiziksel müdahaleleri (sulama, yapay aydınlatma) otonom olarak gerçekleştiren donanım merkezli bir gömülü sistem projesidir. 

Projenin temel amacı, piyasadaki hazır sensör ve güç modülleri (breakout board) kullanım alışkanlıklarını bir kenara bırakarak; voltaj regülasyonundan güç anahtarlamaya kadar tüm elektriksel altyapının özel tasarlanmış **çift taraflı bir PCB** üzerinde komponent seviyesinde (component-level) birleştirilmesidir. Yazılım mimarisinde ise hiçbir soyutlama kütüphanesi (HAL veya standart kütüphane) kullanılmamış, doğrudan mikrodenetleyici register'larına müdahale edilmiştir.

---

## 🛠️ Donanım Mimarisi ve Çift Taraflı PCB Tasarımı

Sistemin en güçlü yönü, elektriksel bütünlüğü ve kararlılığı maksimize etmek amacıyla baştan sona özel olarak tasarlanan donanım altyapısıdır. Piyasada sıkça tercih edilen hazır modüllerden (Bluetooth ve OLED ekran hariç) tamamen kaçınılmış, sistem donanım seviyesinde sıfırdan kurgulanmıştır.

* **Çift Taraflı (Double-Sided) Özelleştirilmiş PCB:** Sinyal yolları ve yüksek akım çeken güç hatları, elektromanyetik girişimi (EMI) minimize edecek şekilde iki katmanlı olarak yönlendirilmiştir.
* **Tümleşik Voltaj Regülasyonu:** Harici bir güç modülü kullanılmaksızın, giriş gerilimini mikrodenetleyicinin ve sensörlerin ihtiyaç duyduğu kararlı 3.3V seviyesine düşüren lineer regülatör devresi doğrudan PCB üzerine gömülmüştür.
* **Komponent Seviyesinde Tasarım:** Hazır röle veya sensör kartları kullanılmamıştır. Su pompasını süren rölenin transistör tabanlı sürücü devresi (demeraj ve ters EMK korumaları dahil) ile sensör okuma hatlarının sinyal koşullandırma devreleri PCB üzerinde bağımsız komponentlerle dizayn edilmiştir.
* **Fiziksel Dayanım:** Ortam şartlarına ve elektromekanik titreşimlere karşı dirençli bir endüstriyel prototip elde edilmiştir.

---

## 💻 Yazılım Mimarisi (Bare-Metal)

Sistemin yazılımı, MSP430G2553 mikrodenetleyicisinin tüm kaynaklarını %100 kontrol edebilmek amacıyla tamamen **"Bare-Metal"** (kütüphanesiz) olarak C dilinde geliştirilmiştir.

* **Register-Level Programlama:** I2C, UART, ADC ve Timer donanımları hazır kütüphanelerle değil, doğrudan işlemci kaydedicileri (register) üzerinden bayt bayt yapılandırılmıştır.
* **Özel I2C ve OLED Sürücüsü:** SSD1306 OLED ekranı kontrol etmek için donanımsal I2C protokolü ve 5x8 ASCII font matrisi sıfırdan yazılarak sisteme entegre edilmiştir.
* **Donanımsal PWM Üretimi:** İşlemciyi döngüsel gecikmelerle (delay) yormamak adına, Timer_A modülü "Reset/Set" modunda (OUTMOD_7) çalıştırılarak pürüzsüz ve işlemciden bağımsız bir PWM sinyali elde edilmiştir.
* **Dinamik Sağlık Algoritması:** Sensörlerden alınan ham ADC verileri, lineer azalan ölçeklendirme matematiksel modeli ile işlenerek bitkinin anlık durumunu gösteren dinamik bir "Büyüme Kalitesi Puanına" dönüştürülür.
* **Histerezis Kontrolü:** Röle tetiklemelerinde sistemin kararsızlığa düşmesini (chatter effect) engellemek için yazılımsal ölü bölgeler (dead-zones) oluşturulmuştur.

---

## ⚙️ Temel Özellikler ve Otonom Fonksiyonlar

* **Akıllı Sulama:** Toprak nemi belirlenen kritik eşiğin altına düştüğünde su pompası devreye girer, ideal neme ulaşıldığında histerezis bandı sayesinde güvenli bir şekilde kapanır.
* **Ters Orantılı Aydınlatma:** Ortamdaki ışık (LDR) seviyesi düştükçe, PWM kontrollü yapay aydınlatma modülü otomatik olarak parlaklığını artırır.
* **Gerçek Zamanlı Telemetri:** Tüm sensör verileri, hesaplanan kalite puanı ve röle durumları Bluetooth (UART) üzerinden kablosuz olarak aktarılır.
* **Lokal Arayüz:** İşlenen veriler ve puan durumu, cihaz üzerindeki OLED ekranda anlık olarak görüntülenir.

---

## 🚀 Kullanılan Teknolojiler ve Bileşenler

* **Mikrodenetleyici:** Texas Instruments MSP430G2553
* **Sensör Ağı:** NTC (Sıcaklık), Analog Toprak Nemi Sensörü, LDR (Ortam Işığı)
* **Aktüatörler:** 5V Su Pompası (Transistör sürücülü röle ile kontrol edilir), LED Aydınlatma (Timer_A PWM destekli)
* **Haberleşme:** HC-05 / UART Bluetooth Modülü (Sadece modül olarak kullanılmıştır)
* **Görüntüleme:** 0.96" SSD1306 OLED (I2C) (Sadece modül olarak kullanılmıştır)
* **Geliştirme Ortamı:** Code Composer Studio (CCS)
* **PCB Tasarım:** KiCad

---

## 📂 Dosya ve Klasör Yapısı

Projenin donanım tasarımı, üretim çıktıları ve yazılım kaynak kodları modüler bir yapıda organize edilmiştir:

* **`C/`**: Sistemin beyni olan yazılımın bulunduğu dizindir. Herhangi bir hazır kütüphane kullanılmadan doğrudan register seviyesinde yazılmış Bare-Metal C kaynak kodlarını içerir.
* **`Docs/`**: Donanım tasarımının mutfağıdır. Devrenin **KiCad** üzerinde sıfırdan çizilmiş elektronik şematiklerini, çift taraflı PCB yerleşim (layout) planlarını ve lazer kazıma yöntemiyle üretime hazır hale getirilmiş ön/arka bakır (Cu) yüzey PDF çıktılarını barındırır.
* **`Images/`**: Teorik tasarımın fiziksel bir ürüne dönüşme serüvenini belgeler. Lazer kazıma makinesindeki üretim aşamalarından, montajı ve lehimlemesi bitmiş çift taraflı kartın detaylı fotoğraflarına kadar projenin donanımsal kalitesini gösteren görsel dökümanları içerir.

---

## 👨‍💻 Geliştirici

**Mehmet Burak Yılmaz** 
Kocaeli Üniversitesi - Elektronik ve Haberleşme Mühendisliği
