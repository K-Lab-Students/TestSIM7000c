//
// Created by vadim on 5/26/24.
//

#ifndef TESTSIM7000C_SIM7000MQTT_HPP
#define TESTSIM7000C_SIM7000MQTT_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "usart.h"

class SIM7000MQTT {
public:
	using URL = std::string;
	using Port = std::string;
	using CliendID = std::string;
	using Username = std::string;
	using Password = std::string;

	using Topic = std::string;

	enum class Status {
		kOk,
		kError
	};

private:
	enum class State {
		kInit,
		kSetupMQTT,
		kEnableMQTT,
		kDisableMQTT,
		kGNSSUpdate,
		kIdle,
		kFatalError
	};

public:
	SIM7000MQTT(UART_HandleTypeDef *huart, URL url, Port port,
				CliendID client_id, Username username, Password password);

	void enableMQTT() noexcept;
	void setupMQTT() noexcept;
	void disableMQTT() noexcept;
	void process() noexcept;
	void setupGNSS(const Topic& topic, uint32_t timeout) noexcept;

	void publishMessage(const Topic& topic, const std::string& message) noexcept;

	void rxCallback(uint16_t size) noexcept;

private:
	void init_() noexcept;
	void setupMQTT_() noexcept;
	void enableMQTT_() noexcept;
	void disableMQTT_() noexcept;
	void GNSSUpdate_() noexcept;
	void idle_() noexcept;
	void fatalError_() noexcept;

	Status rawSend_(const std::string& str, std::string& reply, uint32_t timeout = 3000) noexcept;

	static bool checkOk_(const std::string& str) noexcept;

private:
	UART_HandleTypeDef *huart_{};

	URL url_;
	Port port_;
	CliendID client_id_;
	Username username_;
	Password password_;

	State state_;

	std::vector<std::string> setup_mqtt_cmds_;
	std::vector<std::string> enable_mqtt_cmds_;
	std::vector<std::string> disable_mqtt_cmds_;

	char rx_buffer_[256]{};
	uint16_t rx_size_{};
};

#endif //TESTSIM7000C_SIM7000MQTT_HPP
