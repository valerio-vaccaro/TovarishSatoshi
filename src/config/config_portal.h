#pragma once

#include <Arduino.h>
#include <board_config.h>

struct MinerSettings {
    char wallet[MAX_WALLET_LEN + 1];
    char worker[32];
    char pool[MAX_POOL_URL_LEN + 1];
    uint16_t port;
    char password[MAX_PASSWORD_LEN + 1];
};

void config_portal_load();
void config_portal_ap_name(char *name, size_t size);
void config_portal_connect(void (*onPortal)(const char *ssid, const char *ip));
const MinerSettings &config_portal_settings();
