# EsteiraIot
Esteira de materiais de forma cúbica ou paralelepípedos que são medidos enquanto passam por ela.
ASHBOARD DA ESTEIRA V3 - COMO RODAR

ANTES DE TUDO (uma vez por computador):
1. Instale o Mosquitto: https://mosquitto.org/download/ (versao Windows 64 bits).
   Precisa de permissao de administrador.
2. Instale o Python: https://www.python.org/downloads/
   Na primeira tela marque "Add python.exe to PATH".

PARA RODAR:
1. Deixe os arquivos desta pasta juntos (nao mova um para outra pasta).
2. D? dois cliques em iniciar.bat.
3. Vao abrir duas janelas pretas (broker e simulador). Nao feche essas janelas.
4. O dashboard abre no navegador. Clique em CONECTAR.
5. Em alguns segundos os valores comecam a mudar.

PARA ENCERRAR:
Feche as janelas "Broker MQTT" e "Simulador".

SE DER ERRO:
- "mosquitto nao e reconhecido": o Mosquitto nao esta instalado ou nao esta no PATH.
- "python nao e reconhecido": o Python foi instalado sem marcar "Add python.exe to PATH".
- Dashboard mostra erro/desconectado: a janela "Broker MQTT" nao esta aberta.
- Numeros nao mudam: a janela "Simulador" nao esta aberta ou deu erro (leia a mensagem nela).

ARQUIVOS:
- iniciar.bat: abre broker, simulador e dashboard.
- mosquitto.conf: configuracao do broker (portas 1883 MQTT e 9001 WebSocket).
- simulador_esteira.py: envia dados falsos para testar o dashboard sem o ESP32.
- dashboard_esteira_local.html: o dashboard (abre no navegador).
