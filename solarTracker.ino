
/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
  http://chinabiz.ro/arduino/solarTracker.php?Voltaj=18.3&TempInt=3.15&TempOut=-8.7&Amperaj=0.3
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;
unsigned long seconds = 1000L; // !!! SEE THE CAPITAL "L" USED!!!
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;
unsigned long previousMillis = 0;
String timpul = "Is not readed";
String test = " xxxxx";
String ora = " ";
String minut = " ";
int turatie = 750; //pwm of the motor (up to 1023).
int pwm = D8; //pin turatia motor
int rcw = D6; //rev clockwise
int rccw = D7; //rev counter clockwise
int relLight = D5; //control the light relay
int semn; //to know if the operation was made (0 or 1)
int durata; //duratation of an motor revolution
int ora0 = 11; //ora=hour. There are four hours scheduled. The last one is for reset
int ora1 = 13;
int ora2 = 16;
int ora3 = 17;
int ora4 = 20;
int oraS = 6; //bulb light off
int oraA = 21; // bulb light on
int lungIP; //lenght of the IP string. I need it because the IP is dynamic last digit of IP  is changing frequently. Thus the builtin led blink accordingly
//Ex. if the IP is 192.168.0.4, builtin led blink four time. If is 192.168.0.7, builtin led blink seven time etc 
String hh;
String mm;
IPAddress ownIP;//IP-ul
String req;
String continut;
unsigned long freqSvr = 100L;
unsigned long millisSvr = 0;
unsigned long prevmillisSvr = 0;
int ofset = 300; //is not used 
int mmm = 42; //was used for testing
int oprire; //not used
int stopaj; //cand se apasa butonul se roteste invers
const char* ssid = "Your SSID";
const char* password = "Your password";
unsigned long frecventa = 1; // Set the reading frequency from time server
String cul1 = "powderblue";
String cul2 = "powderblue";
String cul3 = "powderblue";
String cul4 = "powderblue";
String cul5 = "powderblue"; //lights on
String cul6 = "powderblue";//lights off
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {

  Serial.begin(115200);
  delay(10);
  durata = 1.5 * seconds; // - is the durstion of motor rotation. 

  pinMode(pwm, OUTPUT);  // pwm from 0 to 1023
  pinMode(rcw, OUTPUT);
  pinMode(rccw, OUTPUT);
  pinMode(relLight, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  analogWrite(pwm, turatie); //the motor speed is controlled through  PWM value (0...1023).
  digitalWrite(rcw, HIGH);
  digitalWrite(rccw, HIGH);
  digitalWrite(relLight, LOW);
  // Connect to WiFi network
  // Serial.println();
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    digitalWrite(LED_BUILTIN, HIGH);

  }

  // Start the server
  server.begin();
  // Serial.println("Server started");
  // Print the IP address
  ownIP = WiFi.localIP();
  int ip4 = ownIP[3];
  Serial.println(ownIP);

  PotrivesteCeas();
  //   req = client.readStringUntil('\r');
  for (int i = 1; i < ip4 + 1; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    // turn the pin off:
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }

}

void loop() {

  millisSvr = millis();
  WiFiClient client = server.available();
  unsigned long currentMillis = millis();

  String req = client.readStringUntil('\r');


  // Match the request from his own webpage
  int val;
  if (req.indexOf("inainte") != -1)
  {
    Rotire(0);
  }
  else if (req.indexOf("inapoi") != -1)
  {
    Rotire(1);
  }
  else if (req.indexOf("ceas") != -1)
    PotrivesteCeas();
  else if (req.indexOf("aprins") != -1)
  {
    digitalWrite(relLight, HIGH);
    Culoare(5);
  }
  else if (req.indexOf("stins") != -1)
  {
    digitalWrite(relLight, LOW);
    Culoare(6);
  }
  else if (req.indexOf("durata=on") != -1)
    durata = durata + 50;
  else if (req.indexOf("turP") != -1)
  {
    if (turatie <= 900)
      turatie += 50;

    analogWrite(pwm, turatie);
  }
  else if (req.indexOf("turM") != -1)
  {
    if (turatie >= 100)
      turatie -= 100;
    analogWrite(pwm, turatie);
  }
  else if (req.indexOf("durata=off") != -1)
  {
    if (durata >= 0)
      durata = durata - 100;
  }
  else {
    // Serial.println(req + " Stop pagina");
    //   client.stop();
    //  return;
  }
  client.flush();


  // Prepare the response
  timpul = ora + ":" + minut;

  // Webpage interface


  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><head><title>Machiuka\"s Solar Tracker v1.0</title>";
  s += "<style>body {    tr:  font-size: 30px;} h1 {    color: maroon;    margin-left: 40px;}</style></head>";
  s += "<body style=\"background-color:powderblue;\"><h1>\r\nMACHIUKA'S SOLAR TRACKER</h1><br> ";
  //s += (val)?"high":"low";
  s += "<h2>Time updated at " + timpul + "<br> Rotation timelapse is <u><i>" + durata + "</i></u> milisec. PWM = " + turatie + "<br>";
  s += "<h2>Lights OFF at  " + String(oraS) + ". Lights ON at " + oraA + "<hr>";
  s += "<h2>ROTATION SCHEDULE</h2>";
  s += "<table><tr><td style=\"background-color:" + cul1 + "\">First at " + String(ora0) + "." + minut + ".</td>";
  s += "<td style=\"background-color:" + cul1 + "\">Second at " + String(ora1) + "." + minut + ".</td>";
  s += "<td style=\"background-color:" + cul2 + "\">Third at " + String(ora2) + "." + minut + ".</td>";
  s += "<td style=\"background-color:" + cul3 + "\">Forth at " + String(ora3) + "." + minut + ".</td>";
  s += "<td style=\"background-color:" + cul4 + "\">Park position at " + String(ora4) + "." + minut + ".</td></tr>";
  s += "<table><tr><td><p><a href=\"?inainte\"><button>NORMAL ROTATION</button></a>&nbsp</p></td>";
  s += "<td><p> <a href=\"?inapoi\"><button>REVERSE ROTATION</button></a>&nbsp</p></td>";
  s += "<td><p><a href=\"?ceas\"><button>UPDATE TIME</button></a>&nbsp</p></td></tr>";
  s += "<tr><td style=\"background-color:" + cul5 + "\"><p> <a href=\"?aprins\"><button>LIGHTS ON</button></a>&nbsp</p></td>";
  s += "<td style=\"background-color:" + cul6 + "\"><p><a href=\"?stins\"><button>LIGHTS OFF</button></a>&nbsp</p><br></td></tr>";
  s += "<tr><td><p> <a href=\"?turP\"><button>PWM ++ </button></a>&nbsp</p></td>";
  s += "<tr><td><p> <a href=\"?turM\"><button>PWM -- </button></a>&nbsp</p></td>";
  s += "<td><p> <a href=\"?durata=on\"><button>INCREMENT TIMELAPSE</button></a>&nbsp</p></td>";
  s += "<td><p><a href=\"?durata=off\"><button>DECREMENT TIMELAPSE</button></a>&nbsp</p><br></td></tr>";
  /*
    s += "<form action=\"?\" method=\"get\">  First name: <input type=\"text\" name=\"fname\"><br>";
    s += "Last name: <input type=\"text\" name=\"lname\"><br>";
    s += "<input type=\"submit\" value=\"Submit\"></form>";
  */
  s += "</h2></table></body></html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  // Serial.println("Client disonnected");

  //if este necesar pentru actualizarea orei
  if (currentMillis - previousMillis >= hours * frecventa) //sau ore*frecventa
  {
    previousMillis = currentMillis;
    PotrivesteCeas();
    semn = 0; //inactiv
  }

  // Serial.println("minutul este :" + minut + "  mm= " + mmm);
 // if (ora.toInt()  == ora0 || ora.toInt()  == ora1 || ora.toInt()  == ora2  || ora.toInt()  == ora3)
   if (ora.toInt()  == ora1 || ora.toInt()  == ora2)
  {
    if (semn == 0)
    {
      Rotire(0);
      semn = 1;
      Colorare();
    }
  }


  Verifica();
}

void Colorare()
{
  if (ora.toInt()  == ora1)
    Culoare(1);
  else if (ora.toInt()  == ora2)
    Culoare(2);
  else if (ora.toInt()  == ora3)
    Culoare(3);
  else if (ora.toInt()  == ora4)
    Culoare(4);
  else if (ora.toInt()  == oraA)
    Culoare(5);
  else if (ora.toInt()  == oraS)
    Culoare(6);
}
void Culoare(int colo)
{
  cul1 = "powderblue";
  cul2 = "powderblue";
  cul3 = "powderblue";
  cul4 = "powderblue";
  if (colo == 1)
  {
    cul1 = "crimson";
    cul2 = "powderblue";
    cul3 = "powderblue";
    cul4 = "powderblue";

  }
  else if (colo == 2)
  {
    cul2 = "crimson";
    cul1 = "powderblue";
    cul3 = "powderblue";
    cul4 = "powderblue";

  }
  else if (colo == 3)
  {
    cul3 = "crimson";
    cul2 = "powderblue";
    cul1 = "powderblue";
    cul4 = "powderblue";

  }
  else if (colo == 4)
  {
    cul4 = "crimson";
    cul2 = "powderblue";
    cul3 = "powderblue";
    cul1 = "powderblue";

  }
  else if (colo == 5)
  {
    cul5 = "crimson";
    cul6 = "powderblue";
  }
  else if (colo == 6)
  {
    cul6 = "crimson";
    cul5 = "powderblue";
  }

}

void Rotire(int rotatie)
{

  
  if (rotatie == 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(rcw, LOW);
    digitalWrite(rccw, HIGH);
    delay(durata);
    digitalWrite(rcw, HIGH);
    digitalWrite(rccw, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);

  }
  else
  {

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(rccw, LOW);
    digitalWrite(rcw, HIGH);
    delay(durata);
    digitalWrite(rcw, HIGH);
    digitalWrite(rccw, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);

  }


}

void PotrivesteCeas()
{
  ora = Ceas("http://formare.ro/ceas.php", "o");
  minut = Ceas("http://formare.ro/ceas.php", "m");
}

