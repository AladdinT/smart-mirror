#ifndef CONF_H
#define CONF_H

//Allow unknown faces to navigate through the UI
#define UNKNOWN_CONTROL     ALLOWED

//Number of consecutive times of detecting unknown face/no face till block UI navigation
#define BLOCK_MARGIN        10

#define PROJECT_DIR     "/home/pi/myQt/mySmartMirror"

//static IP address of Esp32
#define ESP32_IP        "192.168.1.50"

//your openweather api key
#define OPENWEATHERMAP_API_KEY  "paste_your_key_here"

/************************************************************************************/
//Configurations Definitions
#define ALLOWED         1
#define NOT_ALLOWED     0

#endif // CONF_H
