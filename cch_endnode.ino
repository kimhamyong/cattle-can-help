/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network. It's pre-configured for the Adafruit
 * Feather M0 LoRa.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "pitches.h"

#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN
#endif

// APPEUI of the device
static const u1_t PROGMEM APPEUI[8]= {0x01,0x00,0x01,0x00,0x00,0x0C,0x25,0x00};
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// DEVEUI of the device
static const u1_t PROGMEM DEVEUI[8]= {0x0C,0x21,0x00,0x00,0x01,0x0C,0x25,0x00};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// APPKEY of the device
static const u1_t PROGMEM APPKEY[16] = {0x12,0xE2,0x31,0xFA,0x78,0xC1,0x2B,0xC1,0x73,0xB8,0x23,0x2D,0xB7,0x96,0xB5,0x32};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// Inquiry frame to request nitrogen, phosphorus, potassium level
const byte inquiry_fr[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x03, 0x65, 0xCD};

// Answer frame
byte npkdata[11]; 

// Schedule TX every 2 hours. 
static osjob_t sendjob;
const unsigned TX_INTERVAL = 3600;
static byte receivedByte = 9;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    .dio = {26, 34, 35},
};

// Print hex values in onEvent(). 
void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

// Event handler. Different values of 'ev' notify the program about various events that occur during LoRa communication.
void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED: // Prints device information that has been connected to Senet network.
            Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        // Handle the completion of an uplink transmission(TX) and process any available downlink(RX) data.
        case EV_TXCOMPLETE: 
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            // Check if there is any downlink data available.
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) { 
                Serial.println(F("Received "));
              if (LMIC.dataLen >= 1) {
                receivedByte = LMIC.frame[LMIC.dataBeg];
                Serial.println(receivedByte);
                // If the received byte(downlink data) is equal to 1, it means the feeder activation has been requested.
                   if (receivedByte == 1) {
                    // Turn on LED.
                  digitalWrite(2, HIGH); 
                    // Turn on piezo buzzer.
                    tone(2,1000);
                }
                // If the received byte(downlink data) is 0, it means the feeder de-activation has been requested.
                else {
                    // Turn off LED.
                  digitalWrite(2, LOW);
                    // Turn off piezo buzzer.
                    noTone(2);
                }
              }
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next uplink(do_send) in 30 seconds(TX_INTERVAL).
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // Data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;

        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

// Uplink N,P,K values as [0000,0000,0000] in hexadecimal format to a gateway.
void do_send(osjob_t* j){
    // There is a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) { 
        Serial.println(F("OP_TXRXPEND, not sending"));
    } 
    // No TX/RX job running at the moment
    else { 
        // Request npk value to the npk sensor and read as answer frame 
        if(Serial2.write(inquiry_fr, sizeof(inquiry_fr))==8){
          for(byte i=0; i<11; i++){
            npkdata[i]=Serial2.read();
          }
          }
        
        // Extract N,P,K values from the answer frame(npkdata) and store them in mydata.
        byte mydata[6]={npkdata[3],npkdata[4],npkdata[5],npkdata[6],npkdata[7],npkdata[8]};
        
        // Queue mydata as the next uplink transmission packet.
        LMIC_setTxData2(1, mydata, 6, 0); 
        Serial.println(F("Packet queued"));
        
        }        
    // Next TX is scheduled after TX_COMPLETE event.
}

// Setup function runs once when you reset.
void setup() {
    // Initialize SPI communication.
    SPI.begin(5, 19, 27, 18);
    delay(5000);
    while (! Serial);
    // Initialize serial communication with a baud rate of 115200.
    Serial.begin(115200);
    // Hardwareserial assigns pin 16,17 as RX pin and TX pin.
    Serial2.begin(9600); 
    // Set pin 2(LED) as an output.
    pinMode(2, OUTPUT); 
    // Set pin 25(piezo buzzer) as an output. 
    pinMode(25, OUTPUT);
    Serial.println(F("Starting"));

    // Initialize LMIC.
    os_init();
    LMIC_reset();
    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7,14);
    LMIC_selectSubBand(1);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

// Loop function repeats itself indefinitely.
void loop() {
  os_runloop_once();
}
