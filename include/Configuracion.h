#ifndef CONFIGURACION_H
#define CONFIGURACION_H

// Credenciales de Internet y Servidor (Modifica con tus datos)

inline const char* WIFI_SSID     = "PalmasInnWiFi";
inline const char* WIFI_PASSWORD = "palmasinn";

inline const char* MQTT_SERVER   = "81787af48ba24db4aeb07062d144e378.s1.eu.hivemq.cloud";
inline const int   MQTT_PORT     = 8883; // Puerto SSL obligatorio para HiveMQ Cloud
inline const char* MQTT_USER     = "atreus_25";
inline const char* MQTT_PASSWORD = "Argos25$.";

// Canales de Comunicación / Tópicos (MQTT)

inline const char* TOPIC_PUBLICAR    = "esp32/potenciometro/estado";
inline const char* TOPIC_TEMPERATURA = "esp32/termistor/temperatura";
inline const char* TOPIC_SUSCRIBIR   = "esp32/potenciometro/comandos";

// Asignación de Pines Eléctricos (Hardware Físico)

inline const int PIN_POTENCIOMETRO = 34; // Entrada Analógica (Potenciómetro)
inline const int PIN_TERMISTOR     = 35; // Entrada Analógica (Termistor NTC 10K)
inline const int PIN_LED_ROJO      = 12; // Salida Digital (LED Rojo Externo)

#endif