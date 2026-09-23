#ifndef CONTROLADOR_HARDWARE_H
#define CONTROLADOR_HARDWARE_H

#include <Arduino.h>
#include "BrokerInterface.h"

enum EstadoSistema { ESTADO_APAGADO, ESTADO_ENCENDIDO };

class ControladorHardware {
private:
    BrokerInterface* canalComunicacion;
    EstadoSistema estadoActual;

    const int UMBRAL_POTENCIOMETRO;
    unsigned long ultimoEnvioTermistor;
    const unsigned long INTERVALO_TERMISTOR;

    void monitorearPotenciometro();
    void enviarTelemetriaJson();
    void actualizarEstadoActuador(const char* comando);

    float calcularTemperaturaC(int lecturaAnalogica);

public:
    ControladorHardware(BrokerInterface* proveedorCom);
    
    void inicializar();
    void procesarComandoEntrante(String topic, String mensaje);
    void ejecutarMuestreo();
};

#endif