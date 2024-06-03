//
// Created by vadim on 5/26/24.
//

#include "main_app.h"

#include "sim7000mqtt/SIM7000MQTT.hpp"
#include "sim7000mqtt/ATCommunicator.hpp"

#include <memory>

std::shared_ptr<SIM7000MQTT> sim_7000_mqtt;
std::shared_ptr<ATCommunicator> at_communicator;

const SIM7000MQTT::URL kURL = "212.192.134.141";
const SIM7000MQTT::Port kPort = "1883";
const SIM7000MQTT::CliendID kClientID = "dfrobot";
const SIM7000MQTT::Username kUsername = "homeassistant";
const SIM7000MQTT::Password kPassword = "up4IxZQaVLvxSeYbzRkJ";

void sim7000mqttProcessWrapper(ATParser::Status status)
{
//	sim_7000_mqtt->process(status);
	sim_7000_mqtt->onReceive(status);
}

void main_app_init()
{
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_SET);
//	HAL_Delay(2000);
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(3000);

//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_SET);
//	HAL_Delay(2000);
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(3000);
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_SET);
//	HAL_Delay(2000);
//	HAL_GPIO_WritePin(GPIO_SIM7000C_PWR_GPIO_Port, GPIO_SIM7000C_PWR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(100);
//	while (true);
	at_communicator = std::make_shared<ATCommunicator>(&huart1);
	sim_7000_mqtt = std::make_shared<SIM7000MQTT>(at_communicator, kURL, kPort, kClientID, kUsername, kPassword);
//	at_communicator->subscribe({ATParser::Status::kOk, ATParser::Status::kError,
//								ATParser::Status::kCPIN, ATParser::Status::kRDY,
//								ATParser::Status::kCFUN, ATParser::Status::kSMSRdy},
//							   sim7000mqttProcessWrapper);
	sim_7000_mqtt->waitInit();
//	sim_7000_mqtt->setupMQTT();
//	sim_7000_mqtt->enableMQTT();
//	sim_7000_mqtt->publishMessage("test/test_stm", "109");
//	sim_7000_mqtt->disableMQTT();
}

uint32_t timeout = 0;
uint8_t buffer[100];
uint16_t size;

void main_app_process()
{
	sim_7000_mqtt->setupMQTT();
	sim_7000_mqtt->enableMQTT();
	sim_7000_mqtt->publishMessage("test/test_stm", "109");
	sim_7000_mqtt->disableMQTT();
	HAL_Delay(4000);
//	HAL_UARTEx_ReceiveToIdle(&huart1, buffer, sizeof(buffer), &size, 1000);
//	HAL_UART_Transmit(&huart3, buffer, size, 1000);
//	HAL_Delay(1);
//	at_communicator->process();
//	if (HAL_GetTick() - timeout >= 100) {
//		sim_7000_mqtt->process(ATParser::Status::kOk);
//		timeout = HAL_GetTick();
//	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	if (huart == &huart1) {
		at_communicator->rxCallback(size);
	}
}
