// snippet: serial_plotter_dashboard
// tags: serial, plotter, dashboard, telemetry, monitor, csv, python, matplotlib, live
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Stream multi-channel sensor data in Arduino Serial Plotter format AND as CSV.
//   Python companion plots live graphs with matplotlib. Works with any number of channels.
//   Arduino Serial Plotter: open Tools → Serial Plotter (labels appear as channel names).
// prompts: serial plotter live data, CSV telemetry streaming, matplotlib live plot, serial dashboard multi-channel, data visualization
//
// ---- Python live-plot companion ----
//   pip install pyserial matplotlib
//
//   import serial, collections, threading
//   import matplotlib.pyplot as plt
//   import matplotlib.animation as animation
//
//   PORT   = '/dev/ttyUSB0'   # Windows: 'COM3'
//   BAUD   = 115200
//   WINDOW = 200              # samples to display
//
//   ser    = serial.Serial(PORT, BAUD, timeout=1)
//   buf    = collections.defaultdict(lambda: collections.deque([0]*WINDOW, maxlen=WINDOW))
//   labels = []
//
//   def reader():
//       for line in ser:
//           try:
//               parts = line.decode().strip().split(',')
//               for i, p in enumerate(parts):
//                   kv = p.split(':')
//                   key = kv[0].strip() if len(kv)==2 else f"ch{i}"
//                   val = float(kv[-1].strip())
//                   buf[key].append(val)
//           except: pass
//
//   threading.Thread(target=reader, daemon=True).start()
//
//   fig, ax = plt.subplots()
//   def update(_):
//       ax.cla()
//       for k, v in buf.items(): ax.plot(v, label=k)
//       ax.legend(loc='upper right'); ax.grid(True)
//   ani = animation.FuncAnimation(fig, update, interval=50)
//   plt.show()
// -----------------------------------
// exact-include: math.h

#include <math.h>

unsigned long prevMs = 0;
float t = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 20) return;  // 50 Hz
  prevMs = now;
  t += 0.02f;

  // Replace with real sensor reads
  float accel = analogRead(A0) * (5.0f / 1023.0f);
  float temp  = 20.0f + 5.0f * sin(t * 0.3f);       // simulated
  float dist  = 50.0f + 30.0f * cos(t * 0.7f);       // simulated

  // Format: "label:value,label:value\n"
  // Arduino Serial Plotter reads this natively
  Serial.print("accel:"); Serial.print(accel, 3);
  Serial.print(",temp:");  Serial.print(temp,  2);
  Serial.print(",dist:");  Serial.println(dist, 1);
}
