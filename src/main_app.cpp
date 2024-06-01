//
// Created by vadim on 5/26/24.
//

#include "main_app.h"

#include "SIM7000MQTT.hpp"
#include "ATCommunicator.hpp"

#include <memory>

std::shared_ptr<SIM7000MQTT> sim_7000_mqtt;
std::shared_ptr<ATCommunicator> at_communicator;

const SIM7000MQTT::URL kURL = "212.192.134.141";
const SIM7000MQTT::Port kPort = "1883";
const SIM7000MQTT::CliendID kClientID = "dfrobot";
const SIM7000MQTT::Username kUsername = "homeassistant";
const SIM7000MQTT::Password kPassword = "up4IxZQaVLvxSeYbzRkJ";

void main_app_init()
{
	HAL_Delay(1000);
	sim_7000_mqtt = std::make_shared<SIM7000MQTT>(at_communicator, kURL, kPort, kClientID, kUsername, kPassword);
	at_communicator = std::make_shared<ATCommunicator>(&huart1);
//	sim_7000_mqtt->publishMessage("test/test_stm", "109");
}

uint32_t timeout = 0;

void main_app_process()
{
	at_communicator->process();
	if (HAL_GetTick() - timeout >= 100) {
		sim_7000_mqtt->process();
		timeout = HAL_GetTick();
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	if (huart == &huart1) {
		at_communicator->rxCallback(size);
	}
}
