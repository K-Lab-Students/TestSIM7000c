//
// Created by vadim on 5/26/24.
//

#ifndef TESTSIM7000C_SIM7000MQTT_HPP
#define TESTSIM7000C_SIM7000MQTT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <memory>

#include "usart.h"
#include "ATParser.hpp"
#include "ATCommunicator.hpp"

class SIM7000MQTT {
public:
	using URL = std::string;
	using Port = std::string;
	using CliendID = std::string;
	using Username = std::string;
	using Password = std::string;

	using Topic = std::string;

	using Message = std::pair<Topic, std::string>;

	enum class Status {
		kError,
		kOk
	};

private:
	enum class State {
		kWaitSIMInit,
		kSetupMQTT,
		kEnableMQTT,
		kDisableMQTT,
		kGNSSUpdate,
		kIdle,
		kPublishMessage,
		kWaitCommunicator,
		kFatalError
	};

public:
	SIM7000MQTT(std::shared_ptr<ATCommunicator> comm, URL url, Port port,
				CliendID client_id, Username username, Password password);

	void enableMQTT() noexcept;
	void setupMQTT() noexcept;
	void disableMQTT() noexcept;
	void process(ATParser::Status status) noexcept;
	void onReceive(ATParser::Status status) noexcept;
	void setupGNSS(const Topic& topic, uint32_t timeout) noexcept;

	void publishMessage(const Topic& topic, const std::string& message) noexcept;

private:
	void waitSIMInit_(ATParser::Status status) noexcept;
	void setupMQTT_(ATParser::Status status) noexcept;
	void enableMQTT_(ATParser::Status status) noexcept;
	void disableMQTT_(ATParser::Status status) noexcept;
	void GNSSUpdate_(ATParser::Status status) noexcept;
	void idle_(ATParser::Status status) noexcept;
	void publishMessage_(ATParser::Status status) noexcept;
	void waitCommunicator_(ATParser::Status status) noexcept;
	void fatalError_(ATParser::Status status) noexcept;

	inline void setState_(State state) noexcept
	{
		prevState_ = state_;
		state_ = state;
	}

private:
	std::shared_ptr<ATCommunicator> comm_{};

	URL url_;
	Port port_;
	CliendID client_id_;
	Username username_;
	Password password_;

	std::vector<std::string> setup_mqtt_cmds_;
	std::vector<std::string> enable_mqtt_cmds_;
	std::vector<std::string> disable_mqtt_cmds_;
	std::array<std::string, 2> publish_message_cmds_;

	State state_;
	State prevState_;

	uint8_t wait_sim_init_flags_{};

	uint8_t error_cnt_{};

	std::string current_response_;

	bool is_mqtt_enabled_{false};
	bool is_received_{false};

	std::queue<Message> tx_queue_;

	uint8_t current_cmd_idx_{};
	ATParser::Status parser_status_{ATParser::Status::kOk};
};

#endif //TESTSIM7000C_SIM7000MQTT_HPP
