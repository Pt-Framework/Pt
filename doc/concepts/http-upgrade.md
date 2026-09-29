# HTTP Upgrade und Websocket

Es ist das Paradebeispiel für das "Inversion of Control" (Umkehrung der Kontrolle)-Prinzip: Der Server stellt nur die HTTP-Infrastruktur bereit, und die Applikation steuert das Verhalten rein über standardisierte HTTP-Zustände.

## Wer macht das noch so? (Industrie-Beispiele)

Rust / hyper (Die Basis von Axum/Tokio): hyper ist einer der performantesten HTTP-Server überhaupt. Er weiß absolut nichts von WebSockets. Wenn eine Applikation ein Upgrade durchführen möchte, fügt sie der normalen HTTP-Response ein generisches Element hinzu (über das extensions-Feld des Response-Objekts). Nach dem Senden der Response prüft hyper, ob dieses Flag gesetzt ist, koppelt den I/O-Stream ab und übergibt ihn über ein OnUpgrade-Future an die WebSocket-Bibliothek (wie tokio-tungstenite).

Node.js (http & http2 Module): Wenn du in Node.js einen WebSocket-Server an einen HTTP-Server hängst, wartet die WebSocket-Bibliothek oft auf das standardmäßige upgrade-Event, das vom HTTP-Server gefeuert wird, nachdem die HTTP-Header analysiert wurden. Der Server entscheidet anhand des HTTP-Zustands, die Verbindung an die Extension weiterzureichen.

## Warum das eine exzellente Lösung für dein Design ist

Dieses Design bietet eine perfekte Balance zwischen Entkopplung und Protokoll-Treue. Hier sind die wichtigsten Vorteile für deine Architektur:

1. Maximale Entkopplung (Clean Architecture)
Dein Server bleibt zu 100 % ein reiner HTTP-Server. Er benötigt keine Abhängigkeiten zu WebSocket-Frames, Maskierung (Masking) oder Ping/Pong-Logik. Wenn morgen ein neues Protokoll über HTTP/2 oder HTTP/3 tunneln möchte (z. B. WebTransport oder SSH-over-HTTP), musst du am Core-Server keine einzige Zeile Code ändern.

2. Einheitlicher Lebenszyklus
Jeder Request – egal ob REST-API, statisches Bild oder WebSocket – durchläuft exakt dieselbe Pipeline: Mapper -> Service -> Responder.onBeginRequest(). Das macht Logging, Telemetrie, Authentifizierungs-Middleware und Ratenbegrenzung (Rate Limiting) im Server extrem einfach, da sie nicht wissen müssen, was am Ende aus dem Request wird.

3. Perfekte HTTP/2-Kompatibilität (RFC 8441)
Gerade bei HTTP/2 zeigt sich die Stärke dieses Designs. Da die TCP-Verbindung beim Server verbleiben muss, darf die Extension niemals den echten TCP-Socket anfassen. Indem der Server am Reply (CONNECT + 200) erkennt, was zu tun ist, kann er den HTTP/2-Stream-Multiplexer anweisen:

„Leite ab jetzt alle HTTP/2-DATA-Frames mit der Stream-ID X direkt an diesen Responder weiter, anstatt sie durch den HTTP-Parser zu jagen.“

## Worauf du bei der Implementierung achten musst (Die Fallstricke)

Damit die Lösung perfekt funktioniert, solltest du zwei Details im Server-Core beachten:

Das Timing des Protokollwechsels:Der Server darf den Stream/Socket erst nachdem die Header des Reply vollständig und erfolgreich auf die Leitung geschrieben wurden, an den Responder übergeben (provideStream). Schreibt der Responder zu früh in den rohen Stream, vermischen sich die WebSocket-Frames mit den HTTP-Antwort-Headern, was die Verbindung korrumpiert.
Speicher- und Timeout-Management:Normale HTTP-Requests haben im Server oft ein Read-/Write-Timeout (z. B. 30 Sekunden). Sobald der Server am Reply das Upgrade (101) oder den Tunnel (200 CONNECT) erkennt, muss er die HTTP-Timeouts für diese Verbindung/diesen Stream deaktivieren (oder stark anheben), da WebSockets für langlebige Verbindungen gedacht sind.

## Fazit

Du hast hier intuitiv ein High-End-Architekturmuster gewählt. Es hält deinen Server schlank, macht deine WebSocket-Extension modular austauschbar und löst das Problem, dass die Verbindung für HTTP/2 sicher beim Server verbleibt.

## Offene Punkte:

- Wie das Schnittstellen-Design des Stream-Objekts aussehen sollte, damit die
  Extension HTTP/1-Sockets und HTTP/2-Streams identisch behandeln kann?
- Wenn wir im Responder mit der normalen Request/Reply API dem server eine upgrade
  signalisieren wolln, dann haben wir in Reply -> Message -> Connection schon
  Zugriff auf viele Klassen. Kan man da was machen?
- Soll Upgrade wirklich einen Wechsel des Thread durchführen?
