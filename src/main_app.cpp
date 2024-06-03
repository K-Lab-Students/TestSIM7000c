//
// Created by vadim on 5/26/24.
//

#include "main_app.h"

#include "sim7000mqtt/SIM7000MQTT.hpp"
#include "sim7000mqtt/ATCommunicator.hpp"

#include <memory>

namespace {
std::shared_ptr<SIM7000MQTT> sim_7000_mqtt;
std::shared_ptr<ATCommunicator> at_communicator;

const SIM7000MQTT::URL kURL = "212.192.134.141";
const SIM7000MQTT::Port kPort = "1883";
const SIM7000MQTT::CliendID kClientID = "dfrobot";
const SIM7000MQTT::Username kUsername = "homeassistant";
const SIM7000MQTT::Password kPassword = "up4IxZQaVLvxSeYbzRkJ";
}

void main_app_init()
{
	at_communicator = std::make_shared<ATCommunicator>(&huart1);
	sim_7000_mqtt = std::make_shared<SIM7000MQTT>(at_communicator, kURL, kPort, kClientID, kUsername, kPassword);
	sim_7000_mqtt->waitInit();
}

void main_app_process()
{
	sim_7000_mqtt->setupMQTT();
	sim_7000_mqtt->enableMQTT();
	sim_7000_mqtt->publishMessage("test/test_stm", "109");
	sim_7000_mqtt->disableMQTT();
	HAL_Delay(4000);
}
