// Captive provisioning adapted from EasyMiner's WiFiManager configuration.
#include "config_portal.h"

#include <Preferences.h>
#include <WiFi.h>
#include <WiFiManager.h>

namespace {
MinerSettings settings = {};
WiFiManagerParameter *walletParam = nullptr;
WiFiManagerParameter *workerParam = nullptr;
WiFiManagerParameter *poolParam = nullptr;
WiFiManagerParameter *portParam = nullptr;
WiFiManagerParameter *passwordParam = nullptr;
void (*portalCallback)(const char *, const char *) = nullptr;

void saveParameters() {
    char *portEnd = nullptr;
    const long port = strtol(portParam->getValue(), &portEnd, 10);
    if (!walletParam->getValue()[0] || !poolParam->getValue()[0] ||
        portEnd == portParam->getValue() || *portEnd != '\0' || port < 1 || port > 65535) {
        Serial.println("[CONFIG] Wallet, pool host, and valid port are required");
        return;
    }

    snprintf(settings.wallet, sizeof(settings.wallet), "%s", walletParam->getValue());
    snprintf(settings.worker, sizeof(settings.worker), "%s", workerParam->getValue());
    snprintf(settings.pool, sizeof(settings.pool), "%s", poolParam->getValue());
    snprintf(settings.password, sizeof(settings.password), "%s", passwordParam->getValue());
    settings.port = static_cast<uint16_t>(port);
    if (!settings.worker[0]) {
        snprintf(settings.worker, sizeof(settings.worker), "%s", MINER_NAME);
    }

    Preferences store;
    if (store.begin("tovarish", false)) {
        store.putString("wallet", settings.wallet);
        store.putString("worker", settings.worker);
        store.putString("pool", settings.pool);
        store.putUShort("port", settings.port);
        store.putString("password", settings.password);
        store.end();
        Serial.println("[CONFIG] Mining settings saved");
    } else {
        Serial.println("[CONFIG] Cannot save mining settings");
    }
}

void onPortalStarted(WiFiManager *manager) {
    String ip = WiFi.softAPIP().toString();
    Serial.printf("[CONFIG] Connect to %s, then open http://%s/\n",
                  manager->getConfigPortalSSID().c_str(), ip.c_str());
    if (portalCallback) {
        portalCallback(manager->getConfigPortalSSID().c_str(), ip.c_str());
    }
}
}  // namespace

void config_portal_load() {
    snprintf(settings.pool, sizeof(settings.pool), "%s", DEFAULT_POOL_URL);
    snprintf(settings.password, sizeof(settings.password), "%s", DEFAULT_POOL_PASS);
    snprintf(settings.worker, sizeof(settings.worker), "%s", MINER_NAME);
    settings.port = DEFAULT_POOL_PORT;

    Preferences store;
    if (store.begin("tovarish", true)) {
        store.getString("wallet", settings.wallet, sizeof(settings.wallet));
        store.getString("worker", settings.worker, sizeof(settings.worker));
        store.getString("pool", settings.pool, sizeof(settings.pool));
        settings.port = store.getUShort("port", DEFAULT_POOL_PORT);
        store.getString("password", settings.password, sizeof(settings.password));
        store.end();
    }
    if (!settings.worker[0]) {
        snprintf(settings.worker, sizeof(settings.worker), "%s", MINER_NAME);
    }
    if (!settings.pool[0]) {
        snprintf(settings.pool, sizeof(settings.pool), "%s", DEFAULT_POOL_URL);
    }
    if (!settings.port) settings.port = DEFAULT_POOL_PORT;
}

void config_portal_ap_name(char *name, size_t size) {
    WiFi.mode(WIFI_STA);
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(name, size, "%s%02X%02X", AP_SSID_PREFIX, mac[4], mac[5]);
}

void config_portal_connect(void (*onPortal)(const char *, const char *)) {
    portalCallback = onPortal;
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    char portBuffer[8];
    snprintf(portBuffer, sizeof(portBuffer), "%u", settings.port);
    WiFiManagerParameter wallet("wallet", "Bitcoin address", settings.wallet, sizeof(settings.wallet));
    WiFiManagerParameter worker("worker", "Worker name", settings.worker, sizeof(settings.worker));
    WiFiManagerParameter pool("pool", "Pool host (without stratum+tcp://)", settings.pool, sizeof(settings.pool));
    WiFiManagerParameter port("port", "Pool port", portBuffer, sizeof(portBuffer));
    WiFiManagerParameter password("password", "Pool password", settings.password, sizeof(settings.password));
    walletParam = &wallet;
    workerParam = &worker;
    poolParam = &pool;
    portParam = &port;
    passwordParam = &password;

    WiFiManager manager;
    manager.setDebugOutput(false);
    manager.setConnectTimeout(20);
    manager.setConfigPortalTimeout(0);
    manager.setSaveParamsCallback(saveParameters);
    manager.setAPCallback(onPortalStarted);
    manager.addParameter(&wallet);
    manager.addParameter(&worker);
    manager.addParameter(&pool);
    manager.addParameter(&port);
    manager.addParameter(&password);

    char apName[32];
    config_portal_ap_name(apName, sizeof(apName));

    // A missing payout address forces provisioning even if Wi-Fi was saved before.
    if (settings.wallet[0]) {
        manager.autoConnect(apName, AP_PASSWORD);
    }
    while (!settings.wallet[0] || WiFi.status() != WL_CONNECTED) {
        manager.startConfigPortal(apName, AP_PASSWORD);
        delay(100);
    }
    Serial.printf("[WIFI] Connected to %s at %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

const MinerSettings &config_portal_settings() { return settings; }
