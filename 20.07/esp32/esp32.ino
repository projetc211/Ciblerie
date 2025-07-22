
// OK le 08.07.25 à 16H

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>

// === Configuration WiFi (modifiable via Bluetooth) ===
String ssid = "Livebox-9410";
String password = "37322944";

// === Déclaration des objets ===
WebSocketsServer webSocket = WebSocketsServer(81);
BluetoothSerial SerialBT;

// === Gestion Jeu ===
enum GameState { IDLE, WAITING_CONFIRMATION, GAME_STARTED };
GameState currentGameState = IDLE;
unsigned long lastSerialActivity = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("🔗 Liaison série avec Arduino Méga initialisée.");

  // === VOTRE PARTIE BLUETOOTH EXACTEMENT COMME VOUS L'AVEZ ===
  SerialBT.begin("🖥️ ESP32_BT_Config");
  Serial.println("📡 Bluetooth initialisé. Connectez-vous à 'ESP32_BT_Config'.");

  SerialBT.register_callback([](esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (event == ESP_SPP_SRV_OPEN_EVT) {
      sendBluetoothWelcomeMessage();
    }
  });
  // === FIN DE VOTRE PARTIE BLUETOOTH ===

  connectToWiFi();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  handleBluetoothCommands(); // Votre fonction originale
  processIncomingSerialData();
}

// ============== VOS FONCTIONS BLUETOOTH ORIGINALES (100% inchangées) ==============
void sendBluetoothWelcomeMessage() {
  SerialBT.println("🔗 Bienvenue sur ESP32_BT_Config !");
  SerialBT.println("Voici les informations actuelles et commandes disponibles :");

  if (WiFi.status() == WL_CONNECTED) {
    SerialBT.println("✅ WiFi connecté !");
    SerialBT.print("📶 Adresse IP : ");
    SerialBT.println(WiFi.localIP());
  } else {
    SerialBT.println("❌ WiFi non connecté.");
  }

  SerialBT.print("📡 SSID actuel : ");
  SerialBT.println(ssid);
  SerialBT.print("🔑 Mot de passe actuel : ");
  SerialBT.println(password);

  SerialBT.println("\n⚙️ Commandes disponibles :");
  SerialBT.println(" - SSID:<NomDuReseau>");
  SerialBT.println(" - PASS:<MotDePasse>");
  SerialBT.println(" - RESTART");
}

void handleBluetoothCommands() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SSID:")) {
      ssid = command.substring(5);
      Serial.println("✅ SSID mis à jour : " + ssid);
      SerialBT.println("✅ SSID mis à jour : " + ssid);
      connectToWiFi();
    } else if (command.startsWith("PASS:")) {
      password = command.substring(5);
      Serial.println("✅ Mot de passe mis à jour.");
      SerialBT.println("✅ Mot de passe mis à jour.");
      connectToWiFi();
    } else if (command == "RESTART") {
      Serial.println("🔄 Redémarrage de l'ESP32...");
      SerialBT.println("🔄 Redémarrage de l'ESP32...");
      delay(1000);
      ESP.restart();
    } else {
      Serial.println("⚠️ Commande inconnue : " + command);
      SerialBT.println("⚠️ Commande inconnue : " + command);
    }
  }
}

// ============== FONCTIONS WiFi (adaptées pour garder votre style) ==============
void connectToWiFi() {
  Serial.println("🌐 Connexion en cours...");
  SerialBT.println("🌐 Connexion en cours...");

  WiFi.begin(ssid.c_str(), password.c_str());

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    SerialBT.print(".");
    retries++;
  }

  Serial.println("");
  SerialBT.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Connexion WiFi réussie !");
    SerialBT.println("✅ Connexion WiFi réussie !");
  } else {
    Serial.println("❌ Connexion WiFi échouée.");
    SerialBT.println("❌ Connexion WiFi échouée.");
  }
}

// ============== NOUVELLES FONCTIONS OPTIMISÉES ==============
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_TEXT: {
      String jsonStr = String((char*)payload);
      StaticJsonDocument<256> doc;

      DeserializationError error = deserializeJson(doc, jsonStr);
      if (error) {
        Serial.println("❌ Erreur JSON: " + String(error.c_str()));
        return;
      }

      String type = doc["type"] | "";
      String msg = doc["message"] | "";

      if (type == "group_game") {
        Serial1.println(msg);
        Serial.println("📨 Envoi à Arduino: " + msg);
        currentGameState = WAITING_CONFIRMATION;

      } else if (type == "game_status" && msg == "START_GAME") {
        Serial1.println("START_GAME");
        Serial.println("📨 Envoi à Arduino: START_GAME");

      } else if (type == "game_status" && msg == "NEXT_PLAYER") {
        Serial1.println("NEXT_PLAYER");
        Serial.println("📨 Envoi à Arduino: NEXT_PLAYER");

      } else if (type == "game_status" && msg == "NEXT_TURN") {
        Serial1.println("NEXT_TURN");
        Serial.println("📨 Envoi à Arduino: NEXT_TURN");
      
      } else if (type == "game_status" && msg == "GO") {
        Serial1.println("GO");
        Serial.println("📨 Envoi à Arduino: GO");
      
      } else if (type == "game_status" && msg == "VALID_E") {
        Serial1.println("VALID_E");
        Serial.println("📨 Envoi à Arduino: VALID_E"); 
      
      } else if (type == "game_status" && msg == "BOUTON_E_VIRTUEL") {
        Serial1.println("BOUTON_E_VIRTUEL");
        Serial.println("📨 Envoi à Arduino: BOUTON_E_VIRTUEL");
      }
      break;
    }
  }
}

void processIncomingSerialData() {
  while (Serial1.available()) {
    String incomingData = Serial1.readStringUntil('\n');
    incomingData.trim();
    lastSerialActivity = millis();
    Serial.println("📥 Message reçu de l'Arduino: " + incomingData);

    if (incomingData.startsWith("J")) {
      handleScoreData(incomingData.substring(1));
    } 
    else if (incomingData == "START") {
      sendGameStatus("START");
      currentGameState = WAITING_CONFIRMATION;
    }
    else if (incomingData.startsWith("SERVO_UP:")) {
      handleServoUpData(incomingData.substring(9));
    }
    else if (incomingData == "NEXT_PLAYER") {
      sendGameStatus("NEXT_PLAYER");
    }
    else if (incomingData == "NEXT_TURN") {
      sendGameStatus("NEXT_TURN");
    }
    else if (incomingData == "GO") {
      sendGameStatus("GO");
    }
    else if (incomingData == "FIN_GAME") {
      sendGameStatus("FIN_GAME"); 
    }
    else if (incomingData.startsWith("COUNTDOWN:")) {
      float countdownValue = incomingData.substring(10).toFloat();
      // ENVOI TOUJOURS LA VALEUR, MEME SI 0
      StaticJsonDocument<128> doc;
      doc["type"] = "game_status";
      doc["message"] = "COUNTDOWN";
      doc["value"] = countdownValue;
      String output;
      serializeJson(doc, output);
      webSocket.broadcastTXT(output);
      Serial.println("📨 Countdown envoyé: " + output);
    }
    else if (incomingData.startsWith("END_MANCHE:")) {
    // Nouveau format: END_MANCHE:<playerIndex>:<temps>
    int firstColon = incomingData.indexOf(':');
    int secondColon = incomingData.indexOf(':', firstColon + 1);
    if (firstColon > 0 && secondColon > firstColon) {
      int playerIndex = incomingData.substring(firstColon + 1, secondColon).toInt();
      float endMancheValue = incomingData.substring(secondColon + 1).toFloat();
      StaticJsonDocument<128> doc;
      doc["type"] = "game_status";
      doc["message"] = "END_MANCHE";
      doc["value"] = endMancheValue;
      doc["playerIndex"] = playerIndex;
      String output;
      serializeJson(doc, output);
      webSocket.broadcastTXT(output);
      Serial.println("📨 END_MANCHE envoyé: " + output);
      } else {
        Serial.println("❌ Mauvais format END_MANCHE reçu !");
        }
    }
  }
}

void handleScoreData(const String& data) {
  int colon1 = data.indexOf(':');
  int colon2 = data.indexOf(':', colon1 + 1);
  int colon3 = data.lastIndexOf(':');

  // Extraction des données
  int playerIndex = data.substring(0, colon1).toInt();
  int points = data.substring(colon1 + 1, colon2).toInt();
  int pointsbonus = data.substring(colon2 + 1, colon3).toInt();
  int score = data.substring(colon3 + 1).toInt();

  // Création du nouveau document JSON
  StaticJsonDocument<256> outputDoc;
  outputDoc["playerIndex"] = playerIndex;
  outputDoc["points"] = points;
  outputDoc["pointsbonus"] = pointsbonus;
  outputDoc["score"] = score;

  // Serialisation et envoi
  String output;
  serializeJson(outputDoc, output);
  webSocket.broadcastTXT(output);
  Serial.println("📨 Score: " + output);
}

void sendGameStatus(const String& status) {
  StaticJsonDocument<128> doc;
  doc["type"] = "game_status";
  doc["message"] = status;
  String output;
  serializeJson(doc, output);
  webSocket.broadcastTXT(output);
  Serial.println("📨 Données envoyées via WebSocket : " + output); // Ajout du log
}

void sendWebSocketPing() {
  for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
    if (webSocket.clientIsConnected(i)) {
      webSocket.sendPing(i);
    }
  }
}

void handleServoUpData(const String& data) {
  int servoIndex = data.toInt();
  StaticJsonDocument<128> doc;
  doc["type"] = "servo_up";
  doc["servoIndex"] = servoIndex;
  String output;
  serializeJson(doc, output);
  webSocket.broadcastTXT(output);
  Serial.println("📨 Servo levé: " + output);
}
