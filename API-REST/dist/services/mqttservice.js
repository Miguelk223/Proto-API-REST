"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.MqttService = void 0;
const mqtt_1 = __importDefault(require("mqtt"));
class MqttService {
    client = null;
    static instance;
    estaActivo = true;
    constructor() { }
    static getInstance() {
        return MqttService.instance || (MqttService.instance = new MqttService());
    }
    connect(url, options) {
        if (this.client)
            return;
        console.log('[MQTT] Conectando a HiveMQ...');
        this.client = mqtt_1.default.connect(url, options);
        this.client.on('connect', () => {
            console.log('[MQTT] Conectado con éxito.');
            this.client?.subscribe('esp32/#');
        });
        this.client.on('message', (topic, msg) => this.procesarMensaje(topic, msg.toString().trim()));
        this.client.on('error', (err) => console.error('[MQTT ERROR]', err));
    }
    procesarMensaje(topic, contenido) {
        if (contenido.toLowerCase() === 'apagar')
            return this.desactivar('[TEXTO]');
        if (!this.estaActivo)
            return;
        if (contenido.startsWith('{')) {
            try {
                const data = JSON.parse(contenido);
                if (data.comando?.toLowerCase() === 'apagar')
                    return this.desactivar('[JSON]');
                if (data.dispositivo === 'ESP32_IoT' && data.ntc_termistor) {
                    const { lectura_raw, temperatura_c } = data.ntc_termistor;
                    console.log(`\n=============================================\n[TELEMETRÍA] ${data.dispositivo}\n  - ADC Raw: ${lectura_raw} | Temp: ${temperatura_c} °C\n=============================================\n`);
                }
            }
            catch {
                console.error(`[MQTT PARSE ERROR] Error en ${topic}`);
            }
        }
    }
    desactivar(origen) {
        this.estaActivo = false;
        console.log(`[MQTT] Comando "apagar" detectado ${origen}. Peticiones detenidas.`);
    }
    publish(topic, message) {
        return new Promise((resolve, reject) => {
            if (!this.estaActivo)
                return reject(new Error('Servicio MQTT desactivado (APAGADO)'));
            if (!this.client?.connected)
                return reject(new Error('Broker MQTT desconectado'));
            this.client.publish(topic, message, { qos: 1 }, (err) => {
                if (err)
                    return reject(err);
                console.log(`[MQTT OUT] Publicado en [${topic}]: ${message}`);
                resolve();
            });
        });
    }
    cambiarEstado(activo) {
        this.estaActivo = activo;
        console.log(`[MQTT] Estado cambiado a: ${activo ? 'ACTIVO' : 'APAGADO'}`);
    }
}
exports.MqttService = MqttService;
