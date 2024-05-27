//
// Created by vadim on 5/26/24.
//

#include "SIM7000MQTT.hpp"

#include <utility>

#include <algorithm>

SIM7000MQTT::SIM7000MQTT(UART_HandleTypeDef *huart, URL url, Port port,
						 CliendID client_id, Username username, Password password) :
		huart_(huart),
		url_(std::move(url)),
		port_(std::move(port)),
		client_id_(std::move(client_id)),
		username_(std::move(username)),
		password_(std::move(password)),
		state_{State::kIdle} {

}

void SIM7000MQTT::run() noexcept {
	setState_(State::kStart);
}

void SIM7000MQTT::process() noexcept {
	switch (state_) {
		case State::kStart:
			start_();
			break;
		case State::kConnectSetup:
			connectSetup_();
			break;
		case State::kMQTTConfig:
			MQTTConfig_();
			break;
		case State::kConnect:
			connect_();
			break;
		case State::kPublish:
			publish_();
			break;
		case State::kGNSSUpdate:
			GNSSUpdate_();
			break;
		case State::kIdle:
			idle_();
			break;
		case State::kFatalError:
			fatalError_();
			break;
		default:
			break;
	}
}

void SIM7000MQTT::publishMessage(const SIM7000MQTT::Topic& topic, const std::string& message) noexcept {

}

void SIM7000MQTT::start_() noexcept {

}

void SIM7000MQTT::connectSetup_() noexcept {

}

void SIM7000MQTT::MQTTConfig_() noexcept {

}

void SIM7000MQTT::connect_() noexcept {

}

void SIM7000MQTT::publish_() noexcept {

}

void SIM7000MQTT::GNSSUpdate_() noexcept {

}

void SIM7000MQTT::idle_() noexcept {

}

void SIM7000MQTT::fatalError_() noexcept {

}

SIM7000MQTT::Status SIM7000MQTT::rawSend_(const std::string& str, std::string& reply) noexcept {

	auto res = HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.length(), 10);
	if (res != HAL_OK)
		return Status::kError;

	static char temp_buffer[256];
	uint16_t size;
	res = HAL_UARTEx_ReceiveToIdle(huart_, reinterpret_cast<uint8_t *>(temp_buffer), sizeof(temp_buffer), &size, 5000);
	if (res != HAL_OK)
		return Status::kError;

	reply = std::string{temp_buffer};

	return Status::kOk;
}
bool SIM7000MQTT::checkOk_(const std::string& str) noexcept {
	return str.find("OK") != std::string::npos;
}

