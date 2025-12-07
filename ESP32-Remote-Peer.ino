#include "BatteryMonitor.h"
#include "ESPNowManager.h"
#include "SDCardHandler.h"
BatteryMonitor battery;
ESPNowManager ESPNow;
SDCardHandler sdCard;

// Timing für Logging
unsigned long lastBatteryLog = 0;
unsigned long lastConnectionLog = 0;
unsigned long setupStartTime = 0;

void setup() {
  setupStartTime = millis();

  Serial.begin(115200);
  delay(2000);

  Serial.println("-> GPIO...");

    // ═══════════════════════════════════════════════════════════════
    // Battery
    // ═══════════════════════════════════════════════════════════════
    Serial.println("→ Battery...");
    if (battery.begin()) {
        Serial.println("  ✅ Battery OK");
        sdCard.logSetupStep("Battery", true);
        
        // Initial-Status loggen
        sdCard.logBattery(battery.getVoltage(), battery.getPercent(), 
                         battery.isLow(), battery.isCritical());
    } else {
        sdCard.logSetupStep("Battery", false, "Sensor error");
        sdCard.logError("Battery", ERR_BATTERY_INIT, "begin() failed");
    }

    // ═══════════════════════════════════════════════════════════════
    // ESP-NOW
    // ═══════════════════════════════════════════════════════════════
    Serial.println("→ ESP-NOW...");
    EspNowManager& espnow = EspNowManager::getInstance();

    if (espnow.begin(ESPNOW_CHANNEL)) {
        Serial.println("  ✅ ESP-NOW OK");
        Serial.printf("  MAC: %s\n", espnow.getOwnMacString().c_str());
        
        // Heartbeat aus Config
        espnow.setHeartbeat(true, config.espnowHeartbeatInterval);
        espnow.setTimeout(config.espnowTimeout);
        
        sdCard.logSetupStep("ESP-NOW", true, espnow.getOwnMacString().c_str());
        
        // Events für Logging registrieren
        espnow.onEvent(EspNowEvent::PEER_CONNECTED, [](EspNowEventData* data) {
            String mac = EspNowManager::macToString(data->mac);
            sdCard.logConnection(mac.c_str(), "connected");
            Serial.printf("ESP-NOW: Peer %s connected\n", mac.c_str());
        });
        
        espnow.onEvent(EspNowEvent::PEER_DISCONNECTED, [](EspNowEventData* data) {
            String mac = EspNowManager::macToString(data->mac);
            sdCard.logConnection(mac.c_str(), "disconnected");
            Serial.printf("ESP-NOW: Peer %s disconnected\n", mac.c_str());
        });
        
        espnow.onEvent(EspNowEvent::HEARTBEAT_TIMEOUT, [](EspNowEventData* data) {
            String mac = EspNowManager::macToString(data->mac);
            sdCard.logConnection(mac.c_str(), "timeout");
            Serial.printf("ESP-NOW: Peer %s timeout\n", mac.c_str());
        });
    } else {
        sdCard.logSetupStep("ESP-NOW", false, "WiFi init error");
        sdCard.logError("ESP-NOW", 3, "esp_now_init() failed");
    }

    // ═══════════════════════════════════════════════════════════════
    // Setup Complete
    // ═══════════════════════════════════════════════════════════════
    unsigned long setupTime = millis() - setupStartTime;
    
    Serial.println();
    Serial.println("✅ Setup complete!");
    Serial.printf("   Setup-Zeit: %lu ms\n", setupTime);
    Serial.println();
    
    sdCard.logBootComplete(setupTime, true);
  }
}

void loop() {
  // ═══════════════════════════════════════════════════════════════
  // Battery Monitor
  // ═══════════════════════════════════════════════════════════════
  battery.update();
  
  static unsigned long lastBatteryUpdate = 0;
  if (millis() - lastBatteryUpdate > 2000) {
      globalUI.updateBatteryIcon();
      lastBatteryUpdate = millis();
  }
  
  // Battery-Status loggen (alle 60 Sekunden)
  if (sdCard.isAvailable() && (millis() - lastBatteryLog > 60000)) {
      sdCard.logBattery(battery.getVoltage(), battery.getPercent(), 
                        battery.isLow(), battery.isCritical());
      lastBatteryLog = millis();
  }
  
  // Battery Critical → Error-Log
  if (battery.isCritical()) {
      static bool criticalLogged = false;
      if (!criticalLogged) {
          sdCard.logError("Battery", ERR_BATTERY_CRITICAL, 
                        "Critical voltage!", ESP.getFreeHeap());
          criticalLogged = true;
      }
  }

  // ═══════════════════════════════════════════════════════════════
  // ESP-NOW
  // ═══════════════════════════════════════════════════════════════
  EspNowManager& espnow = EspNowManager::getInstance();
  espnow.update();
  
  // Connection-Stats loggen (alle 5 Minuten)
  if (sdCard.isAvailable() && (millis() - lastConnectionLog > 300000)) {
      if (espnow.isConnected()) {
          // Vereinfacht: Stats vom System
          int rxPending, txPending, resultPending;
          espnow.getQueueStats(&rxPending, &txPending, &resultPending);
          
          String mac = espnow.getOwnMacString();
          sdCard.logConnectionStats(mac.c_str(), 0, 0, 0, 0, 0, -65);
      }
      lastConnectionLog = millis();
  }

  delay(10);
}
