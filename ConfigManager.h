/**
 * ConfigManager.h
 * 
 * Zentrale Konfigurationsverwaltung
 * 
 * Features:
 * - Lädt/Speichert Config von/auf SD-Karte (JSON)
 * - Validiert Config-Werte
 * - Stellt Default-Werte bereit
 * - Separate Configs für Main und Peer Device
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "SDCardHandler.h"

// Config-Dateien
#define CONFIG_FILE_MAIN    "/config_main.json"
#define CONFIG_FILE_PEER    "/config_peer.json"

class ConfigManager {
public:
    /**
     * Konstruktor
     * @param sdCard Referenz zum SDCardHandler
     * @param isPeer true = Peer Device, false = Main Device
     */
    ConfigManager(SDCardHandler& sdCard, bool isPeer = false);

    /**
     * Config initialisieren (laden oder Default erstellen)
     * @return true bei Erfolg
     */
    bool begin();

    /**
     * Config von SD-Karte laden
     * @return true bei Erfolg
     */
    bool load();

    /**
     * Config auf SD-Karte speichern
     * @return true bei Erfolg
     */
    bool save();

    /**
     * Config validieren
     * @return true wenn alle Werte gültig
     */
    bool validate();

    /**
     * Default-Config setzen
     */
    void setDefaults();

    /**
     * Ist Peer Device?
     */
    bool isPeerDevice() const { return isPeer; }

    // ═══════════════════════════════════════════════════════════════════════
    // CONFIG ZUGRIFF
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Main-Config abrufen (nur wenn !isPeer)
     */
    MainConfig& getMain() { return mainConfig; }
    const MainConfig& getMain() const { return mainConfig; }

    /**
     * Peer-Config abrufen (nur wenn isPeer)
     */
    PeerConfig& getPeer() { return peerConfig; }
    const PeerConfig& getPeer() const { return peerConfig; }

    /**
     * Debug-Info ausgeben
     */
    void printInfo();

private:
    SDCardHandler& sd;          // Referenz zum SD-Handler
    bool isPeer;                // Peer oder Main Device?
    
    MainConfig mainConfig;      // Main Device Config
    PeerConfig peerConfig;      // Peer Device Config
    
    /**
     * Main-Config laden
     */
    bool loadMainConfig();
    
    /**
     * Peer-Config laden
     */
    bool loadPeerConfig();
    
    /**
     * Main-Config speichern
     */
    bool saveMainConfig();
    
    /**
     * Peer-Config speichern
     */
    bool savePeerConfig();
    
    /**
     * Main-Config validieren
     */
    bool validateMainConfig();
    
    /**
     * Peer-Config validieren
     */
    bool validatePeerConfig();
};

// Globale Instanz (wird in .cpp definiert und in main.ino extern deklariert)
extern ConfigManager configManager;

#endif // CONFIG_MANAGER_H