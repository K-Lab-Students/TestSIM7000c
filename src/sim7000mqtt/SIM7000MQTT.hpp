//
// Created by vadim on 5/26/24.
//

#ifndef TESTSIM7000C_SIM7000MQTT_HPP
#define TESTSIM7000C_SIM7000MQTT_HPP

#include <cstdint>
#include <string>
#include <queue>

#include "usart.h"

class SIM7000MQTT {
public:
	using URL = std::string;
	using Port = std::string;
	using CliendID = std::string;
	using Username = std::string;
	using Password = std::string;

	using Topic = std::string;
	using Message = std::string;

private:
	enum class State {
		kStart,
		kConnectSetup,
		kMQTTConfig,
		kConnect,
		kPublish,
		kGNSSUpdate,
		kIdle,
		kFatalError
	};

public:
	SIM7000MQTT(UART_HandleTypeDef *huart, URL url, Port port,
				CliendID client_id, Username username, Password password);

	void run() noexcept;
	void process() noexcept;

	void publishMessage(const Topic& topic, const Message& message) noexcept;

private:
	void start_() noexcept;
	void connectSetup_() noexcept;
	void MQTTConfig_() noexcept;
	void connect_() noexcept;
	void publish_() noexcept;
	void GNSSUpdate_() noexcept;
	void idle_() noexcept;
	void fatalError_() noexcept;

	inline void setState_(State new_state) {
		prevState_ = state_;
		state_ = new_state;
	}

private:
	UART_HandleTypeDef *huart_{};

	URL url_;
	Port port_;
	CliendID client_id_;
	Username username_;
	Password password_;

	State state_;
	State prevState_;

	std::queue

};

#endif //TESTSIM7000C_SIM7000MQTT_HPP
