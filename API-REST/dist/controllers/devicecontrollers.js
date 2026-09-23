"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.DeviceController = void 0;
const mqttservice_js_1 = require("../services/mqttservice.js");
class DeviceController {
    mqttService = mqttservice_js_1.MqttService.getInstance();
    enviarComandoLed = async (req, res) => {
        const { comando } = req.body;
        if (comando !== 'ENCENDER' && comando !== 'APAGAR') {
            res.status(400).json({
                status: 'error',
                message: 'Comando invalido. Debe ser ENCENDER o APAGAR'
            });
            return;
        }
        try {
            const payloadJson = JSON.stringify({ comando });
            await this.mqttService.publish('esp32/potenciometro/comandos', payloadJson);
            res.status(200).json({
                status: 'success',
                message: `Comando ${comando} enviado exitosamente hacia el hardware`
            });
        }
        catch (error) {
            res.status(500).json({
                status: 'error',
                message: error.message || 'Error de comunicacion con el broker'
            });
        }
    };
}
exports.DeviceController = DeviceController;
