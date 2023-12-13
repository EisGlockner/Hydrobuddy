#ifndef PRIMARY_H_
#define PRIMARY_H_

#include <atomic>
#include <iostream>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "led_strip.h"
#include "weightSensor.h"

using namespace std;

void primaryLoop(void *pvParameters);
void timer(void *pvParameters);
void weightUpdate(void *pvParameters);
void ledControl(void *pvParameters);

struct dataStore
{
    atomic<int> curTime;
    atomic<int> lastDrinkT;
    atomic<int> weight;
};

#endif