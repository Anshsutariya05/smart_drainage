
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

const readings = [];

app.get('/', (req, res) => {
  res.send('Smart Drainage backend is running');
});

app.get('/data', (req, res) => {
  res.json({
    count: readings.length,
    latest: readings.length ? readings[readings.length - 1] : null,
  });
});

app.post('/data', (req, res) => {
  // console.log('Received request body:', req.body);
  
  const { distance, motor } = req.body || {};
  
  if (typeof distance !== 'number' || !['ON', 'OFF'].includes(motor)) {
    return res.status(400).json({
      error: 'Invalid payload. Expect: { distance: number, motor: "ON"|"OFF" }',
      got: req.body
    });
  }
  
  const event = { distance, motor, ts: Date.now() };
  readings.push(event);
  console.log('✓ Received:', event);
  
  res.json({ ok: true });
});

app.listen(PORT, () => {
  console.log(`Server running at http://127.0.0.1:${PORT}`);

});