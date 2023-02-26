#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "irq.h"

#define toggleButton GPIO_PIN(PORT_C, 0)

// Cars traffic lights
#define mainRed GPIO_PIN(PORT_B, 0)
#define mainYellow GPIO_PIN(PORT_B, 1)
#define mainGreen GPIO_PIN(PORT_B, 2)

// Pedestrians traffic light
#define pedRed GPIO_PIN(PORT_D, 2)
#define pedGreen GPIO_PIN(PORT_D, 3)

#define CARS_MOVING_TIME 30
#define PEDS_MOVING_TIME 5
#define TOGGLE_TIMEOUT 10

void toggleToPedestrain(void) {
    // Blink green
    for (int i = 0; i < 3; i++) {
        gpio_clear(mainGreen);
        xtimer_msleep(300);
        gpio_set(mainGreen);
        xtimer_msleep(300);
    }

    // Turn on yellow
    gpio_set(mainYellow);
    gpio_clear(mainGreen);

    // Turn on red for cars and turn green for peds
    xtimer_msleep(1000);
    gpio_clear(mainYellow);
    gpio_set(mainRed);
    gpio_clear(pedRed);
    gpio_set(pedGreen);
}

void toggleToCars(void) {
    for (int i = 0; i < 3; i++) {
        gpio_clear(pedGreen);
        xtimer_msleep(300);
        gpio_set(pedGreen);
        xtimer_msleep(300);
    }
    gpio_clear(pedGreen);
    gpio_set(pedRed);

    xtimer_msleep(500);
    gpio_clear(mainRed);
    gpio_set(mainGreen);
}
volatile bool carsMoving = true;
volatile uint8_t timeLeft = CARS_MOVING_TIME;

void button_handler(void* arg) {
    if (timeLeft > TOGGLE_TIMEOUT && carsMoving) {
        timeLeft = TOGGLE_TIMEOUT;
    }
    (void) arg;
}

int main(void) {
    gpio_init_int(toggleButton,
        GPIO_IN,
        GPIO_BOTH,
        button_handler, NULL
    );
    gpio_init(mainRed, GPIO_OUT);
    gpio_init(mainYellow, GPIO_OUT);
    gpio_init(mainGreen, GPIO_OUT);
    gpio_init(pedRed, GPIO_OUT);
    gpio_init(pedGreen, GPIO_OUT);
    gpio_clear(mainRed);
    gpio_clear(mainYellow);
    gpio_set(mainGreen);
    gpio_clear(pedGreen);
    gpio_set(pedRed);
    gpio_irq_enable(toggleButton);

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
