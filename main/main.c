#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/ledc.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024

#define SERVO_GPIO 18

// Servo pulse width (for 16-bit resolution at 50Hz)
#define SERVO_MIN_DUTY 1638   // ~1ms
#define SERVO_MAX_DUTY 8192   // ~2ms

void servo_init() {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,  // Servo frequency
        .duty_resolution = LEDC_TIMER_16_BIT,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel);
}

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,   // MUST match Python
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
}

void set_servo_angle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    int duty = SERVO_MIN_DUTY +
               ((SERVO_MAX_DUTY - SERVO_MIN_DUTY) * angle / 180);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    printf("Angle: %d -> Duty: %d\n", angle, duty);
}

void app_main(void) {
    uart_init();
    servo_init();

    uint8_t data[BUF_SIZE];

    printf("Servo ready. Waiting for angle...\n");

    while (1) {
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, 100 / portTICK_PERIOD_MS);

        if (len > 0) {
            data[len] = '\0';

            int angle = atoi((char*)data);

            set_servo_angle(angle);
        }
    }
}