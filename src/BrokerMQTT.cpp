#include "BrokerMQTT.h"
#include "Configuracion.h"

BrokerMQTT* BrokerMQTT::instanciaActual = nullptr;

BrokerMQTT::BrokerMQTT() : client(espClient), appCallback(nullptr) {
    instanciaActual = this;
}

void BrokerMQTT::mqqtCallbackGlobal(char* topic, byte* payload, unsigned int length) {
    if (instanciaActual && instanciaActual->appCallback) {
        String mensaje;
        mensaje.reserve(length);
        for (unsigned int i = 0; i < length; i++) {
            mensaje += (char)payload[i];
        }
        instanciaActual->appCallback(String(topic), mensaje);
    }
}

void BrokerMQTT::inicializar(OnMessageCallback callback) {
    appCallback = callback;
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqqtCallbackGlobal);
}

void BrokerMQTT::asegurarConexion() {
    while (!client.connected()) {
        Serial.println("\n[COMUNICACION] Conectando al Broker HiveMQ seguro...");
        
        espClient.setInsecure(); 
        
        char clientId[24];
        snprintf(clientId, sizeof(clientId), "ESP32-Wrover-%04X", (unsigned int)random(0xffff));
        
        if (client.connect(clientId, MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("   [COMUNICACION] Conexion MQTT Establecida con Exito!");
            client.subscribe(TOPIC_SUSCRIBIR);
            Serial.printf("   [COMUNICACION] Escuchando topico: %s\n", TOPIC_SUSCRIBIR);
        } else {
            Serial.printf("   [COMUNICACION] Error (codigo: %d). Reintentando en 5s...\n", client.state());
            delay(5000);
        }
    }
}

bool BrokerMQTT::publicar(String topic, String payload) {
    return client.publish(topic.c_str(), payload.c_str());
}

void BrokerMQTT::escuchar() {
    client.loop();
}
