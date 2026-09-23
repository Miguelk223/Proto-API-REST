#include <WiFi.h>
#include <time.h>
#include <Preferences.h>
#include "Configuracion.h"
#include "BrokerMQTT.h"
#include "ControladorHardware.h"

BrokerMQTT canalRed;
ControladorHardware chip(&canalRed);
Preferences memoriaPermanente;
bool sistemaActivo = true; 

void alRecibirMensajeServidor(String topic, String payload) {
    payload.trim();

    if (payload.equalsIgnoreCase("apagar")) {
        if (sistemaActivo) {
            Serial.println("\n[SISTEMA] Comando 'apagar' recibido. Deteniendo el envío de telemetría.");
            sistemaActivo = false;
            memoriaPermanente.begin("config", false);
            memoriaPermanente.putBool("activo", false);
            memoriaPermanente.end();
        }
        return; 
    }
    
    if (payload.equalsIgnoreCase("encender")) {
        if (!sistemaActivo) {
            Serial.println("\n[SISTEMA] Comando 'encender' recibido. Reactivando el envío de telemetría.");
            sistemaActivo = true;
            memoriaPermanente.begin("config", false);
            memoriaPermanente.putBool("activo", true);
            memoriaPermanente.end();
        }
        return;
    }

    if (sistemaActivo) {
        chip.procesarComandoEntrante(topic, payload);
    }
}

void conectarWiFi() {
    Serial.printf("\n [SISTEMA] Conectando a: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[SISTEMA] ¡Conexión WiFi Exitosa!");

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("[SISTEMA] Sincronizando hora TLS...");
    
    time_t ahora = time(nullptr);
    while (ahora < 24 * 3600) { 
        delay(500);
        Serial.print(".");
        ahora = time(nullptr);
    }
    Serial.printf("\n[SISTEMA] Reloj sincronizado correctamente.\n[SISTEMA] IP: %s\n", WiFi.localIP().toString().c_str());
}

void setup() {
    Serial.begin(115200);
    
    memoriaPermanente.begin("config", true); 
    sistemaActivo = memoriaPermanente.getBool("activo", true); 
    memoriaPermanente.end();
    
    Serial.printf("\n[SISTEMA] Estado inicial: %s\n", sistemaActivo ? "ACTIVO. Se enviará telemetría." : "APAGADO. Telemetría en pausa permanente.");

    conectarWiFi();
    canalRed.inicializar(alRecibirMensajeServidor);
    chip.inicializar();
}

void loop() {
    canalRed.asegurarConexion(); 
    canalRed.escuchar();          
    
    if (sistemaActivo) {
        chip.ejecutarMuestreo();      
    } else {
        delay(490);
    }
    
    delay(10); 
}