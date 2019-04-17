#include <Arduino.h>
#include "car.h"

Car *car;

void setup() {
    car = new Car();
}

void loop() {
    car->drive();
}
