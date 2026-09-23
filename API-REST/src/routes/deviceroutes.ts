import { Router } from 'express';
import { DeviceController } from '../controllers/devicecontrollers';

const router = Router();
const controller = new DeviceController();

router.post('/led/comando', controller.enviarComandoLed);

export default router;