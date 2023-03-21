/*
 * MQTT.c
 *
 *  Created on: Mar 12, 2023
 *      Author: Pola
 */

#include "MQTT.h"
#include "../../HELPERS/std_types.h"
#include "../../MCAL/Communication_Protocols/UART/UART_INTERFACE.h"
#include <avr/delay.h>


void test(uint16 data){
	data =5;
}
/***********************************************************************************************************
 * Description:
 * it's the function of the MQTT protocol that connect the ESP8266 wifi module with the choosen brocker
 * we send the arrays by uart to the esp8266 wifi module to connect to the wifi
 * and then we send the commands to connect our module with the MQTT brocker
 *
 * *********************************************************************************************************/
void MQTT_Connect(uint8* clientID ){
	uint8 clientID_len = strlen(clientID);
	uint8 pckge_len_arr[50];
	itoa((clientID_len+14),pckge_len_arr,10);

	UART_init(115200);

	UART_sendString("ATE1\r\n"); //we enable the echo of the wifi module (esp8266)
		_delay_ms(1000);
		_delay_ms(1000);

	UART_sendString("AT+GMR\r\n");  //view the version of the wifi module
		_delay_ms(1000);
		_delay_ms(1000);

	UART_sendString("AT+CWMODE=3\r\n"); //work as client or server
		_delay_ms(2000);
		_delay_ms(1000);

	UART_sendString("AT+CIPMUX=0\r\n"); // use single channel
			_delay_ms(1000);
			_delay_ms(1000);

	UART_sendString("AT+CWJAP=\"Tony\",\"12345678\"\r\n"); //we connect the esp to the wifi by password and IP
			_delay_ms(5000);
			_delay_ms(5000);

	UART_sendString("AT+CIPSTART=\"TCP\",\"broker.mqttdashboard.com\",1883\r\n"); //we send the mqtt brocker and the device port to connect to
			_delay_ms(2000);
			_delay_ms(2000);

	UART_sendString("AT+CIPSEND=");  //we send the length of the  packet to the wifi module
	UART_sendString(pckge_len_arr);
	UART_sendString("\r\n");		//the end of the packet
			_delay_ms(2000);
			_delay_ms(2000);

uint8 MQTT_ARR[14+clientID_len]; //MQTT_ARR[255]; we can put 255 instead of 14+clientID_len
	MQTT_ARR[0]=	0x10;
	MQTT_ARR[1]=	(16);
	MQTT_ARR[2]=	0x00;
	MQTT_ARR[3]=	0x04;
	MQTT_ARR[4]=	'M';
	MQTT_ARR[5]=	'Q';
	MQTT_ARR[6]=	'T';
	MQTT_ARR[7]=	'T';
	MQTT_ARR[8]=	0x04;
	MQTT_ARR[9]=	0x02;
	MQTT_ARR[10]=	0xFF;
	MQTT_ARR[11]=	0xFF;
	MQTT_ARR[12]=	0x00;
	MQTT_ARR[13]=	0x04;

	for(uint8 i=0;i<clientID_len;i++){
		MQTT_ARR[14+i]=	clientID[0];
	}


for(uint8 i=0; i<18;i++){
	UART_Transmit(MQTT_ARR[i]); //we send bytes of the connect packet the the brocker.
	}

}
/**************************************************************************************************************
 * DESCRIPTION:
 * it's the function that publish topic on the MQTT service
 * we need to call the connect funtion before we use it
 * we can also add Qo to the function so we can control the Qo level
 * it return nothing just publish the topic to be subscribed later from other devices
 *
 * ************************************************************************************************************/
void MQTT_Publish(uint8* topic, uint8* msg){


	uint8 Topic_len=strlen(topic);
	uint8 msg_len=strlen(msg);
	uint8 pckge_len_arr[50];
	itoa((6+Topic_len+msg_len),pckge_len_arr,10);
	uint8 i=0;
	uint8 j=0;

				//UART_sendString("AT+CIPSEND=11\r\n");
	UART_sendString("AT+CIPSEND=");  //we send the length of the  packet to the wifi module
	UART_sendString(pckge_len_arr);
	UART_sendString("\r\n");		//the end of the packet
		_delay_ms(2000);
		_delay_ms(2000);

		_delay_ms(10);
	uint8 PUBLISH_ARRAY[6+Topic_len+msg_len];	//PUBLISH_ARRAY[6+msg_len+Topic_len];
	PUBLISH_ARRAY[0]=0x30;						//it's the 	Qo level 0 so we recieve no ack.
	PUBLISH_ARRAY[1]=Topic_len+msg_len+4; 		//it's the reminding length of the packet
	PUBLISH_ARRAY[2]=0x00;						//the starting length of the topic which is 0
	PUBLISH_ARRAY[3]= Topic_len;
	_delay_ms(10);
	for(i=0;i<Topic_len;i++){
		PUBLISH_ARRAY[4+i]= topic[i];
	}
	PUBLISH_ARRAY[4+(i++)]=0x00;
	PUBLISH_ARRAY[4+(i++)]=msg_len;
	_delay_ms(10);

	for( j=0;j<msg_len;j++){
		PUBLISH_ARRAY[4+i+j]= msg[j];
		}

	for(uint8 k=0;k<(6+Topic_len+msg_len);k++){
	_delay_ms(10);
	UART_Transmit(PUBLISH_ARRAY[k]);

	}

}

/***********************************************************************************************************
 * DESCRIPTION:
 *it's the function that subscribe the data from the server of the MQTT service
 *we need to the call the connect function before we use it
 *it return data with uart interrupt
 *
 *
 * *********************************************************************************************************/
void MQTT_Subscribe(uint8* topic){
	uint8 i=0;
	uint8 topic_len=strlen(topic);
	uint8 pckge_len_arr[50];
	uint8 Remaining_LEn=(topic_len+5);
	uint8 arr_dis=topic_len+7;

	itoa((topic_len+7),pckge_len_arr,10);
	UART_sendString("AT+CIPSEND=");  //we send the length of the  packet to the wifi module
	UART_sendString(pckge_len_arr);
	UART_sendString("\r\n");		//the end of the packet
			_delay_ms(2000);
			_delay_ms(2000);

	uint8 Sub_Arr[arr_dis];
		Sub_Arr[0]=0x82;//control field
		Sub_Arr[1]=Remaining_LEn;
		Sub_Arr[2]=0x00;
		Sub_Arr[3]=0x01;
		Sub_Arr[4]=0x00;
		Sub_Arr[5]=topic_len;
		for(;i<Sub_Arr[5];i++){
			Sub_Arr[6+i]=topic[i];
		}
		Sub_Arr[6+i]=0x00;
		for(uint8 Indx=0;Indx<arr_dis;Indx++){
			UART_Transmit(Sub_Arr[Indx]);
		}

}



