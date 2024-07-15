This project make your ESP32 read files on sdcard /root and display it on webserver then you can click to download the file!
![image](https://github.com/user-attachments/assets/c12d6e5e-a9bf-463c-b9cc-dc66c985ab3c)


ASYNC can download MULTIPLE files at once, but you need this 2 library:

https://github.com/me-no-dev/ESPAsyncWebServer

https://github.com/me-no-dev/ESPAsyncTCP

you need to rename the file after download to extension of the original file. normal webserver already correct this but you CANNOT DOWNLOAD multiple files at once!

Created on esp32 board version 2.0.12, in case future version broke the code
made by MMV Project, free distribute, do not forget to add credit!
