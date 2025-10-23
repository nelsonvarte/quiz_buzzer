const express = require('express');
const bodyParser = require('body-parser');
const http = require('http');
const WebSocket = require('ws');
const app = express();

app.use(bodyParser.json());

let responders = [];  // Array to store multiple responders

// Serve static files from the 'public' folder
app.use(express.static('public'));

// Endpoint to submit button press
app.post('/submit', (req, res) => {
  const { id, name } = req.body;  // Extract ID and name from the body

  // Check if this responder is already in the list
  const existingResponder = responders.find(responder => responder.id === id);
  
  if (!existingResponder) {
    // Add new responder to the list
    responders.push({ id, name });
    console.log(`New responder: ${name} (ID: ${id})`);
	
    broadcastResult(true); // Broadcast update to all connected clients
  } else {
    console.log(`Responder already submitted: ${name} (ID: ${id})`);
	broadcastResult(false);
  }

  res.status(200).send({ status: 'received', id, name });
});

// Endpoint to reset the responders
app.post('/reset', (req, res) => {
  responders = [];  // Clear all responders
  console.log('Reset all responders');
  broadcastResult(false); // Notify all clients
  res.status(200).send({ status: 'reset' });
});

// Create HTTP and WebSocket server
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Broadcast the updated list of responders to all WebSocket clients
function broadcastResult(isNewResponder) {
  const message = JSON.stringify({ responders, new: isNewResponder });
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(message);
    }
  });
}


// Listen for WebSocket connections
wss.on('connection', (ws) => {
  console.log('New WebSocket connection');
  // Send the current list of responders to the new client
  ws.send(JSON.stringify({ responders }));
});

// Start the server on port 3000
server.listen(3000, () => {
  console.log('Server running on port 3000');
});
