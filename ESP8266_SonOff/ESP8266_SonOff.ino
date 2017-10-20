/*
ESP8266 Development Board:

LED pinout left to right:

RGB-LED:
 G = GPIO12
 B = GPIO13
 R = GPIO15

red LEDS:
GPIO16
GPIO14
GPIO05
GPIO04
GPIO00
GPIO02

       FOR SONOFF-MODULES
   WARNING SET FLASH MODE TO DOUT!!

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "TP-LINK";
const char* password = "Das_ist_eine_1a_sichere_Passphrase";


ESP8266WebServer server(80);
WiFiClient telnetToOctoPi;

String webPage = "";

#define USE_DUMMY


#define SONOFF_PIN_LED             13
#define SONOFF_PIN_RELAY           12

#ifdef USE_DUMMY 
#define PIN_RGB_RED                15
#define PIN_RGB_GREEN              12
#define PIN_RGB_BLUE               13
#endif // USE_DUMMY


#define POWER_OFF_DELAY_SECONDS   120

void powerOn(void)
{
#ifdef USE_DUMMY 
    analogWrite(PIN_RGB_RED, 0);
//    analogWrite(PIN_RGB_GREEN, 255);
#else
    digitalWrite(SONOFF_PIN_LED, LOW);
    digitalWrite(SONOFF_PIN_RELAY, HIGH); 
#endif // USE_DUMMY
}


void powerOff(void)
{
#ifdef USE_DUMMY
//    analogWrite(PIN_RGB_GREEN, 0);
    analogWrite(PIN_RGB_RED, 255);
#else
    digitalWrite(SONOFF_PIN_LED, HIGH);
    digitalWrite(SONOFF_PIN_RELAY, LOW);
#endif // USE_DUMMY
}


void setupPage(void)
{
    String settingsPage = "";

    settingsPage += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
    settingsPage += "<html>";
    settingsPage += "<head>";
    settingsPage += "<meta charset=\"utf-8\">";
    settingsPage += "<title>Setup</title>";
    settingsPage += "<style>";
    settingsPage += "input[type=textdate] { width: 200px; }";
    settingsPage += "</style>";
    settingsPage += "</head>";
    settingsPage += "<body bgcolor=\"#D4C9C9\" text=\"#000000\" link=\"#1E90FF\" vlink=\"#0000FF\">";
    settingsPage += "<h2 align=\"center\"><strong>Poweroff Server Setup</strong></h2>";
    settingsPage += "<form action=\"/\" method=\"get\" enctype=\"text/plain\" />";
    settingsPage += "<table border align=\"center\" />";
    settingsPage += "<tr>";
    settingsPage += "<td align=\"center\" />";
    settingsPage += "<label>IP des OctoPi</label>";
    settingsPage += "<input type=\"text\" name=\"OctoPiIP\" value=\"192.168.1.125\" style=\"width: 110px;\" maxlength=15 />";
    settingsPage += "<br>";
    settingsPage += "</td>";
    settingsPage += "</tr>";

    settingsPage += "<tr>";
    settingsPage += "<td align=\"center\" />";
    settingsPage += "<label>Port des OctoPi</label>";
    settingsPage += "<input type=\"text\" name=\"OctoPiPort\" value=\"1717\" style=\"width: 40px;\" maxlength=5 />  ";
    settingsPage += "<br>";
    settingsPage += "</td>";
    settingsPage += "</tr>";
    settingsPage += "</table";
    settingsPage += "<hr align=\"center\"><br>";
    settingsPage += "  <div align=\"center\">";
    settingsPage += "<input type=\"submit\" name=\"submit\" value=\"Speichern\">";
    settingsPage += "<input type=\"submit\"  name=\"cancel\"  value=\"Abbruch\">";
    settingsPage += "</div>";
    settingsPage += "<hr align=\"center\"><br>";
    settingsPage += "</form>";
    settingsPage += "</body>";

    server.send(200, "text/html", settingsPage);

}


void setup(void)
{
  webPage += "<h1>OCTOREMOTE Web Server</h1><p><a href=\"on\"><button>Einschalten</button></a>&nbsp;<a href=\"off\"><button>Ausschalten</button></a></p>";  
  // preparing GPIOs
  Serial.begin(115200); 
  Serial.println("");
  Serial.print("init pins ...");
  
  pinMode(SONOFF_PIN_LED, OUTPUT); 
  pinMode(SONOFF_PIN_RELAY, OUTPUT);
#ifdef USE_DUMMY  
  pinMode(PIN_RGB_RED, OUTPUT);
//  pinMode(PIN_RGB_GREEN, OUTPUT);  
//  pinMode(PIN_RGB_BLUE, OUTPUT);  
#endif // USE_DUMMY
  
  powerOn();

  Serial.println("done!");

//  WiFi.mode(WIFI_STA);
  delay(5000);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/setup", setupPage);

  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/on", [](){
    server.send(200, "text/html", webPage);
    powerOn();  
//    digitalWrite(SONOFF_PIN_LED, LOW);
//    digitalWrite(SONOFF_PIN_RELAY, HIGH);
    delay(1000);
  });
  server.on("/off", [](){
    server.send(200, "text/html", webPage);
    powerOff();
//    digitalWrite(SONOFF_PIN_LED, HIGH);
//    digitalWrite(SONOFF_PIN_RELAY, LOW);
    delay(1000); 
  });
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 

/*
char *pIPAddressOctoPi = "192.168.1.125";
short PortOctoPi = 1717;
telnetToOctoPi

WiFiClient telnetToOctoPi;
telnetToOctoPi.stop();


IPAddress targetServer;
targetServer.fromString(pIPAddressOctoPi);

telnetToOctoPi.connect(targetServer, PortOctoPi);

telnetToOctoPi.connected()

while(telnetToOctoPi.available()) 

telnetToOctoPi.read();

telnetToOctoPi.write(sbuf, len);

telnetToOctoPi.close();







void setupPage()
{

  int i;
  IPAddress localIP;

  pageContent = "";
  localIP = WiFi.localIP();

#ifdef DO_LOG
  if ( !beQuiet )
  {
    Logger.Log(LOGLEVEL_DEBUG, (const char*) "handleIndexPage\n");
  }
#endif // DO_LOG

  for (i = 0; i < server.args(); i++ )
  {
#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log(LOGLEVEL_DEBUG, (const char*) "%s = %s\n", server.argName(i).c_str(), server.arg(i).c_str() );
    }
#endif // DO_LOG
  }

  if ( server.method() == SERVER_METHOD_POST )
    //        server.hasArg(INDEX_BUTTONNAME_ADMIN)  )
  {
#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log(LOGLEVEL_DEBUG, (const char*) "POST REQUEST\n");
    }
#endif // DO_LOG
  }
  else
  {
#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log(LOGLEVEL_DEBUG, (const char*) "GET REQUEST\n");
    }
#endif // DO_LOG
  }

  if ( server.hasArg("submit") && server.arg("submit").equalsIgnoreCase("speichern") )

  {
    // for( i = 0; i < MAX_ACTION_TABLE_LINES; i++ )
    for ( i = 0; i < CONNECTED_RELAIS; i++ )
    {

      formFieldName[KW_IDX_BEZEICHNER] = String("bezeichner") + String(i+1);
      formFieldName[KW_IDX_ENABLED_1]  = String("enabled1_")  + String(i+1);
      formFieldName[KW_IDX_HFROM_1]    = String("hfrom1_")    + String(i+1);
      formFieldName[KW_IDX_MFROM_1]    = String("mfrom1_")    + String(i+1);
      formFieldName[KW_IDX_HTO_1]      = String("hto1_")      + String(i+1);
      formFieldName[KW_IDX_MTO_1]      = String("mto1_")      + String(i+1);
      formFieldName[KW_IDX_ENABLED_2]  = String("enabled2_")  + String(i+1);
      formFieldName[KW_IDX_HFROM_2]    = String("hfrom2_")    + String(i+1);
      formFieldName[KW_IDX_MFROM_2]    = String("mfrom2_")    + String(i+1);
      formFieldName[KW_IDX_HTO_2]      = String("hto2_")      + String(i+1);
      formFieldName[KW_IDX_MTO_2]      = String("mto2_")      + String(i+1);
      formFieldName[KW_IDX_EXT_1]      = String("ext1_")      + String(i+1);
      formFieldName[KW_IDX_EXT_2]      = String("ext2_")      + String(i+1);
      formFieldName[KW_IDX_MODE]       = String("mode")       + String(i+1);


      tblEntry[i].name         = server.arg(formFieldName[KW_IDX_BEZEICHNER]);

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].name         = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_BEZEICHNER].c_str(), server.arg(formFieldName[KW_IDX_BEZEICHNER]).c_str());
    }
#endif // DO_LOG

      tblEntry[i].mode         = server.arg(formFieldName[KW_IDX_MODE]);

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].mode         = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_MODE].c_str(), server.arg(formFieldName[KW_IDX_MODE]).c_str());
    }
#endif // DO_LOG

      if( isValidHour( server.arg(formFieldName[KW_IDX_HFROM_1]).c_str()) )
      {
          tblEntry[i].hourFrom_1   = server.arg(formFieldName[KW_IDX_HFROM_1]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*)"tblEntry[%d].hourFrom_1    = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_HFROM_1].c_str(), server.arg(formFieldName[KW_IDX_HFROM_1]).c_str());
    }
#endif // DO_LOG

      if( isValidMinute(server.arg(formFieldName[KW_IDX_MFROM_1]).c_str()) )
      {
          tblEntry[i].minuteFrom_1 = server.arg(formFieldName[KW_IDX_MFROM_1]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].minuteFrom_1 = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_MFROM_1].c_str(), server.arg(formFieldName[KW_IDX_MFROM_1]).c_str());
    }
#endif // DO_LOG

      if( isValidHour( server.arg(formFieldName[KW_IDX_HTO_1]).c_str()) )
      {
          tblEntry[i].hourTo_1     = server.arg(formFieldName[KW_IDX_HTO_1]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*)"tblEntry[%d].hourTo_1      = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_HTO_1].c_str(), server.arg(formFieldName[KW_IDX_HTO_1]).c_str());
    }
#endif // DO_LOG

      if( isValidMinute( server.arg(formFieldName[KW_IDX_MTO_1]).c_str()) )
      {
          tblEntry[i].minuteTo_1   = server.arg(formFieldName[KW_IDX_MTO_1]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].minuteTo_1   = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_MTO_1].c_str(), server.arg(formFieldName[KW_IDX_MTO_1]).c_str());
    }
#endif // DO_LOG

      if( isValidHour( server.arg(formFieldName[KW_IDX_HFROM_2]).c_str()) )
      {
          tblEntry[i].hourFrom_2   = server.arg(formFieldName[KW_IDX_HFROM_2]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].hourFrom_2   = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_HFROM_2].c_str(), server.arg(formFieldName[KW_IDX_HFROM_2]).c_str());
    }
#endif // DO_LOG

      if( isValidMinute( server.arg(formFieldName[KW_IDX_MFROM_2]).c_str()) )
      {
          tblEntry[i].minuteFrom_2 = server.arg(formFieldName[KW_IDX_MFROM_2]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].minuteFrom_2 = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_MFROM_2].c_str(), server.arg(formFieldName[KW_IDX_MFROM_2]).c_str());
    }
#endif // DO_LOG

      if( isValidHour( server.arg(formFieldName[KW_IDX_HTO_2]).c_str()) )
      {
          tblEntry[i].hourTo_2     = server.arg(formFieldName[KW_IDX_HTO_2]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].hourTo_2     = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_HTO_2].c_str(), server.arg(formFieldName[KW_IDX_HTO_2]).c_str());
    }
#endif // DO_LOG

      if( isValidMinute( server.arg(formFieldName[KW_IDX_MTO_2]).c_str()) )
      {
          tblEntry[i].minuteTo_2   = server.arg(formFieldName[KW_IDX_MTO_2]);
      }

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].minuteTo_2   = server.arg(\"%s\") ->%s\n", 
                  i, formFieldName[KW_IDX_MTO_2].c_str(), server.arg(formFieldName[KW_IDX_MTO_2]).c_str());
    }
#endif // DO_LOG


      if( isValidHour( server.arg(formFieldName[KW_IDX_HFROM_1]).c_str()) &&
          isValidMinute( server.arg(formFieldName[KW_IDX_MFROM_1]).c_str()) &&
          isValidHour( server.arg(formFieldName[KW_IDX_HTO_1]).c_str()) &&
          isValidMinute( server.arg(formFieldName[KW_IDX_MTO_1]).c_str()) )
      {
          tblEntry[i].enabled_1   = 
                      server.arg(formFieldName[KW_IDX_ENABLED_1]).equalsIgnoreCase("aktiv") ? true : false;

#ifdef DO_LOG
        if ( !beQuiet )
        {
          Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].enabled_1   = server.arg(\"%s\") -> %d\n", 
                      i, formFieldName[KW_IDX_ENABLED_1].c_str(), tblEntry[i].enabled_1);
        }
#endif // DO_LOG
      }
      else
      {
          tblEntry[i].enabled_1   = false;
#ifdef DO_LOG
        if ( !beQuiet )
        {
          Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].enabled_1   = FALSE - error in time spec\n", i);
        }
#endif // DO_LOG
      }
      
      tblEntry[i].extEnable_1 = server.arg(formFieldName[KW_IDX_EXT_1]).equalsIgnoreCase("aktiv") ? true : false;

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].extEnable_1 = server.arg(\"%s\") -> %d\n", 
                  i, formFieldName[KW_IDX_EXT_1].c_str(), tblEntry[i].extEnable_1);
    }
#endif // DO_LOG
            

      if( isValidHour( server.arg(formFieldName[KW_IDX_HFROM_2]).c_str()) &&
          isValidMinute( server.arg(formFieldName[KW_IDX_MFROM_2]).c_str()) &&
          isValidHour( server.arg(formFieldName[KW_IDX_HTO_2]).c_str()) &&
          isValidMinute( server.arg(formFieldName[KW_IDX_MTO_2]).c_str()) )
      {
          tblEntry[i].enabled_2   = 
                    server.arg(formFieldName[KW_IDX_ENABLED_2]).equalsIgnoreCase("aktiv") ? true : false;

#ifdef DO_LOG
        if ( !beQuiet )
        {
          Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].enabled_2   = server.arg(\"%s\") -> %d\n", 
                      i, formFieldName[KW_IDX_ENABLED_2].c_str(), tblEntry[i].enabled_2);
        }
#endif // DO_LOG
      }
      else
      {
          Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].enabled_2   = FALSE - error in time spec\n", i);
      }
      
      tblEntry[i].extEnable_2 = server.arg(formFieldName[KW_IDX_EXT_2]).equalsIgnoreCase("aktiv") ? true : false;

#ifdef DO_LOG
    if ( !beQuiet )
    {
      Logger.Log( LOGLEVEL_DEBUG, (const char*) "tblEntry[%d].extEnable_2 = server.arg(\"%s\") -> %d\n", 
                  i, formFieldName[KW_IDX_EXT_2].c_str(), tblEntry[i].extEnable_2);
    }
#endif // DO_LOG
      

    }

    storeActionTable();
    eeprom.validate();

    startupActions();
    // return;

  }

  pageContent += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
  pageContent += "<html>";
  pageContent += "<head>";
  pageContent += "<meta charset=\"utf-8\">";
  pageContent += "<title>Home</title>";

  pageContent += "<style> ";
  pageContent += "input[type=textdate] { width: 1.5em; }  ";
  pageContent += "</style>";

  pageContent += "</head>";

  pageContent += "<body bgcolor=\"#D4C9C9\" text=\"#000000\" link=\"#1E90FF\" vlink=\"#0000FF\">";
  pageContent += "<div align=\"center\"><strong><h1>WiFi Relais</h1></strong></div>";


  pageContent += "<div align=\"center\"><strong><h1>" + String(localIP.toString()) + "</h1></strong></div>";

  pageContent += "<form action=\"/\" method=\"get\">";
  pageContent += "<hr align=\"center\"><br>";
  pageContent += "<table border align=\"center\">";

  // Zeile 1
  pageContent += "<tr>";
  doCreateLine( 1 );
  pageContent += "</tr>";

  // Zeile 2
  pageContent += "<tr>";
  doCreateLine( 2 );
  pageContent += "</tr>";

  // Zeile 3
  pageContent += "<tr>";
  doCreateLine( 3 );
  pageContent += "</tr>";

  // Zeile 4
  pageContent += "<tr>";
  doCreateLine( 4 );
  pageContent += "</tr>";

  // Zeile 5
  pageContent += "<tr>";
  doCreateLine( 5 );
  pageContent += "</tr>";

  // Zeile 6
  pageContent += "<tr>";
  doCreateLine( 6 );
  pageContent += "</tr>";

  // Zeile 7
  pageContent += "<tr>";
  doCreateLine( 7 );
  pageContent += "</tr>";

  // Zeile 8
  pageContent += "<tr>";
  doCreateLine( 8 );
  pageContent += "</tr>";

  pageContent += "</table>";
  pageContent += "<hr align=\"center\">";
  pageContent += "<div align=\"center\">";
  pageContent += "<input type=\"submit\" name=\"submit\" value=\"speichern\">";
  pageContent += "<input type=\"reset\" name=\"reset\" value=\"reset\">";
  pageContent += "<input type=\"submit\" name=\"ESP\" value=\"reboot\">";
  pageContent += "</div>";
  pageContent += "<hr align=\"center\"><br>";
  pageContent += "</form>";
  pageContent += "</body>";
  pageContent += "</html>";

  server.send(200, "text/html", pageContent);

  //  Serial.print(pageContent.c_str());

}





*/

