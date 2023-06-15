#include <WiFi.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid     = "Kazachya118A";
const char* password = "874325";
DHT dht(4, DHT22);
 
// Set web server port number to 80
WiFiServer server(80);
 
// Variable to store the HTTP request
String header;
 
// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
 
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
 
void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  print(WiFi.softAP(ssid, password));
 
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
  dht.begin();
}
 
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  float h=dht.readHumidity();
  float t=dht.readTemperature();
 
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv=\"Content-type\" content=\"text/html; initial-scale=1; charset=utf-8\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { height: 100%;  margin: 0; font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("body {background: linear-gradient(45deg, rgba(232,232,232,1) 27%, rgba(0,157,203,1) 100%);}");
            client.println("#cube {width: 60%; height: auto; background: white; color: #0000000; text-align: center; border-radius: 10px;}");
            client.println("</style></head>");
            
            // Web Page Heading
            client.println("<center>");          
            client.println("<div id='cube'>");
            client.println("<body><font size='128'>Казачья 118А</font>");
            client.println("<font size='64'><p>");
            client.println("Влажность: ");
            client.println(h);
            client.println("%");
            client.println("</p>");
            client.println("<p>");
            client.println("Температура: ");
            client.println(t);
            client.println("°C");
            client.println("</p></font>");
            client.println("</div>");
            client.println("</center>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
