#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define LED_FILAMENT_GPIO     2
#define LEDC_TIMER            LEDC_TIMER_0
#define LEDC_MODE             LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL          LEDC_CHANNEL_0
#define LEDC_DUTY_RES         LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY        5000
#define LEDC_DUTY_CYCLE_MAX   (1 << LEDC_DUTY_RES)
#define LED_FADE_TIME_MS      1000

static const char *TAG = "led_filament";

static void led_filament_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = LED_FILAMENT_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
        .flags.output_invert = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_ERROR_CHECK(ledc_fade_func_install(0));
}

static void led_filament_set_brightness(uint32_t brightness_percent)
{
    uint32_t duty_cycle = (brightness_percent * LEDC_DUTY_CYCLE_MAX) / 100;
    duty_cycle = duty_cycle > LEDC_DUTY_CYCLE_MAX ? LEDC_DUTY_CYCLE_MAX : duty_cycle;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

static void led_filament_fade_in_out_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting LED filament glow effect");

    while (1) {
        ESP_LOGI(TAG, "Fading in...");
        ledc_set_fade_time_and_start(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY_CYCLE_MAX * 0.8, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);

        vTaskDelay(pdMS_TO_TICKS(500));

        ESP_LOGI(TAG, "Fading out...");
        ledc_set_fade_time_and_start(LEDC_MODE, LEDC_CHANNEL, 0, LED_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "LED Filament Controller starting...");

    led_filament_init();

    xTaskCreate(led_filament_fade_in_out_task, "led_filament_glow", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "LED Filament Controller initialized successfully");
}
