#include "ControladorHardware.h"
#include "Configuracion.h"
#include <Arduino.h>
#include <cmath>
#include <ArduinoJson.h>

ControladorHardware::ControladorHardware(BrokerInterface* proveedorCom) 
    : canalComunicacion(proveedorCom), 
      estadoActual(ESTADO_APAGADO), 
      UMBRAL_POTENCIOMETRO(2048), 
      ultimoEnvioTermistor(0),
      INTERVALO_TERMISTOR(5000) {}

void ControladorHardware::inicializar() {
    pinMode(PIN_LED_ROJO, OUTPUT);
    pinMode(PIN_POTENCIOMETRO, INPUT);
    pinMode(PIN_TERMISTOR, INPUT);
    digitalWrite(PIN_LED_ROJO, LOW);
}

void ControladorHardware::procesarComandoEntrante(String topic, String mensaje) {
    Serial.printf("\n[MQTT IN] Ruta: '%s' | Mensaje: %s\n", topic.c_str(), mensaje.c_str());
    
    mensaje.trim();
    if (mensaje.equalsIgnoreCase("apagar") || mensaje.equalsIgnoreCase("encender")) {
        return;
    }

    JsonDocument doc;
    if (deserializeJson(doc, mensaje)) {
        Serial.println("   [ERROR] JSON inválido");
        return;
    }

    if (doc["comando"].is<const char*>()) {
        actualizarEstadoActuador(doc["comando"].as<const char*>());
    }
}

void ControladorHardware::actualizarEstadoActuador(const char* comando) {
    bool encender = (strcmp(comando, "ENCENDER") == 0);
    digitalWrite(PIN_LED_ROJO, encender ? HIGH : LOW);
    Serial.printf("   [HARDWARE] Actuador -> %s\n", encender ? "ENCENDIDO" : "APAGADO");
}

void ControladorHardware::ejecutarMuestreo() {
    unsigned long tiempoActual = millis();

    monitorearPotenciometro();

    if (tiempoActual - ultimoEnvioTermistor >= INTERVALO_TERMISTOR) {
        ultimoEnvioTermistor = tiempoActual;
        enviarTelemetriaJson();
    }
}

void ControladorHardware::monitorearPotenciometro() {
    int lecturaPot = analogRead(PIN_POTENCIOMETRO);

    if (lecturaPot >= UMBRAL_POTENCIOMETRO && estadoActual == ESTADO_APAGADO) {
        estadoActual = ESTADO_ENCENDIDO;
        canalComunicacion->publicar(TOPIC_PUBLICAR, "ENCENDER");
        Serial.printf("[EVENTO] Potenciómetro superó umbral (%d)\n", lecturaPot);
    } 
    else if (lecturaPot < UMBRAL_POTENCIOMETRO && estadoActual == ESTADO_ENCENDIDO) {
        estadoActual = ESTADO_APAGADO;
        canalComunicacion->publicar(TOPIC_PUBLICAR, "APAGAR");
        Serial.printf("[EVENTO] Potenciómetro bajó de umbral (%d)\n", lecturaPot);
    }
}

void ControladorHardware::enviarTelemetriaJson() {
    int lecturaNtc = analogRead(PIN_TERMISTOR);
    
    JsonDocument doc;
    doc["dispositivo"] = "ESP32_IoT";
    doc["ntc_termistor"]["lectura_raw"] = lecturaNtc;
    doc["ntc_termistor"]["temperatura_c"] = calcularTemperaturaC(lecturaNtc); 

    String payloadJson;
    serializeJson(doc, payloadJson);

    canalComunicacion->publicar(TOPIC_TEMPERATURA, payloadJson);
    Serial.printf("[MQTT OUT] Telemetría enviada -> %s\n", payloadJson.c_str());
}

float ControladorHardware::calcularTemperaturaC(int lecturaAnalogica) {
    if (lecturaAnalogica == 0) return 0.0;

    float R_NTC = 10000.0 * (4095.0 / (float)lecturaAnalogica - 1.0);
    float temperaturaK = 1.0 / (0.003354016 + (1.0 / 3950.0) * log(R_NTC / 10000.0));
    
    return temperaturaK - 273.15;
}
