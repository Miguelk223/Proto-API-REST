"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = require("express");
const devicecontrollers_1 = require("../controllers/devicecontrollers");
const router = (0, express_1.Router)();
const controller = new devicecontrollers_1.DeviceController();
router.post('/led/comando', controller.enviarComandoLed);
exports.default = router;
