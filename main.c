#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define BTN0_PIN    GPIO_PIN(0, 13);
//TODO: replace pins
#define LED_GREEN   GPIO_PIN(2,13)
#define LED_YELLOW  GPIO_PIN(3,13)
#define LED_RED     GPIO_PIN(4,13)

volatile int delay = 2000000;
volatile int will_plan_next_row_delay = delay * 2
volatile int next_row = true;
volatile int cycle_start_timestamp = xtimer_now_usec();
char t2_stack[THREAD_STACKSIZE_MAIN];
void* second_thread(void* arg)
{
  gpio_init(BTN0_PIN, GPIO_IN);
  while (true) {
    int is_pressed = gpio_read(BTN0_PIN);
    int current_timestamp = xtimer_now_usec();
    int delta = abs(cycle_start_timestamp - current_timestamp);
    printf("green led will be on in %d ms", abs(delay * 3 - delta));
    if (is_pressed) {
      if (next_row == false && delta < delay * 2) {
        next_row = true;
      }
    }
  }

  return NULL;
}


int main() {
  //thread for btn
  (void)thread_create(
    t2_stack, sizeof(t2_stack),
    THREAD_PRIORITY_MAIN - 1,
    THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
    second_thread, NULL, "nr2");

  gpio_init(LED_GREEN, GPIO_OUT);
  gpio_init(LED_YELLOW, GPIO_OUT);
  gpio_init(LED_RED, GPIO_OUT);
  while (next_row) {
    next_row = false;
    gpio_set(LED_RED);
    xtimer_usleep(delay);
    gpio_clear(LED_RED);

    gpio_set(LED_YELLOW);
    xtimer_usleep(delay);
    gpio_clear(LED_YELLOW);

    gpio_set(LED_GREEN);
    xtimer_usleep(delay);
    gpio_clear(LED_GREEN);
    cycle_start_timestamp = xtimer_now_usec();

  }
  return 0;
}
