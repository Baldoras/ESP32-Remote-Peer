/**
 * SDCardHandler.h
 * 
 * Einfacher SD-Karten Handler - NUR für File I/O
 * Keine Business-Logic, keine JSON-Serialisierung
 * 
 * Features:
 * - Mount/Unmount auf VSPI
 * - Datei lesen/schreiben/löschen
 * - Thread-safe Operationen
 */

#ifndef SD_CARD_HANDLER_H
#define SD_CARD_HANDLER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include "config.h"

class SDCardHandler {
public:
    /**
     * Konstruktor
     */
    SDCardHandler();

    /**
     * Destruktor
     */
    ~SDCardHandler();

    /**
     * SD-Karte initialisieren und mounten
     * @return true bei Erfolg
     */
    bool begin();

    /**
     * SD-Karte unmounten
     */
    void end();

    /**
     * Ist SD-Karte verfügbar?
     */
    bool isAvailable() const { return mounted; }

    /**
     * Freier Speicher in Bytes
     */
    uint64_t getFreeSpace();

    /**
     * Gesamter Speicher in Bytes
     */
    uint64_t getTotalSpace();

    /**
     * Verwendeter Speicher in Bytes
     */
    uint64_t getUsedSpace();

    // ═══════════════════════════════════════════════════════════════════════
    // FILE OPERATIONEN
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * Datei schreiben (überschreiben)
     * @param path Dateipfad
     * @param data Daten
     * @return true bei Erfolg
     */
    bool writeFile(const char* path, const char* data);

    /**
     * Datei schreiben (überschreiben, String)
     * @param path Dateipfad
     * @param data String-Daten
     * @return true bei Erfolg
     */
    bool writeFile(const char* path, const String& data);

    /**
     * Datei anhängen (append)
     * @param path Dateipfad
     * @param data Daten
     * @return true bei Erfolg
     */
    bool appendFile(const char* path, const char* data);

    /**
     * Datei anhängen (append, String)
     * @param path Dateipfad
     * @param data String-Daten
     * @return true bei Erfolg
     */
    bool appendFile(const char* path, const String& data);

    /**
     * Zeile anhängen (mit Newline)
     * @param path Dateipfad
     * @param line Zeile (ohne \n)
     * @return true bei Erfolg
     */
    bool appendLine(const char* path, const String& line);

    /**
     * Datei lesen (komplett)
     * @param path Dateipfad
     * @param buffer Buffer für Daten
     * @param maxLen Maximale Länge
     * @return Anzahl gelesener Bytes, -1 bei Fehler
     */
    int readFile(const char* path, char* buffer, size_t maxLen);

    /**
     * Datei lesen als String
     * @param path Dateipfad
     * @return String-Inhalt, leer bei Fehler
     */
    String readFileAsString(const char* path);

    /**
     * Datei löschen
     * @param path Dateipfad
     * @return true bei Erfolg
     */
    bool deleteFile(const char* path);

    /**
     * Datei umbenennen
     * @param oldPath Alter Pfad
     * @param newPath Neuer Pfad
     * @return true bei Erfolg
     */
    bool renameFile(const char* oldPath, const char* newPath);

    /**
     * Datei existiert?
     * @param path Dateipfad
     * @return true wenn vorhanden
     */
    bool fileExists(const char* path);

    /**
     * Dateigröße abrufen
     * @param path Dateipfad
     * @return Größe in Bytes, 0 bei Fehler
     */
    size_t getFileSize(const char* path);

    /**
     * Verzeichnis erstellen
     * @param path Verzeichnispfad
     * @return true bei Erfolg
     */
    bool createDir(const char* path);

    /**
     * Verzeichnis löschen
     * @param path Verzeichnispfad
     * @return true bei Erfolg
     */
    bool removeDir(const char* path);

    /**
     * Manuelles Flush (alle offenen Dateien)
     */
    void flush();

    /**
     * Debug-Info ausgeben
     */
    void printInfo();

private:
    bool mounted;               // Mount-Status
    SPIClass* vspi;             // VSPI-Bus Pointer
    
    /**
     * Card-Typ als String
     */
    const char* getCardTypeString(uint8_t cardType);
};

#endif // SD_CARD_HANDLER_H