# script a faire tourner dans un container sur la raspberry pi pour
# recevoir les données du joystick ET exposer une petite page web
# de télémétrie en temps réel.

import socket
import threading
import json
from http.server import HTTPServer, BaseHTTPRequestHandler

UDP_IP = "0.0.0.0"
UDP_PORT = 5000

HTTP_IP = "0.0.0.0"
HTTP_PORT = 8000

# Stockage de la dernière télémétrie reçue
latest_telemetry = {
    "posX": None,
    "posY": None,
    "action": None,
    "mode": None,
    "last_sender": None,
}
telemetry_lock = threading.Lock()


def udp_receiver():
    """Thread qui écoute l'UDP et met à jour latest_telemetry."""
    global latest_telemetry

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    print(f"[UDP] Écoute sur UDP {UDP_PORT}...")

    while True:
        data, addr = sock.recvfrom(1024)
        line = data.decode(errors="ignore").strip()

        # Parsing "posX;posY;action;mode"
        try:
            posx_str, posy_str, action_str, mode_str = line.split(";")
            posx = int(posx_str)
            posy = int(posy_str)
            action = int(action_str)
            mode = int(mode_str)

            with telemetry_lock:
                latest_telemetry["posX"] = posx
                latest_telemetry["posY"] = posy
                latest_telemetry["action"] = action
                latest_telemetry["mode"] = mode
                latest_telemetry["last_sender"] = f"{addr[0]}:{addr[1]}"
        except Exception as e:
            print("[UDP] Erreur de parsing:", e, "line =", repr(line))


class TelemetryHandler(BaseHTTPRequestHandler):
    def _set_cors_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")

    def do_OPTIONS(self):
        self.send_response(204)
        self._set_cors_headers()
        self.end_headers()

    def do_GET(self):
        if self.path == "/":
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(self._html_page().encode("utf-8"))
        elif self.path == "/telemetry":
            with telemetry_lock:
                payload = json.dumps(latest_telemetry)
            self.send_response(200)
            self._set_cors_headers()
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            self.wfile.write(payload.encode("utf-8"))
        else:
            self.send_response(404)
            self.end_headers()

    def log_message(self, format, *args):
        # Pour éviter de spammer la console avec les logs HTTP
        return

    @staticmethod
    def _html_page():
        # Petite page HTML avec rafraîchissement périodique de la télémétrie
        return """<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8" />
  <title>Télémétrie manette ESP32</title>
  <style>
    body { font-family: sans-serif; background: #111; color: #eee; }
    .card { max-width: 400px; margin: 40px auto; padding: 20px; border-radius: 8px;
            background: #222; box-shadow: 0 0 10px rgba(0,0,0,0.6); }
    h1 { font-size: 1.4rem; margin-top: 0; }
    .row { margin: 8px 0; }
    .label { display: inline-block; width: 80px; color: #aaa; }
    .value { font-weight: bold; }
    .dot {
      width: 6px; height: 6px; border-radius: 50%; display: inline-block;
      margin-right: 6px; background: #0f0;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1><span class="dot"></span>Télémétrie manette ESP32</h1>
    <div class="row"><span class="label">posX :</span><span id="posX" class="value">-</span></div>
    <div class="row"><span class="label">posY :</span><span id="posY" class="value">-</span></div>
    <div class="row"><span class="label">action :</span><span id="action" class="value">-</span></div>
    <div class="row"><span class="label">mode :</span><span id="mode" class="value">-</span></div>
    <div class="row"><span class="label">source :</span><span id="sender" class="value">-</span></div>
  </div>

  <script>
    async function refreshTelemetry() {
      try {
        const res = await fetch("/telemetry");
        if (!res.ok) return;
        const data = await res.json();
        document.getElementById("posX").textContent = data.posX ?? "-";
        document.getElementById("posY").textContent = data.posY ?? "-";
        document.getElementById("action").textContent = data.action ?? "-";
        document.getElementById("mode").textContent = data.mode ?? "-";
        document.getElementById("sender").textContent = data.last_sender ?? "-";
      } catch (e) {
        console.error(e);
      }
    }

    // Rafraîchissement toutes les 100 ms
    setInterval(refreshTelemetry, 100);
    refreshTelemetry();
  </script>
</body>
</html>
"""


def http_server():
    server = HTTPServer((HTTP_IP, HTTP_PORT), TelemetryHandler)
    print(f"[HTTP] Serveur web sur http://{HTTP_IP}:{HTTP_PORT} (utilise IP de la Pi)")
    server.serve_forever()


if __name__ == "__main__":
    # Thread UDP
    t_udp = threading.Thread(target=udp_receiver, daemon=True)
    t_udp.start()

    # Serveur HTTP (thread principal)
    http_server()
