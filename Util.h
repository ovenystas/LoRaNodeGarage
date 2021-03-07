/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */


#pragma once

#include <Arduino.h>

// TODO: Handle wrap-around after 49 days.
#define seconds() (millis() / 1000)

void printMillis(Stream& stream);
void printUptime(Stream& stream);
