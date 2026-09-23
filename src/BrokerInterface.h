#ifndef BROKER_INTERFACE_H
#define BROKER_INTERFACE_H

#include <Arduino.h>

class BrokerInterface {
public:
    // Puntero de función para avisar cuando llegue un mensaje de internet
    using OnMessageCallback = void (*)(String topic, String payload);

    virtual void inicializar(OnMessageCallback callback) = 0;
    virtual void asegurarConexion() = 0;
    virtual bool publicar(String topic, String payload) = 0;
    virtual void escuchar() = 0;
    virtual ~BrokerInterface() = default;
};

#endif
