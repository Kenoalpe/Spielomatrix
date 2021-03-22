
//Abschnitt 1 - Einbinden der nötigen Bibliotheken-----------------------------------------------------------
#include <RGBmatrixPanel.h>       //Panel
#include <Wire.h>                 //Zur verwendung der I2C Schnittstelle
#include <Nunchuk.h>              //Joystick
#include <DHT.h>                  //Temperatursensor
#include <DFPlayer_Mini_Mp3.h>    //Lautsprecher
//-----------------------------------------------------------------------------------------------------------

//Abschnitt 2 - Pins am Mikrokontroller werden definiert und zugewiesen--------------------------------------
//Panel
#define CLK 11
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

//Temperatur
#define DHTPIN 4
#define DHTTYPE DHT11

//I2C Schnittstelle
#define RX 12
#define TX 13

//Die definierten Pins werden den Bauteilen zugewiesen zur nutzung in den Bibliotheken
RGBmatrixPanel *matrix = new RGBmatrixPanel(A, B, C, D, CLK, LAT, OE, true, 64);
SoftwareSerial mySerial(RX, TX);
DHT dht(DHTPIN, DHTTYPE);

//Zwei Befehle hinzugefügen die das Programmieren vereinfachen z.B. matrix->clear() anstatt matrix->fillScreen(0)
#define clear()          fillScreen(0)
#define show()           swapBuffers(true)
//-----------------------------------------------------------------------------------------------------------

//Abschnitt 3 - Zähler,Indikatoren und Positionen von z.B. Pixeln werden hier als Variablen Deklariert-------

//Nunchuk----------------------------------------------------------
int nunPin1 = 2; //Pin für den ersten Transistor
int nunPin2 = 3; //Pin für den zweiten Transistor
int state = LOW; //Sagt welcher Nunchuck angesteuert werden soll
int z1, c1, x1, y1, z2, c2, x2, y2; //Hierdrin werden die Werte des Joysticks (x1,x2,y1,y2) und für die Knöpfe gespeichert (z1,z2,c1,c2)

//Allgemein genutzte Variablen-------------------------------------
unsigned long farbeAenderung; //Dieser Wert gilt als Indikation für die Farbe in der ColorHSV funktion der RGBmatrixPanel Bibliothek
bool ind[5]; //Indikatoren mit verschiedenen anwendungen - (Prinzip einmalige ausführung in der Loop)
//Deklariere die zwei meist benutzten Farben
int w = matrix->Color333(15, 15, 15); //Weiß angabe in (R,G,B) Werte von 0-15
int cu = matrix->Color333(15, 1, 1);

//Layout und Buchstaben Variablen Auswahl--------------------------
//Speicher der Buchstaben/Zahlen/Zeichen in einer Binären Anordnung 1 = LED AN, 0 = LED AUS
/*zum Beispiel eine 8

    111
    101
    111
    101
    111

*/
int BS[26];
int BSBackup[5];
int Bzahl[10];
int Bzeichen[4];

int UhrzeitPos = 41; //Position der Uhrzeit
char e; //Lesen der Seriellen Schnittstelle
int z; //Zähler für die Auswahl
int u; //Zähler für den Doppelpunkt
int Temp; //Temperatur des DHT11 Sensors

int feldfarbe[5]; //Farben für die Auswahlfelder
int Auswl = 2; //Aktuell ausgewähltes Feld
int indikator = 0; //Zeigt an welches Menü oder Spiel im Moment offen ist

int StartAniD; //Delay zwischen den einzelnen Pixeln
int hueVeraenderung = 50; //Veränderung der Farbe, der Spirale/Buchstaben -> Höher -> Schneller
int schlangex, schlangey; //Position der einzelnen Pixel der spirale

//Variablen Auswahlmenü Animationen--------------------------------
//Ping Pong
int ppa1 = 15;
int ppa2 = 15;
int ppa3 = 12;
int ppa4 = 16;
int zahlerVA = 5;
int zahlerVA2 = 8;

//Ping Pong -------------------------------------------------------
int zahlerx = 10;
int hoehe1 = 15;
int hoehe2 = -15;
int bx, by, richtungStart, score1, score2;

//Snake ----------------------------------------------------------
int score, scoreschneller, ax, ay, posx, posy, zaehler;
int rs = 3;
int speicherx[100];
int speichery[100];
int t = 100;
int startscore = 5;
bool indS = true;
bool indS2 = false;

//YoshisQuest ----------------------------------------------------


//Sounds----------------------------------------------------------
bool Musik = true;
bool indMusik = true;
unsigned long laenge1;
unsigned long laenge2;

//Menue-----------------------------------------------------------
int MenueBuchstabe[10];
int MenueBuchstabenMenge = 1;
int MenueBuchstabenAuswahl;
int MenuePosX = 27;
int MenueBuchstabePosX[10];
int MenuePfeilPosX = 27;
int MenueAuswahl = 0;
int MenuePlusFarbe = w;
int MenuePfeilFarbe = cu;
int MenueNaechsterB = 4;
int MenueAuswahlGsmt;
int MenuePlatzhalter = 1;
int MenuePlatzhalter1 = 15;
int MenuePlatzhalterZahl, MenuePlatzhalterZahl1;
unsigned long MenueZeit1;
unsigned long MenueZeit2;
bool MenueInd = true;
bool MenueInd1 = true;
bool MenueInd2 = true;

//-------------------------------------------------------------------------------------------------------

//Abschnitt 4--------------------------------------------------------------------------------------------
void setup() {

  BuchstabenDek();

  for (int i = 0; i <= 4; i++) {
    ind[i] = true;
  }

  ind[3] = false;

  Serial.begin(9600);
  mySerial.begin (9600);
  mp3_set_serial (mySerial);
  matrix->begin();
  dht.begin();
  nunchuk_initialisieren();
  randomSeed(analogRead(A10));

  for (int i = 0; i < 2; i++) { // Um erste Fehlermessungen auszulöschen
    nunchuck();
  }

  MenueBuchstabePosX[0] = 27;

  delay(2000);
  mp3_stop();
  Temp = dht.readTemperature();

}
//-------------------------------------------------------------------------------------------------------

//Abschnitt 5--------------------------------------------------------------------------------------------
//Im der Loop wird alles kombiniert. Hier wird der Wert des Indikators getestet um herauszufinden welches Menü im Momtent benutz wird.
void loop() {

  nunchuck();

  //change_nunchuck();

  if (z1 == 1 && indikator == 0) {

    StartAni();
    indikator = 1;

  }
  else if (indikator == 0) {

    OpenScreen();

  }
  else if (indikator == 1) {

    LDelay();

    Layout();
    PlayMusik(4, 146);

  }
  else if (indikator == 2) {

    StopMusik();
    Musik = true;
    PingPong();

  }
  else if (indikator == 3) {

    if (indS2 == false) {

      menue();

    }
    else {

      StopMusik();
      Snake();

    }

  }

  matrix->show();
  matrix->clear();

}
//-------------------------------------------------------------------------------------------------------

//Abschnitt 6--------------------------------------------------------------------------------------------
//Layout Funktionen---------------------------------------------------------------------------
//Die einzelnen Komponenten des App-Menues werden hier zusammengesetzt.
void Layout() {

  if (Auswl == 1) {
    Controller(3, matrix->Color333(0, 0, 15));
    Controller(59, matrix->Color333(15, 0, 0));
  }
  else {
    Controller(59, matrix->Color333(15, 0, 0));
  }

  Buchstabe(BS[18], 8, 1, w); Buchstabe(BS[15], 12, 1, w); Buchstabe(BS[8], 16, 1, w); Buchstabe(BS[4], 20, 1, w); Buchstabe(BS[11], 24, 1, w); Buchstabe(BS[14], 28, 1, w); //Spielo

  Buchstabe(BS[12], 32, 1, w); Buchstabe(BS[0], 38, 1, w); Buchstabe(BS[19], 42, 1, w); Buchstabe(BS[17], 46, 1, w); Buchstabe(BS[8], 50, 1, w); Buchstabe(BS[23], 54, 1, w); //Matrix

  Uhrzeit();

  Doppelpunkte();

  FarbeFade(7);

  FarbeFade(25);

  Auswahl();

  Temperatur();

  Pfeil(3, true, feldfarbe[0]);

  Rect(5, feldfarbe[1]);

  Rect(24, feldfarbe[2]);

  Rect(43, feldfarbe[3]);

  Pfeil(60, false, feldfarbe[4]);

  spiel_starten();

}

//Bevor einem großen Screenwechsel sollte erstmal alles Dunkel gemacht werden da sonst Pixelfehler entstehen können
void LDelay() {

  if (ind[4] == true) {
    matrix->fillScreen(matrix->Color333(0, 0, 0));
    matrix->show();
    ind[4] = false;
  }

}

//Lese die Temperatur und teile sie in die einzelnen Ziffern auf
void Temperatur() {

  int T1, T2;

  if (wahldelay(300000) == 1) {

    Temp = dht.readTemperature();

    while (Temp == 0) {
      Temp = dht.readTemperature();
    }

  }

  T1 = Temp / 10;
  T2 = Temp % 10;

  Buchstabe(Bzahl[T1], 5, 27, cu);
  Buchstabe(Bzahl[T2], 9, 27, cu);

  matrix->drawRect(13, 27, 2, 2, cu);

}

//Test ob ein Spiel mit Z ein Spiel gestartet werden soll
//Die Animationen der Spiele im Menue
void spiel_starten() {
  if (Auswl == 1 && z1 == 1) {
    indikator = 2;
    delay(500);
  }
  else if (Auswl == 2 && z1 == 1) {
    indikator = 3;
  }
}

//Wechsel zwischen den App-Auswahlfenstern mit "Delay"
void Auswahl() {

  if (x1 > 188 && Auswl < 4 ) {
    if ( u < 4) {
      u++;
      if ( u == 4 ) {
        Auswl++;
        u = 0;
      }
    }
  }
  else if (x1 < 80 && Auswl > 0) {
    if ( u < 4) {
      u++;
      if ( u == 4 ) {
        Auswl--;
        u = 0;
      }
    }
  }
  for (int i = 0; i <= 4; i++) {

    feldfarbe[i] = matrix->Color333(15, 15, 15);

  }

  feldfarbe[Auswl] = matrix->Color333(15, 1, 1);


  //Ping Pong----------------------------------

  matrix->drawLine(7, ppa1, 7, (ppa1 + 2), matrix->Color333(0, 0, 15));

  matrix->drawPixel(ppa3, ppa4, w);

  matrix->drawLine(17, ppa2, 17, (ppa2 + 2), matrix->Color333(15, 0, 0));

  if (Auswl == 1) {

    if (zahlerVA <= 7) {

      ppa2--;
      ppa1++;
      zahlerVA++;

    }
    else {
      ppa2++;
      ppa1--;
      if (ppa1 == 10) {

        zahlerVA = 0;

      }
    }

    if (zahlerVA2 <= 7) {

      ppa3--;

      zahlerVA2++;

    }
    else {

      ppa3++;

      if (ppa3 == 16) {
        zahlerVA2 = 0;
      }

    }

  }
  else {

    ppa1 = 15;
    ppa2 = 15;
    ppa3 = 12;
    ppa4 = 16;
    zahlerVA = 5;
    zahlerVA2 = 7;

  }
  //------------------------------------------------------------

  //Snake animation---------------------------------------------

  matrix->drawLine(29, 13, 33, 13, matrix->Color333(0, 15, 0)); matrix->drawPixel(33, 14, matrix->Color333(0, 15, 0));

  matrix->drawPixel(33, 17, matrix->Color333(15, 0, 0));

  //------------------------------------------------------------

  //Dino Game---------------------------------------------------



  //------------------------------------------------------------
}

//Vierecke im Menue
void Rect(int xr, int farbe) {
  matrix->drawRect(xr, 9, 15, 15, farbe);
}

//Die Linie wird Pixel für Pixel nacheinander gezeichnet mit einer neuen Farbe.
void FarbeFade(int fy) {

  int lx;

  for (int i = 0; i <= 63; i++) {

    matrix->drawPixel(lx, fy, matrix->ColorHSV(farbeAenderung, 200, 200, true));
    lx++;
    farbeAenderung += 2;

  }

  lx = 0;

}

//Der erste Screen wenn man die Konsole startet
void OpenScreen() {

  matrix->setCursor(5, 4);
  matrix->println("DRUECKE Z");
  matrix->setCursor(17, 13);
  matrix->println("UM ZU");

  if (ind[1] == true) {
    matrix->drawRect(0, 0, 64, 32, matrix->Color333(15, 0, 0));
    matrix->setCursor(11, 22);
    matrix->println("STARTEN");
    ind[1] = false;
    delay(500);
  }
  else {
    matrix->drawRect(0, 0, 64, 32, matrix->Color333(0, 0, 15));
    ind[1] = true;
    delay(500);
  }


}

//Die Animation wenn man die Konsole startet. Spirale + Text
void StartAni() {

  int kleinerx = 63;
  int kleinery = 31;
  int ts = 19;
  int tm = 15;

  while (kleinery >= 11) {
    for (int i = 0; i < kleinerx; i++) {

      matrix->drawPixel(schlangex, schlangey, matrix->ColorHSV(farbeAenderung, 200, 150, true));
      matrix->show();
      delay(StartAniD);

      if (ind[0] == true) {
        schlangex++;
      }
      else {
        schlangex--;
      }

      farbeAenderung += hueVeraenderung;

    }

    for (int i = 0; i < kleinery; i++) {

      matrix->drawPixel(schlangex, schlangey, matrix->ColorHSV(farbeAenderung, 200, 150, true));
      matrix->show();
      delay(StartAniD);

      if (ind[0] == true) {
        schlangey++;
      }
      else {
        schlangey--;
      }

      farbeAenderung += hueVeraenderung;

    }

    if (ind[0] == true) {
      ind[0] = false;
    }
    else {
      ind[0] = true;
    }

    kleinerx -= 2;
    kleinery -= 2;

  }

  mp3_play (1);

  StartAniD = 10;
  hueVeraenderung = 8;

  Text(ts, 14, 0, 0); Text((ts - 1), 13, 1, 0); Text((ts - 3), 14, 1, 2); Text((ts - 2), 16, 1, 1); Text(ts, 17, 1, 2); Text((ts - 1), 19, 1, 0); Text((ts - 3), 18, 0, 0); //S

  Text((ts + 4), 16, 1, 1); Text((ts + 6), 15, 1, 3); Text((ts + 5), 13, 1, 0); Text((ts + 3), 13, 6, 2); //P

  Text((ts + 10), 13, 6, 2); Text((ts + 9), 13, 2, 1); Text((ts + 9), 19, 2, 1); //I

  Text((ts + 14), 13, 6, 2); Text((ts + 15), 13, 2, 1); Text((ts + 15), 16, 1, 1); Text((ts + 15), 19, 2, 1); //E

  Text((ts + 20), 13, 6, 2); Text((ts + 21), 19, 2, 1); //L

  Text((ts + 28), 13, 1, 0); Text((ts + 26), 14, 4, 2); Text((ts + 27), 19, 1, 1); Text((ts + 29), 18, 4, 3); //O

  delay(1000);

  matrix->fillRect(15, 12, 35, 9, matrix->Color333(0, 0, 0));
  matrix->show();

  Text(tm, 19, 6, 3); Text((tm + 1), 14, 0, 0); Text((tm + 2), 15, 0, 0); Text((tm + 3), 14, 0, 0); Text((tm + 4), 13, 6, 2); //M

  Text((tm + 7), 19, 5, 3); Text((tm + 8), 13, 1, 1); Text((tm + 10), 14, 5, 2); Text((tm + 8), 15, 1, 1); //A

  Text((tm + 15), 13, 6, 2); Text((tm + 13), 13, 4, 1); //T

  Text((tm + 20), 19, 6, 3); Text((tm + 21), 13, 1, 1); Text((tm + 23), 14, 1, 2); Text((tm + 22), 16, 1, 0); Text((tm + 23), 17, 2, 2); //R

  Text((tm + 27), 13, 6, 2); Text((tm + 26), 13, 2, 1); Text((tm + 26), 19, 2, 1); //I

  Text((tm + 31), 19, 2, 3); Text((tm + 32), 16, 1, 1); Text((tm + 34), 15, 2, 3); Text((tm + 31), 13, 2, 2); Text((tm + 34), 17, 2, 2); //X

  delay(2000);

  mp3_stop ();

}

//Vereinfachung der Animation durchs zeichnen von Linien mit einer Richtung
void Text(int x, int y, int laenge, int richtung) {

  for (int i = 0; i <= laenge; i++) {
    matrix->drawPixel(x, y, matrix->ColorHSV(farbeAenderung, 200, 255, true));
    matrix->show();
    farbeAenderung += hueVeraenderung;
    delay(StartAniD);

    switch (richtung) {

      case 0:
        x--;                  //Links
        break;
      case 1:
        x++;                  //Rechts
        break;
      case 2:
        y++;                  //Unten
        break;
      case 3:
        y--;                  //Oben
        break;

    }

  }

}

//Das kleine Symbol in den Ecken was anzeigt ob ein Spiel multi oder singel - player ist
void Controller(int cx, int cfarbe) {

  matrix->drawLine(cx, 1, (cx + 2), 1, cfarbe);

  matrix->fillRect(cx, 3, 3, 3, cfarbe);

}

//Vereinfachte zeichnung den Pfeils links und rechts für mögliche erweiterungen
void Pfeil(int pxx, bool richtung, int farbe) {

  int py = 13;

  if (richtung == true) {

    for (int i = pxx; i >= (pxx - 3); i--) {
      matrix->drawPixel(i, py, farbe);
      py++;
    }

    for (int i = (pxx - 2); i <= pxx; i++) {
      matrix->drawPixel(i, py, farbe);
      py++;
    }

  }
  else {

    for (int i = pxx; i <= (pxx + 3); i++) {
      matrix->drawPixel(i, py, farbe);
      py++;
    }

    for (int i = (pxx + 2); i >= pxx; i--) {
      matrix->drawPixel(i, py, farbe);
      py++;
    }

  }

}

/*Da die vorhandene Fuktionen nur Buchstaben mit einer Größe von 8*5px unterstützt und das zu groß ist da ich am besten 5*3 oder auch 5*5 brauche wird hier ein Algorithmus
  erstellt der eine Reihenfolge von Binären Zahlen bekommt und dann in die Buchstaben umwandelt.
  So muss man nicht jeden Pixel einzelnt zeichnen und die Buchstaben/Zeichen die Vorhanden sind können schnell erweitert werden.*/

void Buchstabe(int Buchstabe, int a, int b, int farbe) {

  int zr[25]; //Zwischenspeicher der Bitpositionen zum späteren zeichnen
  int ZS;  //Erweiterung für breitere Buchstaben (5*5)
  int zahler;
  int BuchstabePosX = a;
  int BuchstabenPosY = b;
  int n = 14;
  int laenge;
  int breite;

  if (Buchstabe == BS[6] || Buchstabe == BS[12] || Buchstabe == BS[13] || Buchstabe == BS[16] || Buchstabe == BS[22]) {

    if (Buchstabe == BS[6]) {
      ZS = BSBackup[0];
    }
    else if (Buchstabe == BS[12]) {
      ZS = BSBackup[1];
    }
    else if (Buchstabe == BS[13]) {
      ZS = BSBackup[2];
    }
    else if (Buchstabe == BS[16]) {
      ZS = BSBackup[3];
    }
    else if (Buchstabe == BS[22]) {
      ZS = BSBackup[4];
    }

    breite = 4;

  }
  else {

    laenge = 15;
    breite = 2;

  }

  for (int i = 0; i < laenge; i++) {

    zr[i] = bitRead(Buchstabe, n);
    n--;

  }

  if (breite == 4) {

    n = 9;
    laenge = 25;

    for (int i = 15; i < laenge; i++) {

      zr[i] = bitRead(ZS, n);
      n--;

    }

  }

  while (zahler < laenge) {
    if (zr[zahler] == 1) {
      matrix->drawPixel(BuchstabePosX, BuchstabenPosY, farbe);
    }
    zahler++;
    if (BuchstabePosX != (breite + a)) {
      BuchstabePosX++;
    }
    else {
      BuchstabenPosY++;
      BuchstabePosX = a;
    }

  }

  zahler = 0;

}

//Lese die Uhrzeit Seriell aus (Daten vom ESP) und wandel sie in die einzelnen Werte um
void Uhrzeit() {

  byte stunden, minuten;
  byte minutenWert1, minutenWert2, stundenWert1, stundenWert2;

  //Lese die Serielle Konsole
  while (Serial.available() > 0) {

    e = Serial.read();

    if (e == 'A') {

      while (Serial.available () <= 0);
      e = Serial.read();

      if (e == ',')
      {
        stunden = Serial.parseInt();
        minuten = Serial.parseInt();
      }

    }

  }

  //Teile die Uhrzeit in die einzelenen Ziffern ein um sie zu zeichnen
  stundenWert1 = stunden / 10;
  stundenWert2 = stunden % 10;
  minutenWert1 = minuten / 10;
  minutenWert2 = minuten % 10;

  //Zeichne die Uhrzeit
  Buchstabe(Bzahl[1], UhrzeitPos, 27, cu); Buchstabe(Bzahl[stundenWert2], (UhrzeitPos + 4), 27, cu); //Stunden

  Buchstabe(Bzahl[minutenWert1], (UhrzeitPos + 10), 27, cu); Buchstabe(Bzahl[minutenWert2], (UhrzeitPos + 14), 27, cu);// Minuten

}

//Blinkender Doppelpunkt zwischen den Uhrzeiten
void Doppelpunkte() {

  if ( z < 10 && ind[2] == true) {
    matrix->drawPixel( (UhrzeitPos + 8), 30, cu); matrix->drawPixel( (UhrzeitPos + 8), 28, cu);
    z++;
    if ( z == 10 ) {
      ind[2] = false;
    }
  }
  else {
    matrix->drawPixel( (UhrzeitPos + 8), 30, matrix->Color333(0, 0, 0)); matrix->drawPixel( (UhrzeitPos + 8), 28, matrix->Color333(0, 0, 0));

    if (z > 0) {
      z--;
    }
    else {
      ind[2] = true;
      z = 0;
    }

  }
}

//Deklariere die Binäre Reihenfolge der Buchstaben/Zeichen/Zahlen. Wird im Setup ausgeführt
void BuchstabenDek() {

  BS[0] = 0b010101111101101; BS[1] = 0b110101111101110; BS[2] = 0b111100100100111; BS[3] = 0b110101101101110; BS[4] = 0b111100111100111; BS[5] = 0b111100110100100;  BS[6] = 0b111111000010111; BS[7] = 0b101101111101101;
  BS[8] = 0b111010010010111; BS[9] = 0b111001001101111; BS[10] = 0b101101110101101; BS[11] = 0b100100100100111; BS[12] = 0b100011101110101; BS[13] = 0b100011100110101; BS[14] = 0b111101101101111; BS[15] = 0b111101111100100;
  BS[16] = 0b111111000110001; BS[17] = 0b111101110101101; BS[18] = 0b111100111001111; BS[19] = 0b111010010010010; BS[20] = 0b101101101101111; BS[21] = 0b101101101101010; BS[22] = 0b100011000110101; BS[23] = 0b101101010101101;
  BS[24] = 0b101101010010010; BS[25] = 0b111001010100111;

  BSBackup[0] = 0b1000111111; BSBackup[1] = 0b1000110001; BSBackup[2] = 0b1001110001; BSBackup[3] = 0b1001111111; BSBackup[4] = 0b1010101010;

  Bzahl[0] = 0b111101101101111; Bzahl[1] = 0b010110010010111; Bzahl[2] = 0b111001111100111; Bzahl[3] = 0b111001111001111; Bzahl[4] = 0b101101111001001;
  Bzahl[5] = 0b111100111001111; Bzahl[6] = 0b111100111101111; Bzahl[7] = 0b111001001001001; Bzahl[8] = 0b111101111101111; Bzahl[9] = 0b111101111001111;

  Bzeichen[0] = 0b000010000010000; Bzeichen[1] = 0b000010111010000; Bzeichen[2] = 0b000010100010000; Bzeichen[3] = 0b000010001010000;

}

//-------------------------------------------------------------------------------------------------------

//Auswahlfeld--------------------------------------------------------------------------------------------

//Setzte das Menue zusammen
void menue() {

  MenueReset();

  MenueAbfrage();

  MenueName();

  //---------

  if (MenueAuswahlGsmt == 1) {

    if (x1 > 170 && MenuePlatzhalter < 1) {

      MenuePlatzhalter += wahldelay(200);

    }
    else if (x1 < 80 && MenuePlatzhalter > 1) {

      MenuePlatzhalter -= wahldelay(200);

    }

    Buchstabe(Bzeichen[2], 39, 14, cu);
    Buchstabe(Bzeichen[3], 47, 14, cu);

  }

  Buchstabe(BS[11], 4, 14, w); Buchstabe(BS[4], 8, 14, w); Buchstabe(BS[21], 12, 14, w); Buchstabe(BS[4], 16, 14, w); Buchstabe(BS[11], 20, 14, w); Buchstabe(Bzeichen[0], 23, 14, w);

  Buchstabe(Bzahl[MenuePlatzhalter], 43, 14, w);

  //---------

  //---------

  if (MenueAuswahlGsmt == 2) {

    if (x1 > 170 && MenuePlatzhalter1 < 30) {

      MenuePlatzhalter1 += wahldelay(200);

    }
    else if (x1 < 80 && MenuePlatzhalter1 > 0) {

      MenuePlatzhalter1 -= wahldelay(200);

    }

  }

  Buchstabe(BS[15], 4, 23, w); Buchstabe(BS[4], 8, 23, w); Buchstabe(BS[6], 12, 23, w); Buchstabe(BS[4], 18, 23, w); Buchstabe(BS[11], 22, 23, w); Buchstabe(Bzeichen[0], 25, 23, w);

  if (MenuePlatzhalter1 <= 9) {

    Buchstabe(Bzahl[MenuePlatzhalter1], 43, 23, w);

    if (MenueAuswahlGsmt == 2) {

      Buchstabe(Bzeichen[2], 39, 23, cu);
      Buchstabe(Bzeichen[3], 47, 23, cu);

    }

  }
  else {

    MenuePlatzhalterZahl = MenuePlatzhalter1 / 10;
    MenuePlatzhalterZahl1 = MenuePlatzhalter1 % 10;

    Buchstabe(Bzahl[MenuePlatzhalterZahl], 40, 23, w);
    Buchstabe(Bzahl[MenuePlatzhalterZahl1], 44, 23, w);

    if (MenueAuswahlGsmt == 2) {

      Buchstabe(Bzeichen[2], 36, 23, cu);
      Buchstabe(Bzeichen[3], 48, 23, cu);

    }

  }

  //---------

  if (MenueAuswahlGsmt == 3) {

    indS2 = true;

  }

}

//Gespeicherte Namen werden durch das Drücken von z1 und nach oben ziehen von y1 resettet
void  MenueReset() {

  if (z1 == true && y1 >= 240) {

    for (int i = 0; i <= 10; i++) {
      MenueBuchstabe[i] = 0;
      MenueBuchstabePosX[i] = 0;
    }

    MenueBuchstabenMenge = 1;
    MenueBuchstabenAuswahl = 0;
    MenuePosX = 27;
    MenuePfeilPosX = 27;
    MenueInd = true;
    MenueInd2 = true;


  }

}

//Nächstes Auswahlmöglichkeit wird abgefragt
void MenueAbfrage() {

  if (c1 == true) {

    MenueAuswahlGsmt += wahldelay(500);

  }

}

//System hinter dem Auswählen des Namens im Auswahlmenue
void MenueName() {

  matrix->drawRect(0, 0, 64, 32, cu);

  Buchstabe(BS[13], 4, 5, w); Buchstabe(BS[0], 10, 5, w); Buchstabe(BS[12], 14, 5, w); Buchstabe(BS[4], 20, 5, w); Buchstabe(Bzeichen[0], 23, 5, w);

  if (x1 > 170 && MenueAuswahl != 1 && MenueInd2 == true && MenueAuswahlGsmt == 0) {

    MenueAuswahl += wahldelay(50);

  }
  else if (x1 < 80 && MenueAuswahl != 0 && MenueAuswahlGsmt == 0) {

    MenueAuswahl -= wahldelay(50);

  }

  if (MenueAuswahl == 0) {

    if (y1 > 170 && MenueBuchstabe[MenueBuchstabenAuswahl] != 0 && MenueAuswahlGsmt == 0) {

      MenueBuchstabe[MenueBuchstabenAuswahl] -= wahldelay(100);

    }
    else if (y1 < 80 && MenueBuchstabe[MenueBuchstabenAuswahl] != 25 && MenueAuswahlGsmt == 0) {

      MenueBuchstabe[MenueBuchstabenAuswahl] += wahldelay(100);

    }

    if (MenueBuchstabe[MenueBuchstabenAuswahl] == 6 || MenueBuchstabe[MenueBuchstabenAuswahl] == 12 || MenueBuchstabe[MenueBuchstabenAuswahl] == 13 || MenueBuchstabe[MenueBuchstabenAuswahl] == 16 || MenueBuchstabe[MenueBuchstabenAuswahl] == 22) {

      if (MenueInd == true) {
        MenuePosX += 2;
        MenuePfeilPosX++;
      }

      MenueInd = false;

    }
    else {

      MenueInd = true;

      if ((MenuePosX + 2) == MenueBuchstabePosX[MenueBuchstabenMenge]) {

        MenuePosX -= 2;
        MenuePfeilPosX--;

      }

    }

    MenuePlusFarbe = w;
    MenuePfeilFarbe = cu;

  }
  else if (MenueAuswahl == 1) {

    MenuePlusFarbe = cu;
    MenuePfeilFarbe = w;

    if (z1 == true && MenueAuswahlGsmt == 0) {

      if (wahldelay(75) == 1) {

        MenuePfeilPosX += 4;
        MenuePosX += 4;
        MenueBuchstabenMenge++;
        MenueAuswahl--;
        MenueBuchstabenAuswahl++;

        if (MenueNaechsterB == 6) {

          MenuePfeilPosX ++;

        }

      }

    }

  }

  MenueBuchstabePosX[0] = 27;

  for (int i = 0; i < MenueBuchstabenMenge; i++) {

    Buchstabe(BS[MenueBuchstabe[i]], MenueBuchstabePosX[i], 5, w);

    if (MenueBuchstabe[i] == 6 || MenueBuchstabe[i] == 12 || MenueBuchstabe[i] == 13 || MenueBuchstabe[i] == 16 || MenueBuchstabe[i] == 22) {

      MenueNaechsterB = 6;

    }
    else {

      MenueNaechsterB = 4;

    }

    MenueBuchstabePosX[(i + 1)] = MenueBuchstabePosX[i] + MenueNaechsterB;

  }

  if (MenueAuswahlGsmt == 0) {
    PfeilKlein(MenuePfeilPosX, 2, true, MenuePfeilFarbe); PfeilKlein(MenuePfeilPosX, 10, false, MenuePfeilFarbe);
  }

  if (MenuePosX <= 54 && MenueAuswahlGsmt == 0) {
    Buchstabe(Bzeichen[1], (MenuePosX + 4), 5, MenuePlusFarbe);
    MenueInd2 = true;
  }
  else {
    MenueInd2 = false;
  }

}

//Buchstaben werden in einem Code an den ESP Seriell übermittelt z.B. B,4,0,1,2,3,5 --> Name: ABCD Score: 5
//Die erste Ziffer zeigt die Länge der folgenden Buchstaben an. Die folgenden Buchstaben werden mit der Position im Alphabet angegeben.
//Zuletzt wird nicht der Score übermittelt.
void SendToESP() {

  Serial.print("B,");

  Serial.print(MenueBuchstabenMenge);

  for (int i = 0; i < MenueBuchstabenMenge; i++) {

    Serial.print(",");
    Serial.print(MenueBuchstabe[i]);

  }

  Serial.print(",");
  Serial.print((score - startscore));
  Serial.print(",");

}

//Diese Funktion gibt nach der gegebenen Zeit in MS eine 1 zurück (sonst 0)
//Sehr praktisch für ein "Delay" ohne die Loop aufzuhalten
int wahldelay(long laenge) {

  if (MenueInd1 == true) {
    MenueZeit1 = millis();
    MenueInd1 = false;
  }

  MenueZeit2 = millis();

  if ((MenueZeit2 - MenueZeit1) >= laenge) {
    MenueInd1 = true;
    return 1;
  }

  return 0;

}

//Kleiner Pfeil des Auswahlmenues
void PfeilKlein(int x, int y, bool richtung, int farbe) {

  matrix->drawPixel((x), (y + 1), farbe);
  matrix->drawPixel((x + 2), (y + 1), farbe);

  if (richtung == true) {

    matrix->drawPixel((x + 1), y, farbe);

  }
  else {

    matrix->drawPixel((x + 1), (y + 2), farbe);

  }

}

//-------------------------------------------------------------------------------------

//Nunchuk auslesen---------------------------------------------------------------------------------------

void nunchuk_initialisieren()
{
  pinMode(nunPin1, OUTPUT);
  pinMode(nunPin2, OUTPUT);
  digitalWrite(nunPin1, HIGH);
  Wire.begin();
  nunchuk_init();
  digitalWrite(nunPin1, LOW);
  digitalWrite(nunPin2, HIGH);
  nunchuk_init();
  digitalWrite(nunPin2, LOW);
  digitalWrite(nunPin1, HIGH);
}

void nunchuck()
{
  if (nunchuk_read())
  {
    if (state == LOW)
    {
      z1 = nunchuk_buttonZ();
      c1 = nunchuk_buttonC();
      x1 = nunchuk_joystickX_raw();
      y1 = nunchuk_joystickY_raw();
    }
    else
    {
      z2 = nunchuk_buttonZ();
      c2 = nunchuk_buttonC();
      x2 = nunchuk_joystickX_raw();
      y2 = nunchuk_joystickY_raw();
    }
  }
}

//Nunchucks wechseln
void change_nunchuck()
{
  if (state == LOW)
  {
    digitalWrite(nunPin1, LOW);
    digitalWrite(nunPin2, HIGH);
    state = HIGH;
  }
  else
  {
    digitalWrite(nunPin1, HIGH);
    digitalWrite(nunPin2, LOW);
    state = LOW;
  }
}

//---------------------------------------------------------


//Ping Pong Funktionen---------------------------------------------------------------------------------------------------

//Hier werden die Ping-Pong funktionen zusammengesetzt
void PingPong()
{
  nunchuck();
  start_ping_pong();
  Eingabeh();
  change_nunchuck();

  Ball();

  Spieler(9, hoehe1, 7, matrix->Color333(0, 0, 3), 1, 13, score1);
  Spieler(matrix->width() - zahlerx, matrix->height() - (hoehe2 + 32), 56, matrix->Color333(3, 0, 0), 58, 13, score2);

  Gewinnen();

  delay(70);
}

//Der Ball wird losgesetzt
void start_ping_pong() {
  if ( z1 == true || z2 == true) {
    ind[3] = true;
  }
}

//Der Ball fliegt in eine bestimmte Richtung die sich ändern kann
void Ball() {

  if (ind[3] == true) {

    matrix->drawPixel(bx, by, matrix->Color333(7, 7, 7));

    BallRichtung(richtungStart);

    if (by == 0) {
      if (richtungStart == 1) {
        richtungStart = 3;
      }
      else if (richtungStart == 2) {
        richtungStart = 0;
      }
    }
    else if (by == 31) {
      if (richtungStart == 3) {
        richtungStart = 1;
      }
      else if (richtungStart == 0) {
        richtungStart = 2;
      }
    }

    SpielerKollision();

    if (bx == 7) {

      Punkt(7, matrix->Color333(0, 7, 0));
      score2++;
      matrix->show();
      delay(300);

      PunktAnimation(matrix->Color333(7, 0, 0), "ROT", 23);

    }
    else if (bx == 56) {

      Punkt(56, matrix->Color333(0, 7, 0));
      score1++;
      matrix->show();
      delay(300);

      PunktAnimation(matrix->Color333(0, 0, 7), "BLAU", 18);

    }

  }

  else {

    if (ind[3] == false) {
      bx = random(30, 34);
      by = random(10, 22);
      richtungStart = random(0, 4);

      matrix->setCursor(18, 8);
      matrix->println("Z ZUM");
      matrix->setCursor(18, 17);
      matrix->print("START");
    }

  }

}

//Teste ob der Ball einen der Spieler berührt und ändere die Richtung entsprechend
void SpielerKollision() {

  for (int i = 0; i < 5; i++) {

    if (hoehe1 + i == by && bx == 9) {

      if (richtungStart == 1) {
        richtungStart = 2;
        mp3_play(2);
      }

      else if (richtungStart == 3) {
        richtungStart = 0;
        mp3_play(2);
      }

    }
    if (hoehe2 - i == -by && bx == 54) {

      if (richtungStart == 2) {
        richtungStart = 1;
        mp3_play(2);
      }

      else if (richtungStart == 0) {
        richtungStart = 3;
        mp3_play(2);
      }

    }

  }

}

//Sag was der Ball mit einer der zugewiesenen Richtung machen soll
void BallRichtung(int r) {

  switch (r) {

    case 0:

      bx++;
      by++;

      break;

    case 1:

      bx--;
      by--;

      break;

    case 2:

      bx++;
      by--;

      break;

    case 3:

      bx--;
      by++;

      break;

    default:

      break;
  }

}

//Bewege die Spieler und setzte das Obere und untere Limit
void Eingabeh() {
  if (y2 < 80 && hoehe1 < 27) {
    hoehe1++;
  }
  else if (y2 > 188 && hoehe1 > 0) {
    hoehe1--;
  }
  if (y1 > 188 && hoehe2 < 0) {
    hoehe2++;
  }
  else if (y1 < 80 && hoehe2 > -27) {
    hoehe2--;
  }
}

void Punkt(int lposxy, int farbe) {

  richtungStart = 4;
  matrix->drawPixel(bx, by, matrix->Color333(0, 0, 0));
  matrix->drawLine(lposxy, 0, lposxy, matrix->height(), farbe);

}

//Der Pop-up Screen sobald ein Punkt gemacht wurde
void PunktAnimation(int farbe, String spieler, int mitte) {

  matrix->clear();

  matrix->setCursor(3, 6);
  matrix->println("Punkt fuer");
  matrix->setCursor(mitte, 19);
  matrix->setTextColor(farbe);
  matrix->print(spieler);

  matrix->drawRect(0, 0, matrix->width(), matrix->height(), matrix->Color333(7, 0, 7));
  matrix->show();
  delay(500);
  matrix->drawRect(0, 0, matrix->width(), matrix->height(), matrix->Color333(7, 7, 0));
  matrix->show();
  delay(500);
  matrix->drawRect(0, 0, matrix->width(), matrix->height(), matrix->Color333(0, 7, 7));
  matrix->show();
  delay(500);

  bx = random(30, 34);
  by = random(10, 22);
  richtungStart = random(0, 4);
  ind[3] = false;
  hoehe1 = 15;
  hoehe2 = -15;
  matrix->setTextColor(matrix->Color333(7, 7, 7));

}

//Zeichne den Spieler an der Pos x y mit der Farbe und zeichne den Score an die Stelle x y
void Spieler(int posx, int posy, int lposxy, int farbe, int cursx, int cursy, int score)
{

  matrix->fillRect(posx, posy, 1, 5, matrix->Color333(7, 7, 7)); //Spieler
  matrix->drawLine(lposxy, 0, lposxy, matrix->height(), farbe); //Linie

  matrix->setCursor(cursx, cursy); //Punkteanzeige Spieler
  matrix->print(score);
}

//Gewinnerscreen der einen im Anschluss zurück zu dem Menuescreen setzt
void Gewinnen()
{
  if (score1 == 2)
  {
    score2 = 0;
    score1 = 0;
    matrix->clear();
    matrix->setCursor(20, 4);
    matrix->setTextColor(matrix->Color333(0, 0, 15));
    matrix->println("BLAU");
    matrix->setTextColor(matrix->Color333(15, 15, 15));
    matrix->setCursor(23, 13);
    matrix->print("HAT");
    matrix->setCursor(8, 21);
    matrix->print("GEWONNEN");
    matrix->show();
    delay(2000);
    indikator = 1;
    state = 0;
  }
  if (score2 == 2)
  {
    score2 = 0;
    score1 = 0;
    matrix->clear();
    matrix->setCursor(23, 4);
    matrix->setTextColor(matrix->Color333(15, 0, 0));
    matrix->println("ROT");
    matrix->setTextColor(matrix->Color333(15, 15, 15));
    matrix->setCursor(23, 13);
    matrix->print("HAT");
    matrix->setCursor(8, 21);
    matrix->print("GEWONNEN");
    matrix->show();
    delay(2000);
    indikator = 1;
    state = 0;
  }
}

//-----------------------------------------------

//Snake Funktionen-------------------------------

void Snake() {

  anfangswerte();

  spielrand();

  uebergeben();

  nunchuck();

  eingabeS();

  richtung(rs);

  kollision();

  begrenzung();

  for (int i = 0; i <= score; i++) {
    koerper(speicherx[i], speichery[i]);
  }

  koerper(posx, posy);

  apfel(ax, ay);

  gegessen();

  matrix->drawPixel(0, 0, matrix->Color333(0, 0, 0));

  schnelligkeit();

  delay(t);
}
//übergabe der Kopfposition an die Körperteile nach den abgelaufenen Bildern 
void uebergeben() {

  speicherx[zaehler] = posx;
  speichery[zaehler] = posy;

  if (zaehler < score) {
    zaehler++;
  }
  else {
    zaehler = 0;
  }

}
//begrenzt den Spielraum und lässt das Game enden 
void begrenzung() {
  if (posx == 15 || posx == 49 || posy == -1 || posy == 32) {
    endscreen();
  }
}
//zeichnet den Spielerand 
void spielrand() {

  matrix->drawLine(15, 0, 15, matrix->height(), matrix->Color333(7, 7, 7));
  matrix->drawLine(49, 0, 49, matrix->height(), matrix->Color333(7, 7, 7));
  matrix->setCursor(4, 12);
  matrix->setTextColor(matrix->Color333(7, 7, 7));
  matrix->print((score - startscore));

}
//die Richtung des Kopfes wird durch den Joystick bestimmt 
void eingabeS() {

  if (y1 < 80 && rs != 2) {
    rs = 1;
  }
  else if (y1 > 188 && rs != 1) {
    rs = 2;
  }
  else if (x1 > 188 && rs != 4) {
    rs = 3;
  }
  else if (x1 < 80 && rs != 3) {
    rs = 4;
  }
  else {                // Rauslöschen
  }
}
//
void koerper(int kx, int ky)
{

  matrix->drawPixel(kx, ky, matrix->Color333(0, 7, 0));

}
//Der Apfel kann heirdurch gezeichnet werden  
void apfel(int x, int y)
{

  matrix->drawPixel(x, y, matrix->Color333(7, 0, 0));

}
//die in eingabeS definierte Richtung entscheidet nun über die Positionsänderung
void richtung(int richtung)
{
  switch (richtung) {
    case 1:
      posy++;
      break;
    case 2:
      posy--;
      break;
    case 3:
      posx++;
      break;
    case 4:
      posx--;
      break;
  }
}
//wenn die Postion des Kopfes gleich der Position des Apfels entspricht wird der Score plus 1 gerechnet und die Position des Apfels neu zugewiesen 
void gegessen()
{
  if (ax == posx && ay == posy) {
    mp3_set_volume (MenuePlatzhalter1);
    mp3_play (3);
    score++;
    ax = random(16, 48);
    ay = random(0, 32);
  }
}
//Kollision abfrage des Kopfes mit dem Körper 
void kollision()
{
  for (int i = 0; i <= score; i++) {
    if (posx == speicherx[i] && posy == speichery[i]) {
      endscreen();
    }
  }
}

// animation des Endscreens und zurücksetzen der Werte 
void endscreen()
{
  matrix->clear();

  nunchuck();
  SendToESP();

  matrix->setTextColor(matrix->Color333(7, 7, 7));
  matrix->setCursor(45, 12);
  matrix->print((score - startscore));
  matrix->setCursor(10, 12);
  matrix->print("Score:");
  matrix->drawRect(0, 0, matrix->width(), matrix->height(), matrix->Color333(7, 0, 0));
  matrix->show();
  delay(500);
  matrix->drawRect(0, 0, matrix->width(), matrix->height(), matrix->Color333(0, 7, 0));
  matrix->show();

  delay(500);

  posx = 26;
  posy = 15;

  for (int i = 0; i <= 100; i++) {

    speicherx[i] = 0;
    speichery[i] = 0;

  }

  score = startscore;
  scoreschneller = score + 1;
  ax = random(16, 48);
  ay = random(0, 32);
  t = 100;
  indS = true;
  rs = 3;
  indMusik = true;
  Musik = true;

  while (z1 == false && c1 == false)
  {

    nunchuck();

    matrix->setCursor(5, 6);
    matrix->print("Again: Z");
    matrix->setCursor(5, 20);
    matrix->print("Home: C");
    matrix->show();
    matrix->clear();
    if (c1 == true)  {
      indikator = 1;
      indS2 = false;
      MenueAuswahlGsmt = 0;
    }

  }
}

//pro Erhöhung des Scores wird die Geschwindigkeit erhöht, durch verkleinern des Delays   
void schnelligkeit()
{
  if (score == scoreschneller && t != 30) {
    t -= 2;
    scoreschneller++;
  }
}
//Die variablen bekommen ihren anfangswert 
void anfangswerte()
{
  if (indS == true) {
    ax = random(16, 48);
    ay = random(0, 32);

    posx = 26;
    posy = 15;

    score = startscore;
    scoreschneller = score + 1;
    indS = false;
  }
}

//-----------------------------------------------

//YoshisQuest Funktionen----------------------------------------------



//--------------------------------------------------------------------

//Musik------------------------------------------

//Spiele einen Song für eine bestimmte Länge und wiederhole ihn danach
void PlayMusik(int song, unsigned long songlaenge) {

  if (Musik == true) {

    mp3_set_volume (10);
    mp3_play (song);
    Musik = false;

    laenge1 = millis();

  }

  laenge2 = millis();

  if ((laenge2 - laenge1) >= (songlaenge * 1000)) {

    Musik = true;

  }

}

//Stopt die Musik bevor ein neues Programm gestartet wird  
void StopMusik() {

  if (indMusik == true) {

    mp3_stop ();
    mp3_set_volume (20);
    indMusik = false;

  }

}

//-------------------------------------------
//-------------------------------------------------------------------------------------------------------
