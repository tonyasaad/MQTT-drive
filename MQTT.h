/*
 * MQTT.h
 *
 *  Created on: Mar 12, 2023
 *      Author: Pola
 */

#ifndef HAL_MQTT_MQTT_H_
#define HAL_MQTT_MQTT_H_


#include "../../HELPERS/std_types.h"

void MQTT_Connect(uint8* clientID );
void MQTT_Publish(uint8* topic, uint8* msg);  //, we can add the Qo level
void MQTT_Subscribe(uint8* topic);
void test(uint16 data);
#endif /* HAL_MQTT_MQTT_H_ */
