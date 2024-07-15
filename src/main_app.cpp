//
// Created by vadim on 5/26/24.
//

#include "main_app.h"

#include "sim7000cmqtt/SIM7000MQTT.hpp"

namespace {
std::shared_ptr<SIM7000MQTT> sim_7000_mqtt;

const SIM7000MQTT::URL kURL = "212.192.134.141";
const SIM7000MQTT::Port kPort = "1883";
const SIM7000MQTT::CliendID kClientID = "dfrobot";
const SIM7000MQTT::Username kUsername = "homeassistant";
const SIM7000MQTT::Password kPassword = "up4IxZQaVLvxSeYbzRkJ";
}

uint32_t counter = 0;

void main_app_init()
{
	sim_7000_mqtt = std::make_shared<SIM7000MQTT>(&huart1, kURL, kPort, kClientID, kUsername, kPassword);
	sim_7000_mqtt->waitInit();
	sim_7000_mqtt->setupMQTT();

	sim_7000_mqtt->enableMQTT();

	sim_7000_mqtt->setupGNSS();

	sim_7000_mqtt->disableMQTT();

	while(1);
}

void main_app_process()
{
	sim_7000_mqtt->enableMQTT();
	sim_7000_mqtt->publishMessage("test/test_stm", std::to_string(counter++));
	sim_7000_mqtt->disableMQTT();
	HAL_Delay(4000);
}
