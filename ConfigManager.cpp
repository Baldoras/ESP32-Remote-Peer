/**
 * ConfigManager.cpp
 * 
 * Implementation der Konfigurationsverwaltung
 */

#include "ConfigManager.h"

ConfigManager::ConfigManager(SDCardHandler& sdCard, bool isPeer)
    : sd(sdCard)
    , isPeer(isPeer)
{
    setDefaults();
}

bool ConfigManager::begin() {
    DEBUG_PRINTLN("ConfigManager: Initialisiere...");
    
    // Versuche Config zu laden
    if (load()) {
        DEBUG_PRINTLN("ConfigManager: ✅ Config geladen");
        
        // Validieren
        if (validate()) {
            DEBUG_PRINTLN("ConfigManager: ✅ Config validiert");
            return true;
        } else {
            DEBUG_PRINTLN("ConfigManager: ⚠️ Config ungültig, nutze Defaults");
            setDefaults();
            save();  // Defaults speichern
            return true;
        }
    } else {
        DEBUG_PRINTLN("ConfigManager: ⚠️ Config nicht gefunden, erstelle Default");
        setDefaults();
        save();
        return true;
    }
}

bool ConfigManager::load() {
    if (!sd.isAvailable()) {
        DEBUG_PRINTLN("ConfigManager: ❌ SD-Karte nicht verfügbar");
        return false;
    }
    
    if (isPeer) {
        return loadPeerConfig();
    } else {
        return loadMainConfig();
    }
}

bool ConfigManager::save() {
    if (!sd.isAvailable()) {
        DEBUG_PRINTLN("ConfigManager: ❌ SD-Karte nicht verfügbar");
        return false;
    }
    
    if (isPeer) {
        return savePeerConfig();
    } else {
        return saveMainConfig();
    }
}

bool ConfigManager::validate() {
    if (isPeer) {
        return validatePeerConfig();
    } else {
        return validateMainConfig();
    }
}

void ConfigManager::setDefaults() {
    if (isPeer) {
        // Peer Config Defaults
        strncpy(peerConfig.espnowMainMAC, ESPNOW_MAIN_DEVICE_MAC, 17);
        peerConfig.espnowMainMAC[17] = '\0';
        peerConfig.espnowTimeout = ESPNOW_TIMEOUT_MS;
        peerConfig.batteryCalibration = VOLTAGE_CALIBRATION_FACTOR;
        peerConfig.debugSerialEnabled = DEBUG_SERIAL;
        
        DEBUG_PRINTLN("ConfigManager: Peer-Defaults gesetzt");
    } else {
        // Main Config Defaults
        mainConfig.backlightDefault = BACKLIGHT_DEFAULT;
        
        mainConfig.touchMinX = TOUCH_MIN_X;
        mainConfig.touchMaxX = TOUCH_MAX_X;
        mainConfig.touchMinY = TOUCH_MIN_Y;
        mainConfig.touchMaxY = TOUCH_MAX_Y;
        mainConfig.touchThreshold = TOUCH_THRESHOLD;
        
        mainConfig.joystickCenterX = JOY_CENTER_X;
        mainConfig.joystickCenterY = JOY_CENTER_Y;
        mainConfig.joystickDeadzone = JOY_DEADZONE;
        
        strncpy(mainConfig.espnowPeerMAC, "00:00:00:00:00:00", 17);
        mainConfig.espnowPeerMAC[17] = '\0';
        mainConfig.espnowHeartbeatInterval = ESPNOW_HEARTBEAT_INTERVAL;
        mainConfig.espnowTimeout = ESPNOW_TIMEOUT_MS;
        
        mainConfig.batteryCalibration = VOLTAGE_CALIBRATION_FACTOR;
        mainConfig.debugSerialEnabled = DEBUG_SERIAL;
        
        DEBUG_PRINTLN("ConfigManager: Main-Defaults gesetzt");
    }
}

void ConfigManager::printInfo() {
    DEBUG_PRINTLN("\n╔═══════════════════════════════════════════════╗");
    DEBUG_PRINTLN("║           CONFIG MANAGER INFO                 ║");
    DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝");
    
    DEBUG_PRINTF("Device Type: %s\n", isPeer ? "Peer" : "Main");
    
    if (isPeer) {
        DEBUG_PRINTLN("\n─── Peer Config ───────────────────────────────");
        DEBUG_PRINTF("Main MAC:         %s\n", peerConfig.espnowMainMAC);
        DEBUG_PRINTF("Timeout:          %dms\n", peerConfig.espnowTimeout);
        DEBUG_PRINTF("Battery Cal:      %.2f\n", peerConfig.batteryCalibration);
        DEBUG_PRINTF("Debug Serial:     %s\n", peerConfig.debugSerialEnabled ? "ON" : "OFF");
    } else {
        DEBUG_PRINTLN("\n─── Main Config ───────────────────────────────");
        DEBUG_PRINTF("Backlight:        %d\n", mainConfig.backlightDefault);
        DEBUG_PRINTF("Touch Min/Max X:  %d / %d\n", mainConfig.touchMinX, mainConfig.touchMaxX);
        DEBUG_PRINTF("Touch Min/Max Y:  %d / %d\n", mainConfig.touchMinY, mainConfig.touchMaxY);
        DEBUG_PRINTF("Touch Threshold:  %d\n", mainConfig.touchThreshold);
        DEBUG_PRINTF("Joy Center X/Y:   %d / %d\n", mainConfig.joystickCenterX, mainConfig.joystickCenterY);
        DEBUG_PRINTF("Joy Deadzone:     %d\n", mainConfig.joystickDeadzone);
        DEBUG_PRINTF("Peer MAC:         %s\n", mainConfig.espnowPeerMAC);
        DEBUG_PRINTF("Heartbeat:        %dms\n", mainConfig.espnowHeartbeatInterval);
        DEBUG_PRINTF("Timeout:          %dms\n", mainConfig.espnowTimeout);
        DEBUG_PRINTF("Battery Cal:      %.2f\n", mainConfig.batteryCalibration);
        DEBUG_PRINTF("Debug Serial:     %s\n", mainConfig.debugSerialEnabled ? "ON" : "OFF");
    }
    
    DEBUG_PRINTLN("═══════════════════════════════════════════════\n");
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE - MAIN CONFIG
// ═══════════════════════════════════════════════════════════════════════════

bool ConfigManager::loadMainConfig() {
    if (!sd.fileExists(CONFIG_FILE_MAIN)) {
        DEBUG_PRINTLN("ConfigManager: config_main.json nicht gefunden");
        return false;
    }
    
    String jsonStr = sd.readFileAsString(CONFIG_FILE_MAIN);
    if (jsonStr.isEmpty()) {
        DEBUG_PRINTLN("ConfigManager: ❌ Kann config_main.json nicht lesen");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        DEBUG_PRINTF("ConfigManager: ❌ JSON Parse-Fehler: %s\n", error.c_str());
        return false;
    }
    
    // Display
    mainConfig.backlightDefault = doc["backlight_default"] | BACKLIGHT_DEFAULT;
    
    // Touch
    mainConfig.touchMinX = doc["touch_min_x"] | TOUCH_MIN_X;
    mainConfig.touchMaxX = doc["touch_max_x"] | TOUCH_MAX_X;
    mainConfig.touchMinY = doc["touch_min_y"] | TOUCH_MIN_Y;
    mainConfig.touchMaxY = doc["touch_max_y"] | TOUCH_MAX_Y;
    mainConfig.touchThreshold = doc["touch_threshold"] | TOUCH_THRESHOLD;
    
    // Joystick
    mainConfig.joystickCenterX = doc["joystick_center_x"] | JOY_CENTER_X;
    mainConfig.joystickCenterY = doc["joystick_center_y"] | JOY_CENTER_Y;
    mainConfig.joystickDeadzone = doc["joystick_deadzone"] | JOY_DEADZONE;
    
    // ESP-NOW
    const char* peerMac = doc["espnow_peer_mac"] | "00:00:00:00:00:00";
    strncpy(mainConfig.espnowPeerMAC, peerMac, 17);
    mainConfig.espnowPeerMAC[17] = '\0';
    mainConfig.espnowHeartbeatInterval = doc["espnow_heartbeat"] | ESPNOW_HEARTBEAT_INTERVAL;
    mainConfig.espnowTimeout = doc["espnow_timeout"] | ESPNOW_TIMEOUT_MS;
    
    // Battery
    mainConfig.batteryCalibration = doc["battery_calibration"] | VOLTAGE_CALIBRATION_FACTOR;
    
    // Debug
    mainConfig.debugSerialEnabled = doc["debug_serial"] | DEBUG_SERIAL;
    
    return true;
}

bool ConfigManager::saveMainConfig() {
    JsonDocument doc;
    
    // Display
    doc["backlight_default"] = mainConfig.backlightDefault;
    
    // Touch
    doc["touch_min_x"] = mainConfig.touchMinX;
    doc["touch_max_x"] = mainConfig.touchMaxX;
    doc["touch_min_y"] = mainConfig.touchMinY;
    doc["touch_max_y"] = mainConfig.touchMaxY;
    doc["touch_threshold"] = mainConfig.touchThreshold;
    
    // Joystick
    doc["joystick_center_x"] = mainConfig.joystickCenterX;
    doc["joystick_center_y"] = mainConfig.joystickCenterY;
    doc["joystick_deadzone"] = mainConfig.joystickDeadzone;
    
    // ESP-NOW
    doc["espnow_peer_mac"] = mainConfig.espnowPeerMAC;
    doc["espnow_heartbeat"] = mainConfig.espnowHeartbeatInterval;
    doc["espnow_timeout"] = mainConfig.espnowTimeout;
    
    // Battery
    doc["battery_calibration"] = mainConfig.batteryCalibration;
    
    // Debug
    doc["debug_serial"] = mainConfig.debugSerialEnabled;
    
    // Zu String serialisieren
    String jsonStr;
    serializeJsonPretty(doc, jsonStr);
    
    // Auf SD schreiben
    bool success = sd.writeFile(CONFIG_FILE_MAIN, jsonStr);
    
    if (success) {
        DEBUG_PRINTLN("ConfigManager: ✅ Main-Config gespeichert");
    } else {
        DEBUG_PRINTLN("ConfigManager: ❌ Main-Config speichern fehlgeschlagen");
    }
    
    return success;
}

bool ConfigManager::validateMainConfig() {
    bool valid = true;
    
    // Backlight
    if (mainConfig.backlightDefault > 255) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ Backlight ungültig");
        mainConfig.backlightDefault = BACKLIGHT_DEFAULT;
        valid = false;
    }
    
    // Touch
    if (mainConfig.touchMinX >= mainConfig.touchMaxX || 
        mainConfig.touchMinY >= mainConfig.touchMaxY) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ Touch-Kalibrierung ungültig");
        mainConfig.touchMinX = TOUCH_MIN_X;
        mainConfig.touchMaxX = TOUCH_MAX_X;
        mainConfig.touchMinY = TOUCH_MIN_Y;
        mainConfig.touchMaxY = TOUCH_MAX_Y;
        valid = false;
    }
    
    // ESP-NOW Timeout
    if (mainConfig.espnowTimeout < 1000 || mainConfig.espnowTimeout > 30000) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ ESP-NOW Timeout ungültig");
        mainConfig.espnowTimeout = ESPNOW_TIMEOUT_MS;
        valid = false;
    }
    
    // Battery Calibration
    if (mainConfig.batteryCalibration <= 0.0f || mainConfig.batteryCalibration > 2.0f) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ Battery Calibration ungültig");
        mainConfig.batteryCalibration = VOLTAGE_CALIBRATION_FACTOR;
        valid = false;
    }
    
    return valid;
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE - PEER CONFIG
// ═══════════════════════════════════════════════════════════════════════════

bool ConfigManager::loadPeerConfig() {
    if (!sd.fileExists(CONFIG_FILE_PEER)) {
        DEBUG_PRINTLN("ConfigManager: config_peer.json nicht gefunden");
        return false;
    }
    
    String jsonStr = sd.readFileAsString(CONFIG_FILE_PEER);
    if (jsonStr.isEmpty()) {
        DEBUG_PRINTLN("ConfigManager: ❌ Kann config_peer.json nicht lesen");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        DEBUG_PRINTF("ConfigManager: ❌ JSON Parse-Fehler: %s\n", error.c_str());
        return false;
    }
    
    // ESP-NOW
    const char* mainMac = doc["espnow_main_mac"] | ESPNOW_MAIN_DEVICE_MAC;
    strncpy(peerConfig.espnowMainMAC, mainMac, 17);
    peerConfig.espnowMainMAC[17] = '\0';
    peerConfig.espnowTimeout = doc["espnow_timeout"] | ESPNOW_TIMEOUT_MS;
    
    // Battery
    peerConfig.batteryCalibration = doc["battery_calibration"] | VOLTAGE_CALIBRATION_FACTOR;
    
    // Debug
    peerConfig.debugSerialEnabled = doc["debug_serial"] | DEBUG_SERIAL;
    
    return true;
}

bool ConfigManager::savePeerConfig() {
    JsonDocument doc;
    
    // ESP-NOW
    doc["espnow_main_mac"] = peerConfig.espnowMainMAC;
    doc["espnow_timeout"] = peerConfig.espnowTimeout;
    
    // Battery
    doc["battery_calibration"] = peerConfig.batteryCalibration;
    
    // Debug
    doc["debug_serial"] = peerConfig.debugSerialEnabled;
    
    // Zu String serialisieren
    String jsonStr;
    serializeJsonPretty(doc, jsonStr);
    
    // Auf SD schreiben
    bool success = sd.writeFile(CONFIG_FILE_PEER, jsonStr);
    
    if (success) {
        DEBUG_PRINTLN("ConfigManager: ✅ Peer-Config gespeichert");
    } else {
        DEBUG_PRINTLN("ConfigManager: ❌ Peer-Config speichern fehlgeschlagen");
    }
    
    return success;
}

bool ConfigManager::validatePeerConfig() {
    bool valid = true;
    
    // ESP-NOW Timeout
    if (peerConfig.espnowTimeout < 1000 || peerConfig.espnowTimeout > 30000) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ ESP-NOW Timeout ungültig");
        peerConfig.espnowTimeout = ESPNOW_TIMEOUT_MS;
        valid = false;
    }
    
    // Battery Calibration
    if (peerConfig.batteryCalibration <= 0.0f || peerConfig.batteryCalibration > 2.0f) {
        DEBUG_PRINTLN("ConfigManager: ⚠️ Battery Calibration ungültig");
        peerConfig.batteryCalibration = VOLTAGE_CALIBRATION_FACTOR;
        valid = false;
    }
    
    return valid;
}