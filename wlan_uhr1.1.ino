#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "";
const char *password = "";

const long utcOffsetInSeconds = 3600; // Zeitzonen Einstellung

char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
IPAddress server(192, 168, 103, 100);
WiFiClient client;

String Tag;
int Stunden, Minuten, Sekunden;
int a, laenge, score, Neuscore;
int Buchstabe[10];

char ChName[10];

String NeuA = "A";

bool MenueInd1 = true;
unsigned long MenueZeit1;
unsigned long MenueZeit2;

bool MenueInd2 = true;
unsigned long MenueZeit3;
unsigned long MenueZeit4;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  ChName[0] = 65;

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {

  if (wahldelay1(2000) == 1) {

    Uhrzeit();

  }

  if (wahldelay(100) == 1) {

    Empfangen();

    Umwandeln();

    NeuAusgeben();

  }

}

void Senden() {

  Verbinden();

  client.print(String("GET http://ip_hier/connect.php?") +
               ("&spieler=") + NeuA +
               ("&score=") + score +
               " HTTP/1.1\r\n" +
               "Host: " + "ip_hier" + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();

  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

}

void Verbinden() {

  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println("connection failed");
    return;
  }

}

void Umwandeln() {

  for (int i = 0; i<= 10; i++) {

      ChName[i] = 0;
    
  }

  for (int n = 0; n < laenge; n++) {
    for (int i = 0; i <= 25; i++) {
      if (Buchstabe[n] == i) {

        ChName[n] = (65 + i);

      }
    }
  }

}

void NeuAusgeben() {

  String ChName1 = String(ChName);

  if (ChName1 != NeuA || score != Neuscore) {

    NeuA = ChName1;

    Neuscore = score;

    Serial.println(ChName1);
    Serial.println(score);

    Senden();

  }

}

void Empfangen() {

  while (Serial.available() > 0) {

    a = Serial.read();

    if (a == 'B') {

      while (Serial.available () <= 0) {}

      a = Serial.read();

      if (a == ',')
      {
        laenge = Serial.parseInt();

        for (int i = 0; i < laenge; i++) {
          Buchstabe[i] = Serial.parseInt();
        }

        score = Serial.parseInt();

      }

    }

  }

}

void Uhrzeit() {

  timeClient.update();

  Tag = daysOfTheWeek[timeClient.getDay()];
  Stunden = timeClient.getHours();
  Minuten = timeClient.getMinutes();
  Sekunden = timeClient.getSeconds();
  Serial.print("A");
  Serial.print(",");
  Serial.print(Stunden);
  Serial.print(",");
  Serial.print(Minuten);
  Serial.println(",");

}

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

int wahldelay1(long laenge) {

  if (MenueInd2 == true) {
    MenueZeit3 = millis();
    MenueInd2 = false;
  }

  MenueZeit4 = millis();

  if ((MenueZeit4 - MenueZeit3) >= laenge) {
    MenueInd2 = true;
    return 1;
  }

  return 0;

}
