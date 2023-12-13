#include "primary.h"
#include <iomanip>

#define LED_STRIP_LENGTH 23U
#define LED_STRIP_RMT_INTR_NUM 19U

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

void primaryLoop(void *pvParameters)
{

    dataStore pData;

    pData.curTime = 0;
    pData.lastDrinkT = 0;
    pData.weight = 0;

    xTaskCreate(&timer, "Start_Timer", 1024, &pData, 5, NULL);
    // xTaskCreate(&weightUpdate, "WeightUpdate", 2048, &pData, 3, NULL);
    xTaskCreate(&ledControl, "LedControl", 2048, &pData, 3, NULL);

    while (1)
    {
        //cout << "Current Time: " << pData.curTime << '\n';
        //cout << "Last Drink: " << pData.lastDrinkT << '\n';
        if (mass != 0.000) 
        {
            cout << fixed << "Current Weight: " << setprecision(3) << mass << '\n';
        }
        if (mass < pData.weight - 0.200) {
            pData.curTime = 0;
        }
        pData.weight = mass;
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

/* updates LED's depending on last drink time */
void ledControl(void *pvParameters)
{
    dataStore *dataPtr = (dataStore *)pvParameters;

    /* Define LED */
    struct led_strip_t led_strip = {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .led_strip_length = LED_STRIP_LENGTH,
        .rmt_channel = RMT_CHANNEL_1,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = GPIO_NUM_15,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2};
    led_strip.access_semaphore = xSemaphoreCreateBinary();

    bool led_init_ok = led_strip_init(&led_strip);
    std::cout << (led_init_ok ? "LED Init ok" : "LED Init fail") << std::endl;

    /* init color struct */
    led_color_t color = {
        .red = 0,
        .green = 0,
        .blue = 0
        };

    struct soonTMcolor
    {
        int red;
        int green;
        int blue;
        int redIncrease;
        int greenIncrease;
        int blueIncrease;
    } color_to_be;

    color_to_be.red = 0;
    color_to_be.green = 0;
    color_to_be.blue = 0;

    bool reverse = false;

    while (1)
    {
        int pulseDelay = 105;

        // Cylon effect while calibrating
        while (mass == 0) {
            for(int i = 0; i < LED_STRIP_LENGTH; ++i) {
                color.red = 0;
                color.green = 0;
                color.blue = 230;
                led_strip_set_pixel_color(&led_strip, i, &color);
                led_strip_show(&led_strip);
                vTaskDelay(30 / portTICK_RATE_MS);
            }
        }

        
        while (dataPtr->curTime > (dataPtr->lastDrinkT + 4) && mass > 0.150)
        {
            /* set led state 45 min since last drink. You are going to die */
            for (int i = 0; i < LED_STRIP_LENGTH; i++)
            {
                led_strip_set_pixel_color(&led_strip, i, &color);
            }

            color.red += reverse ? -1 : 1;

            if (color.red > 200 || color.red < 40)
            {
                reverse = !reverse;
            }

            led_strip_show(&led_strip);
            vTaskDelay(15 / portTICK_RATE_MS);
        }
        if (dataPtr->curTime > (dataPtr->lastDrinkT + 4))
        {
            /* set led state 40 min since last drink */
            color_to_be.red = 200;
            color_to_be.green = 0;
            color_to_be.blue = 0;
        }
        else if (dataPtr->curTime > (dataPtr->lastDrinkT + 3))
        {
            /* set led state 35 min since last drink */
            color_to_be.red = 140;
            color_to_be.green = 0;
            color_to_be.blue = 0;
        }
        else if (dataPtr->curTime > (dataPtr->lastDrinkT + 2))
        {
            /* set led state 30 min since last drink */
            color_to_be.red = 65;
            color_to_be.green = 30;
            color_to_be.blue = 0;
        }
        else if (dataPtr->curTime > (dataPtr->lastDrinkT + 1))
        {
            /* set led state 15 min since last drink */
            color_to_be.red = 55;
            color_to_be.green = 40;
            color_to_be.blue = 0;
        }
        else
        {
            /* set initial led state / when recently drunk */
            color_to_be.red = 0;
            color_to_be.green = 30;
            color_to_be.blue = 0;
        }

        if (mass < 0.150 && mass > -1.0)
        {
            dataPtr->curTime = 0;
            dataPtr->lastDrinkT = 0;

            color_to_be.red = 0;
            color_to_be.green = 30;
            color_to_be.blue = 0;
        }

        dataPtr->weight = mass;

        color_to_be.redIncrease = (color_to_be.red > color.red ? 1 : -1);
        color_to_be.greenIncrease = (color_to_be.green > color.green ? 1 : -1);
        color_to_be.blueIncrease = (color_to_be.blue > color.blue ? 1 : -1);

        while (color.red != color_to_be.red || color.green != color_to_be.green || color.blue != color_to_be.blue)
        {
            color.red != color_to_be.red ? color.red += color_to_be.redIncrease : NULL;
            color.green != color_to_be.green ? color.green += color_to_be.greenIncrease : NULL;
            color.blue != color_to_be.blue ? color.blue += color_to_be.blueIncrease : NULL;

            // cout << "Current red: " << color.red << '\n';
            // cout << "Current green: " << color.green << '\n';
            // cout << "Current blue: " << color.blue << '\n';

            for (int i = 0; i < LED_STRIP_LENGTH; i++)
            {
                led_strip_set_pixel_color(&led_strip, i, &color);
            }

            led_strip_show(&led_strip);
            vTaskDelay(30 / portTICK_RATE_MS);
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

/* waddafaaaq */
void weightUpdate(void *pvParameters)
{
    dataStore *dataPtr = (dataStore *)pvParameters;
    int lastWeight = 0;

    if (lastWeight == 0 || dataPtr->weight < (lastWeight - 500) || dataPtr->weight > (lastWeight + 500))
    {
        lastWeight = dataPtr->weight;
        int tmp = dataPtr->curTime;
        dataPtr->lastDrinkT = tmp;
    }
}

/* A timer that updates dataPtr->curTime giving it +1 every 30 seconds */
void timer(void *pvParameters)
{

    dataStore *dataPtr = (dataStore *)pvParameters;

    /* A Manual timer, increasing current time value by 1 every 30 seconds */
    while (1)
    {
        /* Set Delay to 8000 for Presentation */
        vTaskDelay(320000 / portTICK_RATE_MS);
        dataPtr->curTime += 1;
    }
}
