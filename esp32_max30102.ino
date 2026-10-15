/*
 * =====================================================================
 * ESP32 + MAX30102 Instant 1-Second BPM & Real-Time Waveform Monitor
 * =====================================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include "MAX30105.h"

const bool CONNECT_TO_EXISTING_WIFI = false;
const char* ssid     = "ESP32-Health-Monitor";
const char* password = "password123";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
MAX30105 particleSensor;

// --- Fast 1-Second Instant Beat Detection Variables ---
unsigned long lastBeatTime = 0;
float instantBPM = 0;
float displayBPM = 0;
int calculatedSpO2 = 98;

// DC baseline removal and smoothing
float dcIR = 0;
float dcRed = 0;
float smoothAC_IR = 0;
float prevAC_IR = 0;
float peakVal = -9999;
float troughVal = 9999;
float dynamicThreshold = 15.0;
bool rising = false;
unsigned long lastPeakTime = 0;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 MAX30102 Instant Monitor</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: system-ui, -apple-system, sans-serif; }
    body { background: #0b0f19; color: #f8fafc; padding: 14px; display: flex; flex-direction: column; align-items: center; }
    .card { background: #151e2e; border: 1px solid #283548; border-radius: 16px; padding: 18px; width: 100%; max-width: 650px; box-shadow: 0 10px 30px rgba(0,0,0,0.6); }
    .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #283548; padding-bottom: 10px; margin-bottom: 14px; }
    .stats-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 14px; }
    .stat-box { background: #0b0f19; border: 1px solid #283548; border-radius: 12px; padding: 12px 8px; text-align: center; }
    .stat-title { font-size: 11px; text-transform: uppercase; letter-spacing: 0.05em; color: #94a3b8; }
    .stat-value { font-size: 32px; font-weight: 900; font-family: monospace; margin-top: 2px; }
    .bpm-color { color: #ff3366; }
    .spo2-color { color: #00d2ff; }
    .raw-color { color: #ffaa00; }
    .canvas-box { background: #0b0f19; border: 1px solid #283548; border-radius: 12px; padding: 8px; position: relative; }
    canvas { width: 100%; height: 180px; display: block; border-radius: 8px; }
    .legend { display: flex; justify-content: space-between; font-size: 11px; color: #94a3b8; margin-top: 8px; padding: 0 4px; }
    .pulse-anim { animation: beatFlash 0.25s ease-out; }
    @keyframes beatFlash { 0% { transform: scale(1); filter: drop-shadow(0 0 0px #ff3366); } 50% { transform: scale(1.25); filter: drop-shadow(0 0 12px #ff3366); } 100% { transform: scale(1); } }
  </style>
</head>
<body>
  <div class="card">
    <div class="header">
      <h2 style="font-size: 16px; display: flex; align-items: center; gap: 8px;">
        <span id="dot" style="width: 10px; height: 10px; border-radius: 50%; background: #00ff88; box-shadow: 0 0 8px #00ff88;"></span>
        ESP32 Instant Heart Rate (1-Sec BPM)
      </h2>
      <span id="liveTag" style="font-size: 11px; padding: 2px 8px; border-radius: 12px; background: rgba(0,255,136,0.15); color: #00ff88;">Live 50Hz</span>
    </div>

    <div class="stats-grid">
      <div class="stat-box">
        <div class="stat-title">Instant BPM</div>
        <div class="stat-value bpm-color" id="bpmVal">--</div>
        <div style="font-size:10px; color:#94a3b8;">Real-Time Pulse</div>
      </div>
      <div class="stat-box">
        <div class="stat-title">SpO2 (Est.)</div>
        <div class="stat-value spo2-color" id="spo2Val">--</div>
        <div style="font-size:10px; color:#94a3b8;">Oxygen %</div>
      </div>
      <div class="stat-box">
        <div class="stat-title">IR Signal</div>
        <div class="stat-value raw-color" id="rawVal" style="font-size: 22px; padding-top: 6px;">--</div>
        <div style="font-size:10px; color:#94a3b8;">Raw Photodiode</div>
      </div>
    </div>

    <div class="canvas-box">
      <canvas id="chart"></canvas>
      <div class="legend">
        <span><b style="color:#ff3366;">—</b> Red Pulse &nbsp; <b style="color:#ffaa00;">—</b> IR Pulse</span>
        <span id="fingerState" style="color: #00ff88;">Ready</span>
      </div>
    </div>
  </div>

  <script>
    const canvas = document.getElementById('chart');
    const ctx = canvas.getContext('2d');
    function resize() {
      canvas.width = canvas.parentElement.clientWidth - 16;
      canvas.height = 180;
    }
    resize();
    window.addEventListener('resize', resize);

    const maxPoints = 180;
    let redData = new Array(maxPoints).fill(0);
    let irData = new Array(maxPoints).fill(0);

    const ws = new WebSocket(`ws://${location.hostname}:81/`);

    ws.onmessage = (event) => {
      const parts = event.data.split(',');
      if (parts.length >= 6) {
        const acRed = parseFloat(parts[0]);
        const acIR = parseFloat(parts[1]);
        const bpm = parseInt(parts[2]);
        const spo2 = parseInt(parts[3]);
        const rawIR = parseInt(parts[4]);
        const finger = parts[5] === '1';

        const bpmElem = document.getElementById('bpmVal');
        if (finger && bpm > 30) {
          if (bpmElem.innerText != bpm) {
            bpmElem.classList.add('pulse-anim');
            setTimeout(() => bpmElem.classList.remove('pulse-anim'), 250);
          }
          bpmElem.innerText = bpm;
        } else {
          bpmElem.innerText = finger ? 'Detecting...' : '--';
        }

        document.getElementById('spo2Val').innerText = finger && spo2 > 0 ? spo2 + '%' : '--';
        document.getElementById('rawVal').innerText = rawIR.toLocaleString();
        document.getElementById('fingerState').innerText = finger ? 'Pulse Active' : 'Place finger gently on sensor';
        document.getElementById('fingerState').style.color = finger ? '#00ff88' : '#ffaa00';

        redData.shift(); redData.push(finger ? acRed : 0);
        irData.shift();  irData.push(finger ? acIR : 0);

        drawGraph();
      }
    };

    function drawGraph() {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      const w = canvas.width;
      const h = canvas.height;
      const midY = h / 2;

      ctx.strokeStyle = '#1e293b';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, midY); ctx.lineTo(w, midY);
      ctx.stroke();

      let maxVal = 40;
      for (let i = 0; i < maxPoints; i++) {
        maxVal = Math.max(maxVal, Math.abs(redData[i]), Math.abs(irData[i]));
      }
      const scale = (h * 0.44) / maxVal;

      // Draw Red
      ctx.beginPath();
      ctx.strokeStyle = '#ff3366';
      ctx.lineWidth = 2.2;
      for (let i = 0; i < maxPoints; i++) {
        const x = (i / (maxPoints - 1)) * w;
        const y = midY - redData[i] * scale;
        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.stroke();

      // Draw IR
      ctx.beginPath();
      ctx.strokeStyle = '#ffaa00';
      ctx.lineWidth = 1.5;
      for (let i = 0; i < maxPoints; i++) {
        const x = (i / (maxPoints - 1)) * w;
        const y = midY - irData[i] * scale;
        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.stroke();
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("[ERROR] MAX30102 not found!");
    while (1);
  }

  // Setup MAX30102: 400Hz sample rate, 411us pulse width, 18-bit ADC resolution
  particleSensor.setup(0x3F, 4, 2, 400, 411, 4096);
  particleSensor.setPulseAmplitudeRed(0x3F); // ~12.5mA
  particleSensor.setPulseAmplitudeIR(0x7F);  // ~25mA
  particleSensor.setPulseAmplitudeGreen(0);

  if (CONNECT_TO_EXISTING_WIFI) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(300);
  } else {
    WiFi.softAP(ssid, password);
  }

  server.on("/", []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  server.begin();
  webSocket.begin();

  Serial.println("Instant BPM 1-Second Engine Ready.");
}

unsigned long lastSend = 0;

void loop() {
  server.handleClient();
  webSocket.loop();

  uint32_t rawIR = particleSensor.getIR();
  uint32_t rawRed = particleSensor.getRed();

  // Any reading above 7000 indicates a finger is placed
  bool fingerDetected = (rawIR > 6500);

  if (fingerDetected) {
    // 1. High-Pass DC Baseline Tracking
    if (dcIR == 0) { dcIR = rawIR; dcRed = rawRed; }
    dcIR  = 0.93 * dcIR  + 0.07 * rawIR;
    dcRed = 0.93 * dcRed + 0.07 * rawRed;

    float rawAC_IR  = (float)rawIR  - dcIR;
    float rawAC_Red = (float)rawRed - dcRed;

    // 2. Low-Pass Smoothing (eliminates high frequency jitter)
    smoothAC_IR = 0.65 * smoothAC_IR + 0.35 * rawAC_IR;

    // 3. Instant 1-Second Peak Detection (Zero-Crossing + Derivative Peak)
    if (smoothAC_IR > prevAC_IR) {
      // Slope is rising
      if (!rising && prevAC_IR < 0 && smoothAC_IR >= 0) {
        rising = true;
      }
      if (smoothAC_IR > peakVal) peakVal = smoothAC_IR;
    } else if (rising && smoothAC_IR < prevAC_IR) {
      // Peak reached (slope turns downward)
      if (peakVal > dynamicThreshold) {
        unsigned long now = millis();
        unsigned long timeDelta = now - lastPeakTime;

        // Valid human heartbeat period: 330ms (180 BPM) to 1500ms (40 BPM)
        if (timeDelta >= 330 && timeDelta <= 1500) {
          lastPeakTime = now;
          instantBPM = 60000.0 / (float)timeDelta;

          // Responsive Instant BPM filter (80% instant + 20% previous)
          if (displayBPM == 0) {
            displayBPM = instantBPM; // Instant 1st beat calculation!
          } else {
            displayBPM = 0.75 * instantBPM + 0.25 * displayBPM;
          }

          // Adapt threshold to 50% of current pulse height
          dynamicThreshold = max(12.0f, peakVal * 0.45f);
        } else if (timeDelta > 1500) {
          // Reset after long pause
          lastPeakTime = now;
        }

        peakVal = -9999;
        rising = false;
      }
    }

    prevAC_IR = smoothAC_IR;

    // Slowly decay dynamic threshold if no peak
    dynamicThreshold = max(12.0f, dynamicThreshold * 0.997f);

    // Estimate SpO2
    float r = (abs(rawAC_Red) / (dcRed + 1.0)) / (abs(rawAC_IR) / (dcIR + 1.0) + 0.0001);
    int spo2 = constrain((int)(110.0 - 25.0 * r), 90, 100);
    calculatedSpO2 = (calculatedSpO2 * 9 + spo2) / 10;

    // Serial Stream for Serial Plotter
    Serial.print("Raw_IR:"); Serial.print(rawIR);
    Serial.print(",Pulse_IR:"); Serial.print(smoothAC_IR);
    Serial.print(",Pulse_Red:"); Serial.print(rawAC_Red);
    Serial.print(",BPM:"); Serial.println((int)displayBPM);

    // Broadcast to Web Browser (~30 fps)
    if (millis() - lastSend > 33) {
      lastSend = millis();
      String payload = String(rawAC_Red, 1) + "," + String(smoothAC_IR, 1) + "," +
                       String((int)displayBPM) + "," + String(calculatedSpO2) + "," +
                       String(rawIR) + ",1";
      webSocket.broadcastTXT(payload);
    }
  } else {
    // Finger removed -> reset
    dcIR = 0;
    dcRed = 0;
    smoothAC_IR = 0;
    prevAC_IR = 0;
    peakVal = -9999;
    rising = false;
    displayBPM = 0;
    dynamicThreshold = 15.0;

    if (millis() - lastSend > 200) {
      lastSend = millis();
      String payload = "0,0,0,0," + String(rawIR) + ",0";
      webSocket.broadcastTXT(payload);
      Serial.print("Raw_IR:"); Serial.print(rawIR);
      Serial.println(",Finger:NO,BPM:0");
    }
  }

  delay(4);
}
