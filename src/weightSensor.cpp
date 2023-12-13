#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <weightSensor.h>


#define PD_SCK_GPIO GPIO_NUM_18
#define DOUT_GPIO GPIO_NUM_19

extern double mass = 0;

void checkWeight(void *pvParameters)
{
    gpio_set_direction(DOUT_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(DOUT_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_direction(PD_SCK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DOUT_GPIO, 1);
    vTaskDelay(500 / portTICK_RATE_MS);

    const double y1 = 0.5;
    int32_t x1 = 9280900;
    // x1 = 9280891
    int32_t x0 = 0;
    // x0 = 9277942

// Read the Raw Data
    x0 = readData();

// Add 500 grams (value set on y1) to calibrate the Load Cell
    // printf("\nAdd calibration mass\n");
    // do {
    //     x1 = readData();
    //     vTaskDelay(3000 / portTICK_RATE_MS);
    // } while(x1 < x0+800);

    // printf("\nCalibration Complete\n\n");
    // printf("x0: %d\nx1: %d\n\n", x0, x1);

// Read the Data and Calculates it into kg
    while (1)
    {
        int32_t avgValue = 0;
        avgValue = readData();
        double ratio1 = (double)(avgValue - x0);
        double ratio2 = (double)(x1-x0);
        double ratio = ratio1 / ratio2;
        if (y1*ratio < -0.4 || y1*ratio > 2.5) {
            printf("invalid mass: %.3f\n", y1*ratio);
        } else {
            mass = y1*ratio;
        }
        //printf("Value: %.3f kg\n", mass);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

int32_t readData() {
    int32_t avgValue = 0;
    while (gpio_get_level(DOUT_GPIO))
                ;
    for (int8_t i = 0; i <= 10; ++i)
    {
        uint32_t value = 0;
        for (int8_t bit = 0; bit < 24; ++bit)
        {
            gpio_set_level(PD_SCK_GPIO, 1);
            ets_delay_us(1);
            value = value << 1;
            if (gpio_get_level(DOUT_GPIO))
            {
                ++value;
            }
            gpio_set_level(PD_SCK_GPIO, 0);
            ets_delay_us(1);
        }
        gpio_set_level(PD_SCK_GPIO, 1);
        avgValue += value ^ 0x800000;
        gpio_set_level(PD_SCK_GPIO, 0);
    }
    return avgValue /= 10;
}