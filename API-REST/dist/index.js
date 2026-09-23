"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const dotenv_1 = __importDefault(require("dotenv"));
const deviceroutes_1 = __importDefault(require("./routes/deviceroutes"));
const mqttservice_1 = require("./services/mqttservice");
dotenv_1.default.config();
const app = (0, express_1.default)();
const PORT = process.env.PORT || 3000;
app.use(express_1.default.json());
const mqttService = mqttservice_1.MqttService.getInstance();
const mqttUrl = process.env.MQTT_URL || 'mqtts://81787af48ba24db4aeb07062d144e378.s1.eu.hivemq.cloud:8883';
mqttService.connect(mqttUrl, {
    username: process.env.MQTT_USER,
    password: process.env.MQTT_PASSWORD,
    rejectUnauthorized: false
});
app.use('/api/device', deviceroutes_1.default);
app.listen(PORT, () => {
    console.log(`[SERVER] REST API activa en http://localhost:${PORT}`);
});
