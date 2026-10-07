#include <Arduino.h>
#include <ESP_8_BIT_GFX.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include <board_config.h>
#include <uptime.h>
#include "config/config_portal.h"
#include "mining/miner.h"
#include "stratum/stratum.h"

#ifndef APP_VERSION
#define APP_VERSION "dev"
#endif

// NTSC composite video on GPIO25, 256 x 240 pixels.
ESP_8_BIT_GFX videoOut(true, 8);

static uint32_t previousSampleMs = 0;
static uint64_t previousHashes = 0;
static constexpr uint8_t accent = 0x1F;  // Cyan in RGB332.
static constexpr uint8_t highlight = 0xFC;  // Yellow in RGB332.

static void drawHeader(const char *title, const char *subtitle) {
    videoOut.setTextColor(0xFF);
    videoOut.setTextSize(2);
    videoOut.setCursor(24, 27);
    videoOut.print(title);
    videoOut.setTextSize(1);
    videoOut.setTextColor(accent);
    videoOut.setCursor(25, 52);
    videoOut.print(subtitle);
    videoOut.fillRect(25, 64, 191, 2, accent);
    videoOut.fillRect(25, 64, 34, 2, highlight);
    videoOut.setTextColor(0xFF);
}

static void showStartupCredentials() {
    char apName[32];
    config_portal_ap_name(apName, sizeof(apName));
    videoOut.waitForFrame();
    videoOut.fillScreen(0);
    drawHeader("TOVARISH SATOSHI", APP_VERSION);
    videoOut.drawRoundRect(23, 76, 195, 82, 5, accent);
    videoOut.fillRect(24, 82, 3, 17, highlight);
    videoOut.setCursor(34, 82);
    videoOut.setTextColor(accent);
    videoOut.print("CONFIGURATION AP");
    videoOut.setTextColor(0xFF);
    videoOut.setCursor(34, 106);
    videoOut.print(apName);
    videoOut.setCursor(34, 128);
    videoOut.print("Password: ");
    videoOut.print(AP_PASSWORD);
}

static void showStartingStatus() {
    videoOut.setTextColor(highlight);
    videoOut.setCursor(25, 181);
    videoOut.print("Starting...");
}

static uint8_t hashrateColor(double hashesPerSecond) {
    double fraction = hashesPerSecond / 1000000.0;
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    // RGB332: red at low speed, yellow at half scale, green at full scale.
    const uint8_t red = fraction < 0.5 ? 7 : uint8_t((1.0 - fraction) * 14.0 + 0.5);
    const uint8_t green = fraction > 0.5 ? 7 : uint8_t(fraction * 14.0 + 0.5);
    return (red << 5) | (green << 2);
}

static void drawHashrateBar(double hashesPerSecond) {
    const int x = 207;
    const int y = 78;
    const int width = 9;
    const int height = 98;
    double fraction = hashesPerSecond / 1000000.0;
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    const int filled = int(fraction * (height - 2) + 0.5);
    videoOut.drawRect(x, y, width, height, 0xFF);
    if (filled > 0) {
        videoOut.fillRect(x + 1, y + height - 1 - filled,
                          width - 2, filled, hashrateColor(hashesPerSecond));
    }
    videoOut.setCursor(201, 66);
    videoOut.print("1M");
    videoOut.setCursor(208, 179);
    videoOut.print("0");
}

static void line(int y, const char *label, const char *value) {
    videoOut.setCursor(25, y);
    videoOut.setTextColor(0xFF);
    videoOut.print(label);
    videoOut.setCursor(112, y);
    videoOut.print(value);
}

static void showPortal(const char *ssid, const char *ip) {
    videoOut.waitForFrame();
    videoOut.fillScreen(0);
    drawHeader("SETUP", "CONNECT TO THE CONFIGURATION AP");
    videoOut.drawRoundRect(23, 76, 195, 95, 5, accent);
    videoOut.fillRect(25, 77, 34, 2, highlight);
    line(87, "WiFi AP", ssid);
    line(110, "Password", AP_PASSWORD);
    line(133, "Open", ip);
    videoOut.setTextColor(highlight);
    videoOut.setCursor(25, 185);
    videoOut.print("Set WiFi, wallet and pool.");
}

static void showMining() {
    const uint32_t now = uptime_ms();
    if (previousSampleMs && now - previousSampleMs < 5000) return;

    mining_stats_t *stats = miner_get_stats();
    const uint64_t hashes = stats->hashes;
    const double khs = previousSampleMs && hashes >= previousHashes
        ? double(hashes - previousHashes) / double(now - previousSampleMs)
        : 0.0;
    previousSampleMs = now;
    previousHashes = hashes;

    char value[44];
    videoOut.waitForFrame();
    videoOut.fillScreen(0);
    drawHeader("TOVARISH SATOSHI", APP_VERSION);
    videoOut.drawRoundRect(23, 71, 196, 116, 4, accent);
    videoOut.fillRect(25, 72, 34, 2, WiFi.status() == WL_CONNECTED ? accent : highlight);

    line(76, "WiFi", WiFi.status() == WL_CONNECTED ? "connected" : "offline");
    line(90, "Pool", miner_is_running() ? "mining" :
         (stratum_is_connected() ? "waiting for job" : "connecting"));
    snprintf(value, sizeof(value), "%.1f kH/s", khs);
    line(104, "Hashrate", value);
    snprintf(value, sizeof(value), "%llu", static_cast<unsigned long long>(hashes));
    line(118, "Hashes", value);
    snprintf(value, sizeof(value), "%lu / %lu", static_cast<unsigned long>(stats->accepted),
             static_cast<unsigned long>(stats->rejected));
    line(132, "Shares A/R", value);
    snprintf(value, sizeof(value), "%lu", static_cast<unsigned long>(stats->templates));
    line(146, "Jobs", value);
    snprintf(value, sizeof(value), "%.3f/%.3f", stats->bestDifficulty, miner_get_difficulty());
    line(160, "Best/pool", value);
    snprintf(value, sizeof(value), "%lu s", static_cast<unsigned long>(now / 1000));
    line(174, "Uptime", value);
    drawHashrateBar(khs * 1000.0);

    videoOut.fillRect(25, 188, 191, 2, accent);
    char pool[32];
    snprintf(pool, sizeof(pool), "%s", config_portal_settings().pool);
    videoOut.setCursor(25, 193);
    videoOut.setTextColor(highlight);
    videoOut.print(pool);
    videoOut.setCursor(25, 205);
    videoOut.setTextColor(0xFF);
    videoOut.print(WiFi.localIP());

    static uint32_t lastMemoryLog = 0;
    if (now - lastMemoryLog >= 10000) {
        lastMemoryLog = now;
        Serial.printf("[MEM] free=%u min=%u largest=%u\n",
                      ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getMaxAllocHeap());
    }
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.printf("[BOOT] TovarishSatoshi %s\n", APP_VERSION);
    videoOut.begin();
    config_portal_load();
    showStartupCredentials();
    delay(2500);
    showStartingStatus();
    config_portal_connect(showPortal);

    esp_task_wdt_init(30, true);
    miner_init();
    stratum_init();
    const MinerSettings &settings = config_portal_settings();
    stratum_set_pool(settings.pool, settings.port, settings.wallet,
                     settings.password, settings.worker);
    xTaskCreatePinnedToCore(stratum_task, "Stratum", STRATUM_STACK, nullptr,
                            STRATUM_PRIORITY, nullptr, STRATUM_CORE);
    // Leave core 0 for Wi-Fi and Stratum; composite video runs in loop() on core 1.
    xTaskCreatePinnedToCore(miner_task_core1, "Miner", MINER_1_STACK, nullptr,
                            MINER_1_PRIORITY, nullptr, MINER_1_CORE);
}

void loop() {
    showMining();
    delay(10);
}
