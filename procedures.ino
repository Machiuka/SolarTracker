String Ceas(String pagina, char* optiune)
{
  //pagina este adresa de internet de unde ia ora, optiune este o sau m, respectiv returneaza ora sau minutul
  //mesaj este ceea ce returneaza (ora, minut sau eroare)

  String mesaj;
  if (WiFi.status() == WL_CONNECTED) {


    http.begin(pagina); //HTTP

    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //  USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        //code ok=200
        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
       //  uint8_t buff[128] = { 0 };
        char  buff[128] = { 0 };
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        int xx = 0;
       
        while (http.connected() && (len > 0 || len == -1)) {
          xx += 1;
          // get available data size
          size_t size = stream->available();

          if (size) {
            // read up to 128 byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // write it to Serial

//               Serial.write(buff, c);
            
           continut = http.getString();
             hh = continut.substring(0, 2);
        mm = continut.substring(3,5);
 
            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }

        
         

        if (optiune == "o")
        {
          mesaj = hh;
        }
        else
        {
          mesaj =  mm;
        }
      }
    } else {
 
      String eroare = "EROARE DE CONECTARE LA FORMARE.RO";
      mesaj = eroare;
    }

    http.end();
  }
  else
  {
    mesaj = "Nu s-a realizat conexiunea la formare.ro";
  }
  return mesaj;
}
void Resetare()
{
  //se intoarce seara la pozitia 0 - de unde va pleca dimineata

  digitalWrite(rcw, HIGH);
  digitalWrite(rccw, LOW);

  delay(durata);
  digitalWrite(rcw, HIGH);
  digitalWrite(rccw, HIGH);

  Culoare(4);
}

void Verifica()
{
  //Aprinde si stinge becul
  if (ora.toInt() == oraS)
  {
    digitalWrite(relBec, LOW);
    Culoare(6);
  }
  else if (ora.toInt() == oraA)
  {
    digitalWrite(relBec, HIGH);
    Culoare(5);
  }
  // Verifica daca e timpul sa se reseteze
  if (ora.toInt()  == ora4 )
  {
    if (semn == 0) {
      Resetare();

      semn = 1;
    }

  }



}
