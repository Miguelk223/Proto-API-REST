import mqtt from 'mqtt';

interface TelemetriaPayload {
  dispositivo: string;
  comando?: string;
  ntc_termistor?: { lectura_raw: number; temperatura_c: number };
}

export class MqttService {
  private client: mqtt.MqttClient | null = null;
  private static instance: MqttService;
  private estaActivo = true;

  private constructor() {}

  public static getInstance(): MqttService {
    return MqttService.instance || (MqttService.instance = new MqttService());
  }

  public connect(url: string, options?: mqtt.IClientOptions): void {
    if (this.client) return;
    console.log('[MQTT] Conectando a HiveMQ...');
    this.client = mqtt.connect(url, options);

    this.client.on('connect', () => {
      console.log('[MQTT] Conectado con éxito.');
      this.client?.subscribe('esp32/#');
    });

    this.client.on('message', (topic, msg) => this.procesarMensaje(topic, msg.toString().trim()));
    this.client.on('error', (err) => console.error('[MQTT ERROR]', err));
  }

  private procesarMensaje(topic: string, contenido: string): void {
    if (contenido.toLowerCase() === 'apagar') return this.desactivar('[TEXTO]');
    if (!this.estaActivo) return;

    if (contenido.startsWith('{')) {
      try {
        const data: TelemetriaPayload = JSON.parse(contenido);
        if (data.comando?.toLowerCase() === 'apagar') return this.desactivar('[JSON]');

        if (data.dispositivo === 'ESP32_IoT' && data.ntc_termistor) {
          const { lectura_raw, temperatura_c } = data.ntc_termistor;
          console.log(`\n=============================================\n[TELEMETRÍA] ${data.dispositivo}\n  - ADC Raw: ${lectura_raw} | Temp: ${temperatura_c} °C\n=============================================\n`);
        }
      } catch {
        console.error(`[MQTT PARSE ERROR] Error en ${topic}`);
      }
    }
  }

  private desactivar(origen: string): void {
    this.estaActivo = false;
    console.log(`[MQTT] Comando "apagar" detectado ${origen}. Peticiones detenidas.`);
  }

  public publish(topic: string, message: string): Promise<void> {
    return new Promise((resolve, reject) => {
      if (!this.estaActivo) return reject(new Error('Servicio MQTT desactivado (APAGADO)'));
      if (!this.client?.connected) return reject(new Error('Broker MQTT desconectado'));

      this.client.publish(topic, message, { qos: 1 }, (err) => {
        if (err) return reject(err);
        console.log(`[MQTT OUT] Publicado en [${topic}]: ${message}`);
        resolve();
      });
    });
  }

  public cambiarEstado(activo: boolean): void {
    this.estaActivo = activo;
    console.log(`[MQTT] Estado cambiado a: ${activo ? 'ACTIVO' : 'APAGADO'}`);
  }
}
