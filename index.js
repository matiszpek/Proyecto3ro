const express = require("express");
const WebSocket = require("ws");

const app = express();
const port = 3000;

app.use(express.static("public")); // Carpeta para servir archivos estáticos (HTML/CSS/JS)

const server = app.listen(port, () => {
    console.log(`Servidor escuchando en http://localhost:${port}`);
});

// Configurar WebSocket
const wss = new WebSocket.Server({ server });

wss.on("connection", (ws) => {
    console.log("Cliente conectado");

    ws.on("message", (message) => {
        console.log(`Mensaje recibido: ${message}`);

        // Aquí puedes procesar la configuración de las luces y pasarla a ESP-NOW
        // ws.send('Respuesta desde el servidor'); // Enviar respuesta si es necesario

        // Enviar mensaje a todos los clientes conectados
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(message);
            }
        });

        ws.on("close", () => {
            console.log("Cliente desconectado");
        });
    });
});
// el codigo para iniciar el server es: ./localxpose http 3000 --subdomain=tu_subdominio
