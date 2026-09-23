import { Request, Response } from 'express';
import { MqttService } from '../services/mqttservice.js';

export class DeviceController {
  private mqttService = MqttService.getInstance();

  public enviarComandoLed = async (req: Request, res: Response): Promise<void> => {
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
    } catch (error: any) {
      res.status(500).json({ 
        status: 'error', 
        message: error.message || 'Error de comunicacion con el broker' 
      });
    }
  };
}