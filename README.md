This project make your ESP32 read files on sdcard /root and display it on webserver then you can click to download / delete / upload the file!
![image](https://github.com/user-attachments/assets/42596fc5-6725-4608-9d44-db059715d6fb)

![image](https://github.com/user-attachments/assets/a06c79d2-89e5-442c-89b0-9142c520cabc)


Please just use ASYNC Version, its up to 5Times faster!
ASYNC can download MULTIPLE files at once, but you need this 2 library:

https://github.com/me-no-dev/ESPAsyncWebServer

https://github.com/me-no-dev/AsyncTCP

you need to rename the file after download to extension of the original file. normal webserver already correct this but you CANNOT DOWNLOAD multiple files at once!

Created on esp32 board version 2.0.12, in case future version broke the code
made by MMV Project, free distribute, do not forget to add credit!
