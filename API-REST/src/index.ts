import express from 'express';
import dotenv from 'dotenv';
import deviceRoutes from './routes/deviceroutes';
import { MqttService } from './services/mqttservice';

dotenv.config();

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());

const mqttService = MqttService.getInstance();
const mqttUrl = process.env.MQTT_URL || 'mqtts://81787af48ba24db4aeb07062d144e378.s1.eu.hivemq.cloud:8883';

mqttService.connect(mqttUrl, {
  username: process.env.MQTT_USER,
  password: process.env.MQTT_PASSWORD,
  rejectUnauthorized: false
});

app.use('/api/device', deviceRoutes);

app.listen(PORT, () => {
  console.log(`[SERVER] REST API activa en http://localhost:${PORT}`);
});
