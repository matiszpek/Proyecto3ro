// Instala las dependencias: express y ws (websockets)
const express = require('express');
const WebSocket = require('ws');

// Inicializamos el servidor Express
const app = express();
const port = process.env.PORT || 3000;

// Servidor HTTP
const server = app.listen(port, () => {
    console.log(`Servidor escuchando en el puerto ${port}`);
});

// Inicializamos el servidor WebSocket
const wss = new WebSocket.Server({ server });

wss.on('connection', ws => {
    console.log('Nuevo cliente conectado');
    
    ws.on('message', message => {
        console.log('Mensaje recibido del cliente:', message);
        // Aquí puedes procesar los mensajes de la página web y enviar comandos al ESP32
        ws.send('Comando recibido');
    });
    
    ws.on('close', () => {
        console.log('Cliente desconectado');
    });
});

// Rutas de ejemplo para la web
app.get('/', (req, res) => {
    res.send('Servidor WebSocket activo');
});
