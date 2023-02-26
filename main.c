#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "irq.h"

#define BTN_PIN                 GPIO_PIN(PORT_C, 0)
// Pedestrians traffic light
#define PEDESTRIAN_LED_RED      GPIO_PIN(PORT_D, 2)
#define PEDESTRIAN_LED_GREEN    GPIO_PIN(PORT_D, 3)
// Cars traffic lights
#define CAR_LED_GREEN           GPIO_PIN(PORT_B, 2)
#define CAR_LED_YELLOW          GPIO_PIN(PORT_B, 1)
#define CAR_LED_RED             GPIO_PIN(PORT_B, 0)

#define CARS_MOVING_TIME 30
#define PEDS_MOVING_TIME 5
#define TOGGLE_TIMEOUT 10

void toggleToPedestrain(void) {
    // Blink green
    for (int i = 0; i < 3; i++) {
        gpio_clear(CAR_LED_GREEN);
        xtimer_msleep(300);
        gpio_set(CAR_LED_GREEN);
        xtimer_msleep(300);
    }
    // Turn on yellow
    gpio_set(CAR_LED_YELLOW);
    gpio_clear(CAR_LED_GREEN);

    // Turn on red for cars and turn green for peds
    xtimer_msleep(1000);
    gpio_clear(CAR_LED_YELLOW);
    gpio_set(CAR_LED_RED);
    gpio_clear(PEDESTRIAN_LED_RED);
    gpio_set(PEDESTRIAN_LED_GREEN);
}

void toggleToCars(void) {
    for (int i = 0; i < 3; i++) {
        gpio_clear(PEDESTRIAN_LED_GREEN);
        xtimer_msleep(300);
        gpio_set(PEDESTRIAN_LED_GREEN);
        xtimer_msleep(300);
    }
    gpio_clear(PEDESTRIAN_LED_GREEN);
    gpio_set(PEDESTRIAN_LED_RED);
    gpio_clear(CAR_LED_RED);
    gpio_set(CAR_LED_GREEN);
}

volatile bool carsMoving = true;
volatile uint8_t timeLeft = CARS_MOVING_TIME;

void button_handler(void* arg) {
    if (timeLeft > TOGGLE_TIMEOUT && carsMoving) {
        timeLeft = TOGGLE_TIMEOUT;
    }
    (void)arg;
}

int main(void) {
    int res = gpio_init_int(BTN_PIN,
        GPIO_IN,
        GPIO_FALLING,
        button_handler, NULL
    );
    printf("Interupt result: %d\r\n", res);
    gpio_init(CAR_LED_RED, GPIO_OUT);
    gpio_init(CAR_LED_YELLOW, GPIO_OUT);
    gpio_init(CAR_LED_GREEN, GPIO_OUT);
    gpio_init(PEDESTRIAN_LED_RED, GPIO_OUT);
    gpio_init(PEDESTRIAN_LED_GREEN, GPIO_OUT);
    gpio_clear(CAR_LED_RED);
    gpio_clear(CAR_LED_YELLOW);
    gpio_set(CAR_LED_GREEN);
    gpio_clear(PEDESTRIAN_LED_GREEN);
    gpio_set(PEDESTRIAN_LED_RED);
    gpio_irq_enable(BTN_PIN);

    while (true) {
        xtimer_msleep(1000);
        timeLeft--;
        if (timeLeft == 0) {
            if (carsMoving) {
                carsMoving = false;
                toggleToPedestrain();
                timeLeft = PEDS_MOVING_TIME;
            }
            else {
                carsMoving = true;
                toggleToCars();
                timeLeft = CARS_MOVING_TIME;
            }
        }
    }
}
