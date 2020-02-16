# AskSinSniffer328P

Sketch für den Arduino (Pro Mini), um die Telegramme zu sniffen.

## Erweiterung um die Möglichkeit Anzeige des aktuellen Signalpegel

Dieser Fork bietet die Möglichkeit, ein 0.91" Dipsplay zu verwenden, um darauf den aktuellen Signalpegel
anzuzeigen. Eine dynamische Pegelanzeige ist ebenfalls implementiert, um Ändereungen zu visualisieren.

![AskSin-Analyzer Pegel](https://github.com/der-pw/AskSinAnalyzer/blob/master/Images/Sniffer_Display.jpg "AskSin-Analyzer Pegel")

## Flashen

Es gibt verschiedene Möglichkeiten, den Sketch zu flashen:

### Arduino IDE

Benötigte Bibliotheken: AskSinPP, Low-Power, EnableInterrupt

### avrdude

Direktes Flashen der HEX-Datei über avrdude:
```bash
avrdude -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:AskSinSniffer328P.hex:i
```

### PlatformIO

Dem Projekt liegt eine `platformio.ini` bei.

Um die Bibliotheken zu laden, den Sketch zu kompilieren und hochzuladen, reicht ein Command:

```bash
platformio run -t upload
```

```bash
# Serial monitor
platformio device monitor
```


