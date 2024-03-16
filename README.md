# Cattle Can Help (CcH)

## Project Title
Cattle-based manure management system for soil fertilization

## Research Problem Statements
Utilizing Internet of things to strategically attract cattle for defecation in low-nitrogen zones, thereby optimizing soil nutrient distribution.

## Research Novelty
- This approach presents a pragmatic solution to a common agricultural challenge. By integrating automation with the Internet of Things, we can significantly reduce the time farmers spend on manure collection. Our method involves the use of an NPK sensor to monitor nitrogen levels across different land areas. This sensor transmits data to our central server for processing. The analyzed information is then relayed back to an ESP32 controller, which determines whether to open or close a protein basket. This strategy effectively entices cattle to specific areas where their manure can be most beneficial, thereby optimizing land fertility management.
- This system proposed an IoT-based soil fertility management system integrating the natural habits of cattle and their manual activities. By combining utilization of LoRaWAN and multiple NPK sensors, this system aimed to facilitate long-distance and comprehensive fertility monitoring on the farm. Furthermore, drawing inspiration from the previously investigated ideal NPK levels based on inherent tendencies and behavioral patterns, the proposed approach involved inducing waste deposition in specific areas.

## Overview
- This project presents a way to maintain and manage soil fertility evenly through an web application using IoT systems and cattle habits. In this system, feeders and buzzers can be used to attract cattle to low fertility areas and increase fertility through cattle. Soil nutrient data is gathered using NPK sensors and transmitted via LoRaWAN, enabling users at a distance to access precise information about soil fertility from the farm. It is practical as it achieves its purpose and is expected that the physical burden on the farmer can be reduced by offloading menial labor to the cattle by using what cows do naturally.
- Our system consists of 4 main parts; End Node, Senet, AWS server, Web appliation.
- End Node is implemented by using Arduino.. AWS server is implemented by using NodeJS and Express. Web application is implemented by using ReactJS. Senet is public services handling LoRaWAN technology part.

## Proposed system architecture
-

## Wiring Diagram
-

## Code Description
ESP32 has two main functions. First, it sends a valid LoRaWAN packet with payload NPK sensor data. Second, it receives a valid LoRaWAN packet with payload signal("00" or "01") which is an user's command whether to open or lose feeder. The sound sensor activates simultaneously with the feeder. LED represents the feeder.

## Environment Settings
```
- ESP32 version: Heltec WiFi LoRa 32(V2)
- Arduino IDE version: 2.2.1
- Board Manager Version: [Heltec ESP32 Series Dev-boards 0.0.6] (https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series)

Installation
1. Download arduino IDE 2.2.1 version.
2. Connet ESP32 to computer.
3. Download 'Heltec ESP32 Series Dev-boards 0.0.6' board manager.
4. Select Tools -> Board -> Heltec ESP32 Series Dev-boards -> WIFI LoRa 32(V2).
5. Compile and upload the endnode.ino code.
```
