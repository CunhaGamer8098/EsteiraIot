"""
Simulador da esteira v3: publica dados falsos no Mosquitto local para testar o dashboard
sem o ESP32 conectado.

Uso:
    pip install paho-mqtt
    python simulador_esteira.py

Para parar: Ctrl+C
"""
import json
import math
import random
import time

import paho.mqtt.client as mqtt

BROKER = "localhost"   # computador onde o Mosquitto está rodando
PORTA = 1883
BASE = "esteira/v3"    # precisa ser igual ao prefixo do dashboard
ESTADOS = ["parado", "alinhando", "empurrando", "recuando", "medindo"]


def publicar(cliente, sufixo, payload):
    cliente.publish(f"{BASE}/{sufixo}", json.dumps(payload))


def main():
    cliente = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    cliente.connect(BROKER, PORTA)
    cliente.loop_start()
    print(f"Publicando em {BASE}/... (Ctrl+C para parar)")

    total = 0
    t = 0
    try:
        while True:
            z = 8 + 4 * math.sin(t / 5) + random.uniform(-0.3, 0.3)
            lateral = 10 + 3 * math.cos(t / 7) + random.uniform(-0.3, 0.3)
            posicao = min(max(35 + 35 * math.sin(t / 9), 0), 70)
            ir = random.random() < 0.2
            estado = ESTADOS[(t // 6) % len(ESTADOS)]
            ligado = estado != "parado"

            publicar(cliente, "sensores/z", {"distancia_cm": round(z, 1)})
            publicar(cliente, "sensores/lateral", {"distancia_cm": round(lateral, 1)})
            publicar(cliente, "sensores/ir", {"detectado": ir})
            publicar(cliente, "atuadores/parede", {"posicao_mm": round(posicao, 1)})
            publicar(cliente, "atuadores/esteira", {"ligado": ligado})
            publicar(cliente, "status", {"estado": estado})

            if t % 10 == 0 and t > 0:
                total += 1
                publicar(cliente, "contagem", {"total": total})

            time.sleep(1)
            t += 1
    except KeyboardInterrupt:
        print("\nSimulador parado.")
    finally:
        cliente.loop_stop()
        cliente.disconnect()


if __name__ == "__main__":
    main()
