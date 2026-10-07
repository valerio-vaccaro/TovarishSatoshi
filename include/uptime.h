#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ESP timer can jump when composite video and Wi-Fi run together on this board.
// FreeRTOS ticks provide a stable elapsed-time source for miner timeouts.
inline uint32_t uptime_ms() {
    return static_cast<uint32_t>(xTaskGetTickCount()) * portTICK_PERIOD_MS;
}
