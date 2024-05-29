//
// Created by vadim on 5/26/24.
//

#include "SIM7000MQTT.hpp"

#include <utility>

#include <algorithm>
#include <cstring>
#include <functional>

#include "ATCommands.hpp"

SIM7000MQTT::SIM7000MQTT(UART_HandleTypeDef *huart, URL url, Port port,
						 CliendID client_id, Username username, Password password) :
		huart_(huart),
		url_(std::move(url)),
		port_(std::move(port)),
		client_id_(std::move(client_id)),
		username_(std::move(username)),
		password_(std::move(password)),
		state_{State::kIdle}
{
	setup_mqtt_cmds_ = std::vector<std::string>{
			AT AT_ENDL,
			AT_SMCONF_URL + url_ + "," + port_ + AT_ENDL,
			AT_SMCONF_CLIENTID + client_id_ + AT_ENDL,
			AT_SMCONF_USERNAME + username_ + AT_ENDL,
			AT_SMCONF_PASSWORD + password_ + AT_ENDL,
			AT_SMCONF_KEEPTIME_60 AT_ENDL
	};

	enable_mqtt_cmds_ = std::vector<std::string>{
			AT AT_ENDL,
			AT_CNACT_ON AT_ENDL,
			AT_SMCONN AT_ENDL
	};

	disable_mqtt_cmds_ = std::vector<std::string>{
			AT AT_ENDL,
			AT_SMDISC AT_ENDL,
			AT_CNACT_OFF AT_ENDL
	};
}

void SIM7000MQTT::enableMQTT() noexcept
{
	std::string reply{};
	Status res;

	res = rawSend_("AT" AT_ENDL, reply, 1000);
	res = rawSend_(AT_CNACT_ON AT_ENDL, reply, 5000);

	res = rawSend_(AT_SMCONN AT_ENDL, reply, 5000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}
}

void SIM7000MQTT::setupMQTT() noexcept
{
	std::string reply{};
	Status res;

	res = rawSend_("AT" AT_ENDL, reply, 1000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

	res = rawSend_(AT_SMCONF_URL + url_ + "," + port_ + AT_ENDL, reply, 1000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

	res = rawSend_(AT_SMCONF_CLIENTID + client_id_ + AT_ENDL, reply, 1000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

	res = rawSend_(AT_SMCONF_USERNAME + username_ + AT_ENDL, reply, 1000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

	res = rawSend_(AT_SMCONF_PASSWORD + password_ + AT_ENDL, reply, 1000);
	res = rawSend_("AT+SMCONF=\"KEEPTIME\",60" AT_ENDL, reply, 1000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}
}

void SIM7000MQTT::disableMQTT() noexcept
{
	std::string reply{};
	Status res;

	res = rawSend_(AT_SMDISC AT_ENDL, reply, 3000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

	res = rawSend_(AT_CNACT_OFF AT_ENDL, reply, 3000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}
}

void SIM7000MQTT::process() noexcept
{
	switch (state_) {
		case State::kInit:
			init_();
			break;
		case State::kSetupMQTT:
			setupMQTT_();
			break;
		case State::kEnableMQTT:
			enableMQTT_();
			break;
		case State::kDisableMQTT:
			disableMQTT_();
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

void SIM7000MQTT::setupGNSS(const SIM7000MQTT::Topic& topic, uint32_t timeout) noexcept
{

}

void SIM7000MQTT::publishMessage(const SIM7000MQTT::Topic& topic, const std::string& message) noexcept
{
	std::string cmd =
			AT_SMPUB + topic + "," + std::to_string(message.size()) + ",1,1"
					+ AT_ENDL;// + "\r" + message + AT_ENDL;

	std::string reply{};
	auto res = rawSend_(cmd, reply, 3000);
	res = rawSend_(message + AT_ENDL, reply, 3000);
//	if (res != Status::kOk || !checkOk_(reply)) {
//		return;
//	}

}

void SIM7000MQTT::rxCallback(uint16_t size) noexcept
{
	rx_size_ = 1;
}

void SIM7000MQTT::init_() noexcept
{

}

void SIM7000MQTT::setupMQTT_() noexcept
{

}

void SIM7000MQTT::enableMQTT_() noexcept
{

}

void SIM7000MQTT::disableMQTT_() noexcept
{

}

void SIM7000MQTT::GNSSUpdate_() noexcept
{

}

void SIM7000MQTT::idle_() noexcept
{

}

void SIM7000MQTT::fatalError_() noexcept
{

}

SIM7000MQTT::Status SIM7000MQTT::rawSend_(const std::string& str, std::string& reply, uint32_t timeout) noexcept
{

	auto res = HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.length(), 10000);
	if (res != HAL_OK)
		return Status::kError;

	char temp_buffer[256]{};

	rx_size_ = 0;
	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(temp_buffer), sizeof(temp_buffer));

	HAL_UART_Transmit(&huart3, (uint8_t *)temp_buffer, strlen(temp_buffer), 1000);

	reply = std::string{temp_buffer};

	return Status::kOk;
}

bool SIM7000MQTT::checkOk_(const std::string& str) noexcept
{
	return str.find("OK") != std::string::npos;
}


