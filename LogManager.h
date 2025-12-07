/**
 * LogManager.h
 * 
 * Logging-Manager für strukturierte Logs
 * 
 * Features:
 * - Zeilenweise Logs (kein JSON!)
 * - Verschiedene Log-Typen (Boot, Battery, Connection, Error)
 * - Automatisches Timestamp
 * - Log-Rotation bei Größenüberschreitung
 */

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include "SDCardHandler.h"

// Log-Dateinamen
#define LOG_FILE_BOOT       "/boot.log"
#define LOG_FILE_BATTERY    "/battery.log"
#define LOG_FILE_CONNECTION "/connection.log"
#define LOG_FILE_ERROR      "/error.log"

// Log-Rotation
#define LOG_MAX_FILE_SIZE   1048576 // 1 MB

class LogManager {
public:
    /**
     * Konstruktor
     * @param sdCard Referenz zum SDCardHandler
     */
    LogManager(SDCardHandler& sdCard);

    /**
     * Log-Manager initialisieren
     * @return true bei Erfolg
     */
    bool begin();

    /**
     * Alle Logs löschen
     */
    void clearAllLogs();

    // ═══════════════════════════════════════════════════════════════════════
    // BOOT LOG
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Boot-Start loggen
     * @param reason Reset-Grund
     * @param freeHeap Freier Heap in Bytes
     * @param version Firmware-Version
     */
    bool logBootStart(const char* reason, uint32_t freeHeap, const char* version);

    /**
     * Setup-Step loggen
     * @param module Modul-Name
     * @param success Erfolgreich?
     * @param message Optionale Nachricht
     */
    bool logSetupStep(const char* module, bool success, const char* message = nullptr);

    /**
     * Boot-Complete loggen
     * @param totalTimeMs Gesamt-Zeit in ms
     * @param success Erfolgreich?
     */
    bool logBootComplete(uint32_t totalTimeMs, bool success);

    // ═══════════════════════════════════════════════════════════════════════
    // BATTERY LOG
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Battery-Status loggen
     * @param voltage Spannung in Volt
     * @param percent Ladezustand in %
     * @param isLow Low-Battery?
     * @param isCritical Critical-Battery?
     */
    bool logBattery(float voltage, uint8_t percent, bool isLow, bool isCritical);

    // ═══════════════════════════════════════════════════════════════════════
    // CONNECTION LOG (ESP-NOW)
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Connection-Event loggen
     * @param peerMac Peer MAC-Adresse
     * @param event Event-Typ
     * @param rssi Signalstärke (optional)
     */
    bool logConnection(const char* peerMac, const char* event, int8_t rssi = 0);

    /**
     * Connection-Statistiken loggen
     * @param peerMac Peer MAC
     * @param packetsSent Gesendete Pakete
     * @param packetsReceived Empfangene Pakete
     * @param packetsLost Verlorene Pakete
     * @param avgRssi Durchschnittlicher RSSI
     */
    bool logConnectionStats(const char* peerMac, uint32_t packetsSent, 
                            uint32_t packetsReceived, uint32_t packetsLost, int8_t avgRssi);

    // ═══════════════════════════════════════════════════════════════════════
    // ERROR LOG
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Error loggen
     * @param module Modul-Name
     * @param errorCode Fehler-Code
     * @param message Fehlermeldung
     * @param freeHeap Freier Heap (optional, 0 = auto)
     */
    bool logError(const char* module, int errorCode, const char* message, uint32_t freeHeap = 0);

    /**
     * Crash loggen
     * @param pc Program Counter
     * @param excvaddr Exception Address
     * @param exccause Exception Cause
     */
    bool logCrash(uint32_t pc, uint32_t excvaddr, uint32_t exccause);

    /**
     * Debug-Info ausgeben
     */
    void printInfo();

private:
    SDCardHandler& sd;          // Referenz zum SD-Handler
    
    /**
     * Timestamp erstellen (Format: [12345ms])
     */
    String getTimestamp();
    
    /**
     * Log-Zeile schreiben
     * @param logFile Dateiname
     * @param line Log-Zeile
     * @return true bei Erfolg
     */
    bool writeLine(const char* logFile, const String& line);
    
    /**
     * Log-Datei rotieren wenn zu groß
     * @param logFile Dateiname
     */
    void rotateLogIfNeeded(const char* logFile);
};

// Globale Instanz (wird in .cpp definiert und in main.ino extern deklariert)
extern LogManager logManager;

#endif // LOG_MANAGER_H