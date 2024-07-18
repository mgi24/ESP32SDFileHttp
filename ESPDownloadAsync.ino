//DOWNLOAD SD FILE FROM WEBSERVER BY MMV PROJECT
//DO NOT REMOVE CREDIT!!!!

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SD_MMC.h>

AsyncWebServer server(80);
//indikator file terakhir yang BERHASIL diupload
String lastupload = "Belum ada file yang diupload";

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
  WiFi.softAP("ESP32 File");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  //KLO MAU MODE STATION (esp konek ke wifi yang sudah ada) pake kode bawah ini
  //WiFi.begin("nama_wifi", "password_wifi");
  
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
    html += "<form action=\"/uploadfile\" method=\"get\"><button type=\"submit\">Upload</button></form>";
    File root = SD_MMC.open("/");
    if (root){
      while (File file = root.openNextFile()) {
        html += "<li><a href=\"/download?file=" + String(file.name()) + "\">" + String(file.name()) + "</a> "+ String(file.size()) +" Bytes"
                "<a href=\"/delete?file=" + String(file.name()) + "\"> delete</a></li>";
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
  // Server delete file
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("file")) {
      String filename = "/" + request->getParam("file")->value();
      if (SD_MMC.exists(filename)) {
        SD_MMC.remove(filename);
        request->redirect("/");
      } else {
        request->send(404, "text/plain", "File not found");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // Serve file upload
  server.on("/uploadfile", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>Upload to SD Card</h1><ul>";
    html += "<p>File upload terakhir: "+ lastupload + " </p>";
    html +="<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='upload'><input type='submit' value='Upload'></form>";
    html += "<form action=\"/\" method=\"get\"><button type=\"submit\">Download</button></form>";
    html += "</ul></body></html>";
    request->send(200, "text/html", html);
  });

  // Route untuk upload file
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200);
  }, handleFileUpload);

  server.begin();
}

void loop() {
  // Tambahkan kode yang Anda inginkan untuk loop
}

//handler file
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.printf("Upload file dimulai: %s\n", filename.c_str());
    if(SD_MMC.exists("/" + filename)){
      SD_MMC.remove("/" + filename);
    }
    request->_tempFile = SD_MMC.open("/" + filename, FILE_WRITE);
  }
  if(len){
    request->_tempFile.write(data, len);
  }
  if(final){
    request->_tempFile.close();
    lastupload = filename;
    Serial.printf("Upload file selesai: %s (%u bytes)\n", filename.c_str(), index + len);
    request->redirect("/uploadfile");
  }
}
