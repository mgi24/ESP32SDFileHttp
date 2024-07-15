#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SD_MMC.h>

AsyncWebServer server(80);

// Inisialisasi pin untuk SD Card
#define SDMMC_CLK   39
#define SDMMC_CMD   38
#define SDMMC_D0    40 
#define SDMMC_D1    41 
#define SDMMC_D2    42 
#define SDMMC_D3    47

void setup() {
  Serial.begin(115200);
  
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0 , SDMMC_D1, SDMMC_D2, SDMMC_D3);//masukkan info pin
  
  //Wifi yang DIBUAT ESP32, BUKAN ESP32 conect ke wifi ini
  WiFi.softAP("ESP32 File Downloader");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Initializing SD card
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("SD card initialized");

  // Serve the file list at root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>SD Card Files</h1><ul>";
    File root = SD_MMC.open("/");
    if (root){
      while (File file = root.openNextFile()) {
        html += "<li><a href=\"/download?file=" + String(file.name()) + "\">" + String(file.name()) + "</a> "+ String(file.size()) +" Bytes</li>";
        file.close();
      }
      root.close();
    }
    
    html += "</ul></body></html>";
    request->send(200, "text/html", html);
  });

  // Serve file download
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("file")) {
      String filename = "/" + request->getParam("file")->value();
      if (SD_MMC.exists(filename)) {
        request->send(SD_MMC, filename, "application/octet-stream");
      } else {
        request->send(404, "text/plain", "File not found");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
}

void loop() {
  // Tambahkan kode yang Anda inginkan untuk loop
}
