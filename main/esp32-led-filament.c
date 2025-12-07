#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define YELLOW_FILAMENT_GPIO   2
#define BLUE_FILAMENT_GPIO     3
#define LEDC_TIMER             LEDC_TIMER_0
#define LEDC_MODE              LEDC_LOW_SPEED_MODE
#define YELLOW_LEDC_CHANNEL    LEDC_CHANNEL_0
#define BLUE_LEDC_CHANNEL      LEDC_CHANNEL_1
#define LEDC_DUTY_RES          LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY         5000
#define LEDC_DUTY_CYCLE_MAX    (1 << LEDC_DUTY_RES)
#define LED_FADE_TIME_MS       1000

static const char *TAG = "led_filament";

static void led_filaments_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t yellow_channel = {
        .channel = YELLOW_LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = YELLOW_FILAMENT_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
        .flags.output_invert = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&yellow_channel));

    ledc_channel_config_t blue_channel = {
        .channel = BLUE_LEDC_CHANNEL,
        .duty = LEDC_DUTY_CYCLE_MAX * 0.8,
        .gpio_num = BLUE_FILAMENT_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
        .flags.output_invert = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&blue_channel));

    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, BLUE_LEDC_CHANNEL, LEDC_DUTY_CYCLE_MAX * 0.8));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, BLUE_LEDC_CHANNEL));
}

static void led_filaments_inverse_glow_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting dual LED filament inverse glow effect");

    while (1) {
        ESP_LOGI(TAG, "Yellow fading in, Blue fading out...");

        ledc_set_fade_time_and_start(LEDC_MODE, YELLOW_LEDC_CHANNEL, LEDC_DUTY_CYCLE_MAX * 0.8, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);
        ledc_set_fade_time_and_start(LEDC_MODE, BLUE_LEDC_CHANNEL, 0, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);

        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Yellow fading out, Blue fading in...");

        ledc_set_fade_time_and_start(LEDC_MODE, YELLOW_LEDC_CHANNEL, 0, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);
        ledc_set_fade_time_and_start(LEDC_MODE, BLUE_LEDC_CHANNEL, LEDC_DUTY_CYCLE_MAX * 0.8, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Dual LED Filament Controller starting...");

    led_filaments_init();

    xTaskCreate(led_filaments_inverse_glow_task, "dual_filament_glow", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Dual LED Filament Controller initialized successfully");
}
