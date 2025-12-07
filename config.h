/**
 * config.h
 * 
 * Konfigurationsdatei für ESP32-S3 mit SD-Karte
 * 
 * Hardware:
 * - ESP32-S3-N16R8
 * - SD-Karte Reader auf VSPI
 */

#ifndef CONFIG_H
#define CONFIG_H

// ═══════════════════════════════════════════════════════════════════════════
// SD-KARTE PINS (VSPI - eigener Bus!)
// ═══════════════════════════════════════════════════════════════════════════

#define SD_CS       38    // SD-Karte Chip Select (LOW = aktiv)
#define SD_MOSI     40    // SD MOSI (VSPI)
#define SD_MISO     41    // SD MISO (VSPI)
#define SD_SCK      39    // SD SCK (VSPI)

// ═══════════════════════════════════════════════════════════════════════════
// SPANNUNGSSENSOR (0-25V Modul, 2S LiPo Messung mit Auto-Shutdown)
// ═══════════════════════════════════════════════════════════════════════════

#define VOLTAGE_SENSOR_PIN    4         // Analog OUT vom Spannungssensor-Modul (GPIO4)
#define VOLTAGE_RANGE_MAX     25.0      // Modul-Maximum (Hardware-Limit)
#define VOLTAGE_BATTERY_MIN   13.2      // 4S LiPo leer (3.3V/Zelle)
#define VOLTAGE_BATTERY_MAX   16.8      // 4S LiPo voll (4.2V/Zelle)
#define VOLTAGE_BATTERY_NOM   14.8      // 4S LiPo nominal (3.7V/Zelle)
#define VOLTAGE_ALARM_LOW     13.6      // Warnung bei <13.6V (3.5V/Zelle)
#define VOLTAGE_SHUTDOWN      13.2      // AUTO-SHUTDOWN bei 13.2V! ⚠️
#define VOLTAGE_CALIBRATION_FACTOR  0.7 // Calibration factor
#define VOLTAGE_CHECK_INTERVAL 1000     // Spannungs-Check alle 1000ms

// ═══════════════════════════════════════════════════════════════════════════
// ESP-NOW EINSTELLUNGEN
// ═══════════════════════════════════════════════════════════════════════════

#define ESPNOW_MAX_PEERS          1           // Maximale Anzahl Peers
#define ESPNOW_CHANNEL            0           // WiFi-Kanal (0 = auto)
#define ESPNOW_HEARTBEAT_INTERVAL 500         // Heartbeat alle 500ms
#define ESPNOW_TIMEOUT_MS         2000        // Verbindungs-Timeout 2s
#define ESPNOW_MAIN_DEVICE_MAC    "10:20:BA:4D:6C:E4"     // Peer MAC

// ═══════════════════════════════════════════════════════════════════════════
// DEBUG EINSTELLUNGEN
// ═══════════════════════════════════════════════════════════════════════════

#define DEBUG_SERIAL        true    // Debug-Ausgaben aktivieren
#define SERIAL_BAUD_RATE    115200  // Serielle Baudrate

// Debug-Makros
#if DEBUG_SERIAL
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINTF(...)  Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// ═══════════════════════════════════════════════════════════════════════════
// SYSTEM EINSTELLUNGEN
// ═══════════════════════════════════════════════════════════════════════════

#define DEBOUNCE_DELAY      50     // Entprell-Zeit in ms
#define SD_MOUNT_POINT     "/sd"   // Mount-Punkt für SD-Karte
#define SD_MAX_FILES       10      // Maximale Anzahl offener Dateien

// ═══════════════════════════════════════════════════════════════════════════
// FEHLERCODES
// ═══════════════════════════════════════════════════════════════════════════

enum ErrorCode {
    ERR_NONE = 0,
    ERR_DISPLAY_INIT = 1,
    ERR_TOUCH_INIT = 2,
    ERR_SD_INIT = 3,
    ERR_SD_MOUNT = 4,
    ERR_FILE_OPEN = 5,
    ERR_FILE_WRITE = 6,
    ERR_FILE_READ = 7,
    ERR_BATTERY_INIT = 8,
    ERR_BATTERY_CRITICAL = 9
};

// ═══════════════════════════════════════════════════════════════════════════
// VERSION INFO
// ═══════════════════════════════════════════════════════════════════════════

#define FIRMWARE_VERSION "0.1.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#endif // CONFIG_H
