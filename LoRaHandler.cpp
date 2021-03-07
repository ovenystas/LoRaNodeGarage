/*
 * LoRaHandler.cpp
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

#include "LoRaHandler.h"
#include <LoRa.h>

int LoRaHandler::begin() {
  return LoRa.begin(LORA_FREQUENCY);
}

int LoRaHandler::loraRx() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet

    LoRaHeaderT rx_header;
    rx_header.dst = LoRa.read();
    rx_header.src = LoRa.read();
    rx_header.id = LoRa.read();
    rx_header.flags = LoRa.read();

    printHeader(rx_header);
    Serial.print(',');

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());

    int ackSentStatus = sendAckIfRequested(rx_header);
    if (ackSentStatus == 1) {
      Serial.print(", ACK sent");
    } else if (ackSentStatus == 0) {
      Serial.print(", ACK send failed");
    } else {
      // Do nothing
    }
    Serial.println();
  }
  return packetSize;
}

void LoRaHandler::handleLoRaMessage() {
}

void LoRaHandler::printHeader(LoRaHeaderT header) {
    Serial.print(header.dst);
    Serial.print(',');
    Serial.print(header.src);
    Serial.print(',');
    Serial.print(header.id);
    Serial.print(",0x");
    Serial.print(header.flags, HEX);
}

void LoRaHandler::sendHeader(LoRaHeaderT header) {
  LoRa.write(header.dst);
  LoRa.write(header.src);
  LoRa.write(header.id);
  LoRa.write(header.flags);
}

int LoRaHandler::sendAckIfRequested(LoRaHeaderT rx_header) {
  if (rx_header.flags & FLAGS_REQ_ACK) {
     LoRaHeaderT tx_header;
     tx_header.dst = rx_header.src;
     tx_header.src = MY_ADDRESS;
     tx_header.id = rx_header.id;
     tx_header.flags = FLAGS_ACK;

     LoRa.beginPacket();
     sendHeader(tx_header);
     LoRa.print('!');

     return LoRa.endPacket();
  }
  return 2;
}
