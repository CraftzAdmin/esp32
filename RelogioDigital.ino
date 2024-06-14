  // Relogio digital utilizando um display Led Matrix  e ESP32  - led matrix display 8x32
 // Adição de dois botões nos pinos 34 e 35 para avançar ou retroceder a hora de uma cidade específica.

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LedControl.h>
#include <EEPROM.h>

// LED Matrix Configuration
const int DIN_PIN = 23;   // Data pin connected to the MAX7219 DIN
const int CS_PIN = 5;    // Chip select pin connected to the MAX7219 CS
const int CLK_PIN = 18;   // Clock pin connected to the MAX7219 CLK
const int MATRIX_WIDTH = 8;
const int MATRIX_HEIGHT = 32;

// Button Configuration
const int PREVIOUS_BUTTON_PIN = 34;   // Pin for the previous city button
const int ADVANCE_BUTTON_PIN = 35;    // Pin for the advance city button

// Wi-Fi Configuration
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// NTP Configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSeconds = 3600; // GMT offset in seconds
const int daylightOffsetSeconds = 3600; // Daylight offset in seconds

// City Time Configuration
struct CityTime {
  const char* name;
  const char* timeZone;
  const char* iataCode;
};

CityTime cities[] = {
  {"New York", "America/New_York", "NYC"},
  {"London", "Europe/London", "LON"},
  {"Tokyo", "Asia/Tokyo", "TYO"},
  // Add more cities here
  // ...
  {"Paris", "Europe/Paris", "PAR"},
  {"Sydney", "Australia/Sydney", "SYD"}
};

const int numCities = sizeof(cities) / sizeof(cities[0]);

// LED Matrix Object
LedControl matrix = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

// NTP Client and UDP Object
WiFiUDP udp;
NTPClient timeClient(udp, ntpServer, gmtOffsetSeconds, daylightOffsetSeconds);

// Web Server Object
AsyncWebServer server(80);

// Variables for storing selected cities and parameters
String selectedCities[numCities];
int currentCityIndex = 0;
int refreshInterval = 10; // Refresh interval in seconds (10 seconds per city)

// Button state variables
int previousButtonState = HIGH;
int advanceButtonState = HIGH;
int previousButtonPressTime = 0;
int advanceButtonPressTime = 0;
const int debounceDelay = 50; // Debounce delay in milliseconds

// EEPROM memory address to store configuration
const int eepromAddress = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize LED Matrix
  matrix.shutdown(0, false);
  matrix.setIntensity(0, 8);
  matrix.clearDisplay(0);

  // Initialize NTP Client
  timeClient.begin();

  // Initialize web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/network", HTTP_GET, handleNetwork);
  server.on("/network", HTTP_POST, handleNetworkUpdate);
  server.begin();

  // Load configuration from EEPROM
  loadConfiguration();

  // Set initial selected cities
  for (int i = 0; i < numCities; i++) {
    if (isCitySelected(cities[i].name)) {
      selectedCities[i] = cities[i].name;
    }
  }

  // Attach button interrupts
  pinMode(PREVIOUS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ADVANCE_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PREVIOUS_BUTTON_PIN), previousButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ADVANCE_BUTTON_PIN), advanceButtonISR, FALLING);
}

void loop() {
  // Update the NTP client
  timeClient.update();

  // Get the current city index
  int cityIndex = currentCityIndex % numCities;

  // Get the city name, time zone, and IATA code
  String cityName = selectedCities[cityIndex];
  String timeZone = getTimeZone(cityName);
  String iataCode = getIATACode(cityName);

  // Get the time for the city
  time_t cityTime = timeClient.getEpochTime(timeZone.c_str());

  // Format the time
  struct tm* timeInfo;
  timeInfo = localtime(&cityTime);
  int hour = timeInfo->tm_hour;
  int minute = timeInfo->tm_min;
  int second = timeInfo->tm_sec;

  // Display the city name (IATA code) and time on the LED matrix
  displayText(iataCode, 0, 0);
  displayTime(hour, minute, second, 0, 8);

  // Wait for 10 seconds before moving to the next city
  delay(10000);

  // Clear the display
  matrix.clearDisplay(0);

  // Move to the next city
  currentCityIndex++;
}

void previousButtonISR() {
  if (millis() - previousButtonPressTime >= debounceDelay) {
    previousButtonPressTime = millis();
    currentCityIndex--;
  }
}

void advanceButtonISR() {
  if (millis() - advanceButtonPressTime >= debounceDelay) {
    advanceButtonPressTime = millis();
    currentCityIndex++;
  }
}

String getTimeZone(const String& cityName) {
  for (int i = 0; i < numCities; i++) {
    if (cityName.equals(selectedCities[i])) {
      return String(cities[i].timeZone);
    }
  }
  return "";
}

String getIATACode(const String& cityName) {
  for (int i = 0; i < numCities; i++) {
    if (cityName.equals(selectedCities[i])) {
      return String(cities[i].iataCode);
    }
  }
  return "";
}

void displayText(const String& text, int x, int y) {
  for (int i = 0; i < text.length(); i++) {
    char character = text.charAt(i);
    matrix.setChar(0, x + i, character);
  }
}

void displayTime(int hour, int minute, int second, int x, int y) {
  int digit1 = hour / 10;
  int digit2 = hour % 10;
  int digit3 = minute / 10;
  int digit4 = minute % 10;
  int digit5 = second / 10;
  int digit6 = second % 10;

  matrix.setDigit(0, x, digit1, false);
  matrix.setDigit(0, x + 2, digit2, false);
  matrix.setDigit(0, x + 4, digit3, false);
  matrix.setDigit(0, x + 6, digit4, false);
  matrix.setDigit(0, x + 8, digit5, false);
  matrix.setDigit(0, x + 10, digit6, false);
}

void handleRoot(AsyncWebServerRequest* request) {
  String html = "<html><body>";
  html += "<h2>Select Cities:</h2>";
  html += "<form method='post' action='/update'>";

  // Generate checkboxes for each city
  for (int i = 0; i < numCities; i++) {
    String cityName = cities[i].name;
    String iataCode = cities[i].iataCode;
    html += "<input type='checkbox' name='city' value='" + cityName + "'";
    if (isCitySelected(cityName)) {
      html += " checked";
    }
    html += ">" + iataCode + " - " + cityName + "<br>";
  }

  html += "<br>";
  html += "Refresh Interval (seconds): <input type='text' name='interval' value='" + String(refreshInterval) + "'><br>";
  html += "<br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";

  // Display network configuration link
  html += "<br><br><a href='/network'>Network Configuration</a>";

  html += "</body></html>";

  request->send(200, "text/html", html);
}

void handleUpdate(AsyncWebServerRequest* request) {
  for (int i = 0; i < numCities; i++) {
    selectedCities[i] = "";
  }

  for (int i = 0; i < request->args(); i++) {
    String argName = request->argName(i);
    if (argName.equals("city")) {
      String cityName = request->arg(i);
      for (int j = 0; j < numCities; j++) {
        if (cityName.equals(cities[j].name)) {
          selectedCities[j] = cityName;
          break;
        }
      }
    } else if (argName.equals("interval")) {
      refreshInterval = request->arg(i).toInt();
    }
  }

  // Save configuration to EEPROM
  saveConfiguration();

  // Redirect to the root page
  request->redirect("/");
}

void handleNetwork(AsyncWebServerRequest* request) {
  String html = "<html><body>";
  html += "<h2>Network Configuration:</h2>";
  html += "<form method='post' action='/network'>";
  html += "SSID: <input type='text' name='ssid' value='" + String(ssid) + "'><br>";
  html += "Password: <input type='password' name='password' value='" + String(password) + "'><br>";
  html += "<br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "</body></html>";

  request->send(200, "text/html", html);
}

void handleNetworkUpdate(AsyncWebServerRequest* request) {
  if (request->args() > 0) {
    for (int i = 0; i < request->args(); i++) {
      String argName = request->argName(i);
      if (argName.equals("ssid")) {
        String newSSID = request->arg(i);
        newSSID.toCharArray(ssid, sizeof(ssid));
      } else if (argName.equals("password")) {
        String newPassword = request->arg(i);
        newPassword.toCharArray(password, sizeof(password));
      }
    }

    // Save configuration to EEPROM
    saveConfiguration();
  }

  // Redirect to the root page
  request->redirect("/");
}

bool isCitySelected(const String& cityName) {
  for (int i = 0; i < numCities; i++) {
    if (cityName.equals(selectedCities[i])) {
      return true;
    }
  }
  return false;
}

void saveConfiguration() {
  EEPROM.begin(512);

  // Clear previous configuration
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }

  // Save selected cities
  int address = eepromAddress;
  for (int i = 0; i < numCities; i++) {
    String cityName = selectedCities[i];
    int cityNameLength = cityName.length();
    EEPROM.write(address++, cityNameLength);
    for (int j = 0; j < cityNameLength; j++) {
      EEPROM.write(address++, cityName.charAt(j));
    }
  }

  // Save refresh interval
  EEPROM.write(256, highByte(refreshInterval));
  EEPROM.write(257, lowByte(refreshInterval));

  // Save network configuration
  for (int i = 0; i < 32; i++) {
    EEPROM.write(258 + i, ssid[i]);
  }
  for (int i = 0; i < 64; i++) {
    EEPROM.write(290 + i, password[i]);
  }

  EEPROM.commit();
  EEPROM.end();
}

void loadConfiguration() {
  EEPROM.begin(512);

  // Check if configuration is stored
  if (EEPROM.read(eepromAddress) == 0) {
    EEPROM.end();
    return;
  }

  // Load selected cities
  int address = eepromAddress;
  for (int i = 0; i < numCities; i++) {
    int cityNameLength = EEPROM.read(address++);
    String cityName = "";
    for (int j = 0; j < cityNameLength; j++) {
      char character = EEPROM.read(address++);
      cityName += character;
    }
    selectedCities[i] = cityName;
  }

  // Load refresh interval
  int highByte = EEPROM.read(256);
  int lowByte = EEPROM.read(257);
  refreshInterval = (highByte << 8) | lowByte;

  // Load network configuration
  for (int i = 0; i < 32; i++) {
    ssid[i] = EEPROM.read(258 + i);
  }
  for (int i = 0; i < 64; i++) {
    password[i] = EEPROM.read(290 + i);
  }

  EEPROM.end();
}

bool isConfigurationStored() {
  EEPROM.begin(512);
  bool stored = EEPROM.read(eepromAddress) != 0;
  EEPROM.end();
  return stored;
}
