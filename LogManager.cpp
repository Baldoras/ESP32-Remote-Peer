/**
 * LogManager.cpp
 * 
 * Implementation des Logging-Managers
 * Logs werden zeilenweise im Format: [timestamp] level - message
 */

#include "LogManager.h"

LogManager::LogManager(SDCardHandler& sdCard)
    : sd(sdCard)
{
}

bool LogManager::begin() {
    DEBUG_PRINTLN("LogManager: Initialisiere...");
    
    if (!sd.isAvailable()) {
        DEBUG_PRINTLN("LogManager: ⚠️ SD-Karte nicht verfügbar");
        return false;
    }
    
    DEBUG_PRINTLN("LogManager: ✅ Bereit");
    return true;
}

void LogManager::clearAllLogs() {
    if (!sd.isAvailable()) return;
    
    DEBUG_PRINTLN("LogManager: Lösche alle Logs...");
    
    sd.deleteFile(LOG_FILE_BOOT);
    sd.deleteFile(LOG_FILE_BATTERY);
    sd.deleteFile(LOG_FILE_CONNECTION);
    sd.deleteFile(LOG_FILE_ERROR);
    
    DEBUG_PRINTLN("LogManager: ✅ Logs gelöscht");
}

// ═══════════════════════════════════════════════════════════════════════════
// BOOT LOG
// ═══════════════════════════════════════════════════════════════════════════

bool LogManager::logBootStart(const char* reason, uint32_t freeHeap, const char* version) {
    String line = getTimestamp() + " INFO - Boot Start";
    line += " | Reason: " + String(reason);
    line += " | Version: " + String(version);
    line += " | Free Heap: " + String(freeHeap) + " bytes";
    line += " | Chip: " + String(ESP.getChipModel());
    line += " | CPU: " + String(ESP.getCpuFreqMHz()) + " MHz";
    
    return writeLine(LOG_FILE_BOOT, line);
}

bool LogManager::logSetupStep(const char* module, bool success, const char* message) {
    String line = getTimestamp();
    line += success ? " INFO - " : " ERROR - ";
    line += "Setup: " + String(module);
    line += success ? " [OK]" : " [FAILED]";
    
    if (message) {
        line += " | " + String(message);
    }
    
    return writeLine(LOG_FILE_BOOT, line);
}

bool LogManager::logBootComplete(uint32_t totalTimeMs, bool success) {
    String line = getTimestamp();
    line += success ? " INFO - " : " ERROR - ";
    line += "Boot Complete";
    line += " | Time: " + String(totalTimeMs) + "ms";
    line += " | Free Heap: " + String(ESP.getFreeHeap()) + " bytes";
    line += success ? " [SUCCESS]" : " [FAILED]";
    
    // Leere Zeile als Separator
    writeLine(LOG_FILE_BOOT, "");
    
    return writeLine(LOG_FILE_BOOT, line);
}

// ═══════════════════════════════════════════════════════════════════════════
// BATTERY LOG
// ═══════════════════════════════════════════════════════════════════════════

bool LogManager::logBattery(float voltage, uint8_t percent, bool isLow, bool isCritical) {
    String line = getTimestamp();
    
    if (isCritical) {
        line += " CRITICAL - ";
    } else if (isLow) {
        line += " WARN - ";
    } else {
        line += " INFO - ";
    }
    
    line += "Battery";
    line += " | Voltage: " + String(voltage, 2) + "V";
    line += " | Percent: " + String(percent) + "%";
    
    if (isCritical) {
        line += " [CRITICAL]";
    } else if (isLow) {
        line += " [LOW]";
    }
    
    return writeLine(LOG_FILE_BATTERY, line);
}

// ═══════════════════════════════════════════════════════════════════════════
// CONNECTION LOG
// ═══════════════════════════════════════════════════════════════════════════

bool LogManager::logConnection(const char* peerMac, const char* event, int8_t rssi) {
    String line = getTimestamp() + " INFO - ";
    line += "ESP-NOW: " + String(event);
    line += " | Peer: " + String(peerMac);
    
    if (rssi != 0) {
        line += " | RSSI: " + String(rssi) + " dBm";
    }
    
    return writeLine(LOG_FILE_CONNECTION, line);
}

bool LogManager::logConnectionStats(const char* peerMac, uint32_t packetsSent, 
                                     uint32_t packetsReceived, uint32_t packetsLost, int8_t avgRssi) {
    String line = getTimestamp() + " INFO - ";
    line += "ESP-NOW Stats";
    line += " | Peer: " + String(peerMac);
    line += " | Sent: " + String(packetsSent);
    line += " | Received: " + String(packetsReceived);
    line += " | Lost: " + String(packetsLost);
    
    // Loss-Rate berechnen
    if (packetsSent > 0) {
        float lossRate = (packetsLost * 100.0f) / packetsSent;
        line += " | Loss: " + String(lossRate, 1) + "%";
    }
    
    line += " | Avg RSSI: " + String(avgRssi) + " dBm";
    
    return writeLine(LOG_FILE_CONNECTION, line);
}

// ═══════════════════════════════════════════════════════════════════════════
// ERROR LOG
// ═══════════════════════════════════════════════════════════════════════════

bool LogManager::logError(const char* module, int errorCode, const char* message, uint32_t freeHeap) {
    String line = getTimestamp() + " ERROR - ";
    line += String(module);
    line += " | Code: " + String(errorCode);
    line += " | " + String(message);
    
    if (freeHeap > 0) {
        line += " | Free Heap: " + String(freeHeap) + " bytes";
    } else {
        line += " | Free Heap: " + String(ESP.getFreeHeap()) + " bytes";
    }
    
    return writeLine(LOG_FILE_ERROR, line);
}

bool LogManager::logCrash(uint32_t pc, uint32_t excvaddr, uint32_t exccause) {
    String line = getTimestamp() + " FATAL - ";
    line += "CRASH";
    line += " | PC: 0x" + String(pc, HEX);
    line += " | ExcVAddr: 0x" + String(excvaddr, HEX);
    line += " | ExcCause: " + String(exccause);
    line += " | Free Heap: " + String(ESP.getFreeHeap()) + " bytes";
    
    return writeLine(LOG_FILE_ERROR, line);
}

void LogManager::printInfo() {
    DEBUG_PRINTLN("\n╔═══════════════════════════════════════════════╗");
    DEBUG_PRINTLN("║            LOG MANAGER INFO                   ║");
    DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝");
    
    DEBUG_PRINTF("SD Card:    %s\n", sd.isAvailable() ? "✅ Available" : "❌ Not available");
    
    if (sd.isAvailable()) {
        DEBUG_PRINTLN("\n─── Log Files ─────────────────────────────────");
        
        const char* logFiles[] = {
            LOG_FILE_BOOT,
            LOG_FILE_BATTERY,
            LOG_FILE_CONNECTION,
            LOG_FILE_ERROR
        };
        
        for (int i = 0; i < 4; i++) {
            if (sd.fileExists(logFiles[i])) {
                size_t size = sd.getFileSize(logFiles[i]);
                DEBUG_PRINTF("  %s: %.2f KB\n", logFiles[i], size / 1024.0);
            } else {
                DEBUG_PRINTF("  %s: [not exist]\n", logFiles[i]);
            }
        }
    }
    
    DEBUG_PRINTLN("═══════════════════════════════════════════════\n");
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE METHODEN
// ═══════════════════════════════════════════════════════════════════════════

String LogManager::getTimestamp() {
    unsigned long ms = millis();
    
    // Format: [12345ms]
    return "[" + String(ms) + "ms]";
}

bool LogManager::writeLine(const char* logFile, const String& line) {
    if (!sd.isAvailable()) return false;
    
    // Log rotieren falls nötig
    rotateLogIfNeeded(logFile);
    
    // Zeile anhängen
    return sd.appendLine(logFile, line);
}

void LogManager::rotateLogIfNeeded(const char* logFile) {
    size_t fileSize = sd.getFileSize(logFile);
    
    if (fileSize > LOG_MAX_FILE_SIZE) {
        DEBUG_PRINTF("LogManager: Rotiere Log: %s (%.2f KB)\n", logFile, fileSize / 1024.0);
        
        // Backup-Name erstellen (z.B. boot.log -> boot.log.1)
        String backupPath = String(logFile) + ".1";
        
        // Altes Backup löschen
        if (sd.fileExists(backupPath.c_str())) {
            sd.deleteFile(backupPath.c_str());
        }
        
        // Aktuelle Datei umbenennen
        sd.renameFile(logFile, backupPath.c_str());
        
        DEBUG_PRINTF("LogManager: ✅ Log rotiert zu: %s\n", backupPath.c_str());
    }
}