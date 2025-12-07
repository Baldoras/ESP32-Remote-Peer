/**
 * SDCardHandler.cpp
 * 
 * Einfache SD-Karten Handler Implementation - NUR File I/O
 */

#include "SDCardHandler.h"

SDCardHandler::SDCardHandler()
    : mounted(false)
    , vspi(nullptr)
{
}

SDCardHandler::~SDCardHandler() {
    end();
}

bool SDCardHandler::begin() {
    DEBUG_PRINTLN("SDCardHandler: Initialisiere SD-Karte...");
    
    // VSPI initialisieren
    vspi = new SPIClass(FSPI);
    vspi->begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    
    // SD-Karte mounten
    if (!SD.begin(SD_CS, *vspi, SD_SPI_FREQUENCY)) {
        DEBUG_PRINTLN("SDCardHandler: ❌ Mount fehlgeschlagen!");
        delete vspi;
        vspi = nullptr;
        return false;
    }
    
    mounted = true;
    
    // Card-Typ prüfen
    uint8_t cardType = SD.cardType();
    
    if (cardType == CARD_NONE) {
        DEBUG_PRINTLN("SDCardHandler: ❌ Keine SD-Karte erkannt!");
        end();
        return false;
    }
    
    DEBUG_PRINTLN("SDCardHandler: ✅ SD-Karte gemountet");
    DEBUG_PRINTF("  Typ: %s\n", getCardTypeString(cardType));
    DEBUG_PRINTF("  Größe: %.2f GB\n", getTotalSpace() / 1024.0 / 1024.0 / 1024.0);
    DEBUG_PRINTF("  Frei: %.2f GB\n", getFreeSpace() / 1024.0 / 1024.0 / 1024.0);
    
    return true;
}

void SDCardHandler::end() {
    if (mounted) {
        flush();
        SD.end();
        mounted = false;
        DEBUG_PRINTLN("SDCardHandler: SD-Karte unmounted");
    }
    
    if (vspi) {
        delete vspi;
        vspi = nullptr;
    }
}

uint64_t SDCardHandler::getFreeSpace() {
    if (!mounted) return 0;
    return SD.totalBytes() - SD.usedBytes();
}

uint64_t SDCardHandler::getTotalSpace() {
    if (!mounted) return 0;
    return SD.totalBytes();
}

uint64_t SDCardHandler::getUsedSpace() {
    if (!mounted) return 0;
    return SD.usedBytes();
}

// ═══════════════════════════════════════════════════════════════════════════
// FILE OPERATIONEN
// ═══════════════════════════════════════════════════════════════════════════

bool SDCardHandler::writeFile(const char* path, const char* data) {
    if (!mounted || !path || !data) return false;
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        DEBUG_PRINTF("SDCardHandler: ❌ Kann Datei nicht öffnen: %s\n", path);
        return false;
    }
    
    size_t written = file.print(data);
    file.close();
    
    return written > 0;
}

bool SDCardHandler::writeFile(const char* path, const String& data) {
    return writeFile(path, data.c_str());
}

bool SDCardHandler::appendFile(const char* path, const char* data) {
    if (!mounted || !path || !data) return false;
    
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        DEBUG_PRINTF("SDCardHandler: ❌ Kann Datei nicht öffnen: %s\n", path);
        return false;
    }
    
    size_t written = file.print(data);
    file.close();
    
    return written > 0;
}

bool SDCardHandler::appendFile(const char* path, const String& data) {
    return appendFile(path, data.c_str());
}

bool SDCardHandler::appendLine(const char* path, const String& line) {
    String lineWithNewline = line + "\n";
    return appendFile(path, lineWithNewline);
}

int SDCardHandler::readFile(const char* path, char* buffer, size_t maxLen) {
    if (!mounted || !path || !buffer) return -1;
    
    File file = SD.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTF("SDCardHandler: ❌ Kann Datei nicht lesen: %s\n", path);
        return -1;
    }
    
    size_t len = file.size();
    if (len > maxLen) len = maxLen;
    
    size_t bytesRead = file.readBytes(buffer, len);
    file.close();
    
    return bytesRead;
}

String SDCardHandler::readFileAsString(const char* path) {
    if (!mounted || !path) return String();
    
    File file = SD.open(path, FILE_READ);
    if (!file) {
        DEBUG_PRINTF("SDCardHandler: ❌ Kann Datei nicht lesen: %s\n", path);
        return String();
    }
    
    String content = file.readString();
    file.close();
    
    return content;
}

bool SDCardHandler::deleteFile(const char* path) {
    if (!mounted || !path) return false;
    
    if (!SD.exists(path)) {
        DEBUG_PRINTF("SDCardHandler: Datei existiert nicht: %s\n", path);
        return false;
    }
    
    bool success = SD.remove(path);
    
    if (success) {
        DEBUG_PRINTF("SDCardHandler: ✅ Datei gelöscht: %s\n", path);
    } else {
        DEBUG_PRINTF("SDCardHandler: ❌ Löschen fehlgeschlagen: %s\n", path);
    }
    
    return success;
}

bool SDCardHandler::renameFile(const char* oldPath, const char* newPath) {
    if (!mounted || !oldPath || !newPath) return false;
    
    if (!SD.exists(oldPath)) {
        DEBUG_PRINTF("SDCardHandler: Datei existiert nicht: %s\n", oldPath);
        return false;
    }
    
    bool success = SD.rename(oldPath, newPath);
    
    if (success) {
        DEBUG_PRINTF("SDCardHandler: ✅ Datei umbenannt: %s -> %s\n", oldPath, newPath);
    } else {
        DEBUG_PRINTF("SDCardHandler: ❌ Umbenennen fehlgeschlagen: %s\n", oldPath);
    }
    
    return success;
}

bool SDCardHandler::fileExists(const char* path) {
    if (!mounted || !path) return false;
    return SD.exists(path);
}

size_t SDCardHandler::getFileSize(const char* path) {
    if (!mounted || !path) return 0;
    
    File file = SD.open(path, FILE_READ);
    if (!file) return 0;
    
    size_t size = file.size();
    file.close();
    
    return size;
}

bool SDCardHandler::createDir(const char* path) {
    if (!mounted || !path) return false;
    
    bool success = SD.mkdir(path);
    
    if (success) {
        DEBUG_PRINTF("SDCardHandler: ✅ Verzeichnis erstellt: %s\n", path);
    } else {
        DEBUG_PRINTF("SDCardHandler: ❌ Verzeichnis erstellen fehlgeschlagen: %s\n", path);
    }
    
    return success;
}

bool SDCardHandler::removeDir(const char* path) {
    if (!mounted || !path) return false;
    
    bool success = SD.rmdir(path);
    
    if (success) {
        DEBUG_PRINTF("SDCardHandler: ✅ Verzeichnis gelöscht: %s\n", path);
    } else {
        DEBUG_PRINTF("SDCardHandler: ❌ Verzeichnis löschen fehlgeschlagen: %s\n", path);
    }
    
    return success;
}

void SDCardHandler::flush() {
    if (!mounted) return;
    // SD.flush() falls verfügbar in neueren Arduino-Core Versionen
}

void SDCardHandler::printInfo() {
    DEBUG_PRINTLN("\n╔═══════════════════════════════════════════════╗");
    DEBUG_PRINTLN("║          SD CARD HANDLER INFO                 ║");
    DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝");
    
    DEBUG_PRINTF("Status:     %s\n", mounted ? "✅ Mounted" : "❌ Not mounted");
    
    if (mounted) {
        uint8_t cardType = SD.cardType();
        
        DEBUG_PRINTF("Card Type:  %s\n", getCardTypeString(cardType));
        
        uint64_t total = getTotalSpace();
        uint64_t free = getFreeSpace();
        uint64_t used = getUsedSpace();
        
        DEBUG_PRINTF("Total:      %.2f GB\n", total / 1024.0 / 1024.0 / 1024.0);
        DEBUG_PRINTF("Used:       %.2f GB\n", used / 1024.0 / 1024.0 / 1024.0);
        DEBUG_PRINTF("Free:       %.2f GB (%.1f%%)\n", 
                     free / 1024.0 / 1024.0 / 1024.0,
                     (free * 100.0) / total);
    }
    
    DEBUG_PRINTLN("═══════════════════════════════════════════════\n");
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE METHODEN
// ═══════════════════════════════════════════════════════════════════════════

const char* SDCardHandler::getCardTypeString(uint8_t cardType) {
    switch (cardType) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "UNKNOWN";
    }
}