//DOWNLOAD SD FILE FROM WEBSERVER BY MMV PROJECT
//DO NOT REMOVE CREDIT!!!!

#include <WiFi.h>
#include <WebServer.h>
#include <SD_MMC.h>

// Inisialisasi pin untuk SD Card
#define SDMMC_CLK   39
#define SDMMC_CMD   38
#define SDMMC_D0    40 
#define SDMMC_D1    41 
#define SDMMC_D2    42 
#define SDMMC_D3    47

WebServer server(80);//pilih port server, defaultnya ya 80
String lastupload="Belum ada file diupload";

void setup() {
    Serial.begin(115200);
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0 , SDMMC_D1, SDMMC_D2, SDMMC_D3);//masukkan info pin

    // Wifi mode AP sebagai contoh, nanti ada wifi namanya "ESP32 File" connect kesana
    WiFi.softAP("ESP 32 Downloader");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    //KLO MAU MODE STATION (esp konek ke wifi yang sudah ada) pake kode bawah ini
    //WiFi.begin("nama_wifi", "password_wifi");

    // Inisialisasi SD card menggunakan SDMMC
    if (!SD_MMC.begin()) {
        Serial.println("Gagal menginisialisasi SD card.");
        return;
    }

    // Route untuk halaman utama
    server.on("/", HTTP_GET, handleRoot);

    // Route untuk mengunduh file
    server.on("/download", HTTP_GET, handleDownload);
    server.on("/delete", HTTP_GET, handleDelete);
    //halaman upload
    server.on("/uploadfile", HTTP_GET, handleUpload);
    //handle upload
    server.on("/upload", HTTP_POST, [](){ server.send(200); }, handleFileUpload);
    server.onNotFound([](){ server.send(404, "text/plain", "Not found"); });
    // Memulai server
    server.begin();
    Serial.println("Web server dimulai.");
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    String html = "<html><body><h1>List File</h1>"
    "<form action=\"/uploadfile\" method=\"get\"><button type=\"submit\">Upload</button></form>";
    
    File root = SD_MMC.open("/");
    if (root) {
        while (File file = root.openNextFile()) {
            html += "<a href='/download?file=";
            html += file.name();
            html += "'>";
            html += file.name();
            html += "</a> ";
            html += file.size();
            html += " bytes ";
            html += "<a href='/delete?file=";
            html += file.name();
            html += "'>delete</a> <br>";
            file.close();
        }
        root.close();
    }
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleDownload() {
    String fileName = "/"+server.arg("file");
    File file = SD_MMC.open(fileName);
    if (file) {
        // Tambahkan header Content-Disposition
        server.sendHeader("Content-Disposition", "attachment; filename=" + String(file.name()));
        server.streamFile(file, "application/octet-stream");
        file.close();
    } else {
        server.send(404, "text/plain", "File tidak ditemukan.");
    }
}

void handleDelete() {
    String fileName = "/"+server.arg("file");
    if (SD_MMC.exists(fileName)) {
      SD_MMC.remove(fileName);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleUpload() {
  String html = "<h1>Upload to SD</h1>"
                "<p>File berhasil terakhir: "+lastupload+
                "</p><form method='POST' action='/upload' enctype='multipart/form-data'>"
                "<input type='file' name='upload'>"
                "<input type='submit' value='Upload'>"
                "</form><form action=\"/\" method=\"get\"><button type=\"submit\">Download</button></form>";
  server.send(200, "text/html", html);
}
void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    if (SD_MMC.exists(filename)) {
      SD_MMC.remove(filename);
    }
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (file) {
      Serial.print("start uploading ");
      Serial.println(filename);
      file.close();
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    String filename = "/" + upload.filename;
    File file = SD_MMC.open(filename, FILE_APPEND);
    if (file) {
      file.write(upload.buf, upload.currentSize);
      file.close();
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    String filename = "/" + upload.filename;
    File file = SD_MMC.open(filename, FILE_APPEND);
    if (file) {
      lastupload = filename;
      file.close();
      server.sendHeader("Location", "/uploadfile");
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}
