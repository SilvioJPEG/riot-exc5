#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

//TODO: replace pins
#define BTN_PIN                 GPIO_PIN(PORT_C, 0)

#define PEDESTRIAN_LED_RED      GPIO_PIN(PORT_D, 2)
#define PEDESTRIAN_LED_GREEN    GPIO_PIN(PORT_D, 3)

#define CAR_LED_GREEN           GPIO_PIN(PORT_B, 2)
#define CAR_LED_YELLOW          GPIO_PIN(PORT_B, 1)
#define CAR_LED_RED             GPIO_PIN(PORT_B, 0)

const uint32_t car_traffic_time = 60000000; // 60 sec
const uint32_t pedestrian_time = 10000000; // 10 sec

enum light_state {
    green,  // 0
    yellow, // 1
    red     // 2
};

void changePedastrianLight(enum light_state pedastrian_light_state) {
    gpio_clear(PEDESTRIAN_LED_GREEN);
    gpio_clear(PEDESTRIAN_LED_RED);
    switch (pedastrian_light_state) {
    case green:
        gpio_set(PEDESTRIAN_LED_GREEN);
        break;
    case red:
        gpio_set(PEDESTRIAN_LED_RED);
        break;
    default:
        break;
    }
}

void changeCarLight(enum light_state car_light_state) {
    gpio_clear(CAR_LED_GREEN);
    gpio_clear(CAR_LED_YELLOW);
    gpio_clear(CAR_LED_RED);
    switch (car_light_state) {
    case green:
        gpio_set(CAR_LED_GREEN);
        break;
    case yellow:
        gpio_set(CAR_LED_YELLOW);
        break;
    case red:
        gpio_set(CAR_LED_RED);
        break;
    default:
        break;
    }
}

int main(void) {
    uint32_t current_time,
        car_traffic_start, car_delta,
        pedestrian_start, pedastrian_delta;
    car_traffic_start = xtimer_now_usec();
    // bool car_light_blink = false;
    bool pedastrian_light_blink = false;
    /**
     * 0 = green;
     * 1 = yellow;
     * 2 = red;
    */
    enum light_state car_light_state = green;
    enum light_state pedastrian_light_state = green;
    gpio_init(CAR_LED_GREEN, GPIO_OUT);
    gpio_init(CAR_LED_YELLOW, GPIO_OUT);
    gpio_init(CAR_LED_RED, GPIO_OUT);

    gpio_init(PEDESTRIAN_LED_RED, GPIO_OUT);
    gpio_init(PEDESTRIAN_LED_GREEN, GPIO_OUT);
    gpio_init(BTN_PIN, GPIO_IN);
    while (1) {
        current_time = xtimer_now_usec();
        car_delta = current_time - car_traffic_start;


        // время пешеходов
        if (car_delta >= car_traffic_time) {
            car_light_state = red;
            pedastrian_light_state = green;
            pedestrian_start = xtimer_now_usec();
            pedastrian_delta = 0U;
            while (pedastrian_delta < pedestrian_time) {
                current_time = xtimer_now_usec();
                pedastrian_delta = current_time - pedestrian_start;
                if (pedastrian_delta > pedestrian_time / 2) {
                    pedastrian_light_blink = true;
                    car_light_state = yellow;
                }
                changePedastrianLight(pedastrian_light_state);
                if (pedastrian_light_blink) xtimer_usleep(100000);
            }
            car_traffic_start = xtimer_now_usec();
        }


        if (car_delta < car_traffic_time) {
            car_light_state = green;
        }

        changeCarLight(car_light_state);
    }
    return 0;
}
