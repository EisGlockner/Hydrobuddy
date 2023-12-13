#include <stdio.h>
#include "main.h"
#include "systeminfo.h"
#include "weightSensor.h"
#include "primary.h"
#include "led_strip.h"

extern "C"
{
    void app_main();
}

void app_main()
{
    xTaskCreate(&checkWeight, "Read_Weight", 2048, NULL, 2, NULL);
    xTaskCreate(&primaryLoop, "do stuff", 2048, NULL, 2, NULL);

    /* Print chip Information */
    // esp_systeminfo info;
    // info.print();
}

/* DEPRECATED CODE GOES HERE (btw nobody should ever do this wtf..)

    // Required for next part (different logging)
    #define LOG_LEVEL_LOCAL ESP_LOG_VEBOSE
    #include "esp_log.h"
    #define LOG_TAG "MAIN"
    

    // For different way of logging
    while (true){
    ESP_LOGI(LOG_TAG, "HELLO WORLD!");
    vTaskDelay(300);
    } 
    

    // xTaskCreate(&ledctrl.blink(1000), "led_control", 512, NULL, 5, NULL);

    //Print messages and restart                   // Left here for reference
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart(); 
    
*/