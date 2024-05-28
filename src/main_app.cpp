//
// Created by vadim on 5/26/24.
//

#include "main_app.h"

#include "SIM7000MQTT.hpp"

#include <memory>

std::shared_ptr<SIM7000MQTT> sim_7000_mqtt;

const SIM7000MQTT::URL kURL = "212.192.134.141";
const SIM7000MQTT::Port kPort = "1883";
const SIM7000MQTT::CliendID kClientID = "__dfrobot__";
const SIM7000MQTT::Username kUsername = "homeassistant";
const SIM7000MQTT::Password kPassword = "up4IxZQaVLvxSeYbzRkJ";

void main_app_init() {
	HAL_Delay(1000);
	sim_7000_mqtt = std::make_shared<SIM7000MQTT>(&huart1, kURL, kPort, kClientID, kUsername, kPassword);
	sim_7000_mqtt->enableMQTT();
	sim_7000_mqtt->publishMessage("test/test_stm", "69");
	sim_7000_mqtt->disableMQTT();
}

void main_app_process() {
	sim_7000_mqtt->process();
}