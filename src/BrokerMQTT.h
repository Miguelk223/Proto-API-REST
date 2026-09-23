#ifndef BROKER_MQTT_H
#define BROKER_MQTT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "BrokerInterface.h"

class BrokerMQTT : public BrokerInterface {
public:
    using OnMessageCallback = BrokerInterface::OnMessageCallback;

    BrokerMQTT();
    void inicializar(OnMessageCallback callback) override;
    void asegurarConexion() override;
    bool publicar(String topic, String payload) override;
    void escuchar() override;

private:
    WiFiClientSecure espClient;
    PubSubClient client;
    OnMessageCallback appCallback;

    // Métodos estáticos necesarios para enlazar los eventos de la librería PubSubClient
    static BrokerMQTT* instanciaActual;
    static void mqqtCallbackGlobal(char* topic, byte* payload, unsigned int length);
};

#endif
