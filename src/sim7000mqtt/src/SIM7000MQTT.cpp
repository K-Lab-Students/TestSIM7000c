//
// Created by vadim on 5/26/24.
//

#include "SIM7000MQTT.hpp"

#include <utility>

#include <algorithm>
#include <cstring>
#include <functional>

#include "ATCommands.hpp"

static constexpr uint8_t kMaxErrorCnt{3};

SIM7000MQTT::SIM7000MQTT(std::shared_ptr<ATCommunicator> comm, URL url, Port port,
						 CliendID client_id, Username username, Password password) :
		comm_(std::move(comm)),
		url_(std::move(url)),
		port_(std::move(port)),
		client_id_(std::move(client_id)),
		username_(std::move(username)),
		password_(std::move(password)),
		state_{State::kWaitSIMInit},
		prevState_{State::kIdle}
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

void SIM7000MQTT::waitInit() noexcept
{
	do {
		parser_status_ = comm_->waitResponse();
		switch (parser_status_) {
			case ATParser::Status::kCPIN:
				wait_sim_init_flags_ |= 0b0001;
				break;
			case ATParser::Status::kRDY:
				wait_sim_init_flags_ |= 0b1000;
				break;
			case ATParser::Status::kCFUN:
				wait_sim_init_flags_ |= 0b0010;
				break;
			case ATParser::Status::kSMSRdy:
				wait_sim_init_flags_ |= 0b0100;
				break;
			default:
				break;
		}
	}
	while (wait_sim_init_flags_ < 0b0111);
//	if (wait_sim_init_flags_ >= 0b0111) {
////	if (wait_sim_init_flags_ >= 0) {
//		setState_(State::kSetupMQTT);
//		return;
//	}
//	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::setupMQTT() noexcept
{
	for (uint8_t i = 0; i < setup_mqtt_cmds_.size();) {
		HAL_Delay(1000);
		auto s = comm_->rawSend(setup_mqtt_cmds_[i]);
		if (s == ATParser::Status::kOk)
			++i;
	}
}

void SIM7000MQTT::enableMQTT() noexcept
{
	for (uint8_t i = 0; i < enable_mqtt_cmds_.size();) {
		HAL_Delay(1000);
		auto s = comm_->rawSend(enable_mqtt_cmds_[i]);
		if (s == ATParser::Status::kOk)
			++i;
	}
}

void SIM7000MQTT::disableMQTT() noexcept
{
}

void SIM7000MQTT::process(ATParser::Status status) noexcept
{
	switch (state_) {
		case State::kWaitSIMInit:
			waitSIMInit_(status);
			break;
		case State::kSetupMQTT:
			setupMQTT_(status);
			break;
		case State::kEnableMQTT:
			enableMQTT_(status);
			break;
		case State::kDisableMQTT:
			disableMQTT_(status);
			break;
		case State::kGNSSUpdate:
			GNSSUpdate_(status);
			break;
		case State::kIdle:
			idle_(status);
			break;
		case State::kPublishMessage:
			publishMessage_(status);
			break;
		case State::kWaitCommunicator:
			waitCommunicator_(status);
			break;
		case State::kFatalError:
			fatalError_(status);
			break;
		default:
			break;
	}
}

void SIM7000MQTT::onReceive(ATParser::Status status) noexcept
{
	parser_status_ = status;
	is_received_ = true;
}

void SIM7000MQTT::setupGNSS(const SIM7000MQTT::Topic& topic, uint32_t timeout) noexcept
{
}

void SIM7000MQTT::publishMessage(const SIM7000MQTT::Topic& topic, const std::string& message) noexcept
{
	tx_queue_.emplace(topic, message);
}

void SIM7000MQTT::waitSIMInit_(ATParser::Status status) noexcept
{
	switch (parser_status_) {
		case ATParser::Status::kCPIN:
			wait_sim_init_flags_ |= 0b0001;
			break;
		case ATParser::Status::kRDY:
			wait_sim_init_flags_ |= 0b1000;
			break;
		case ATParser::Status::kCFUN:
			wait_sim_init_flags_ |= 0b0010;
			break;
		case ATParser::Status::kSMSRdy:
			wait_sim_init_flags_ |= 0b0100;
			break;
		default:
			break;
	}
	if (wait_sim_init_flags_ >= 0b0111) {
//	if (wait_sim_init_flags_ >= 0) {
		setState_(State::kSetupMQTT);
		return;
	}
	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::setupMQTT_(ATParser::Status status) noexcept
{
	if (current_cmd_idx_ == setup_mqtt_cmds_.size()) {
		current_cmd_idx_ = 0;
		setState_(State::kIdle);
		return;
	}

//	if (parser_status_ == ATParser::Status::kOk) {
//		error_cnt_ = 0;
//		current_cmd_idx_++;
//	} else if (error_cnt_ >= kMaxErrorCnt) {
//		setState_(State::kFatalError);
//		return;
//	}

	comm_->rawSend(setup_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::enableMQTT_(ATParser::Status status) noexcept
{
	if (current_cmd_idx_ == enable_mqtt_cmds_.size()) {
		current_cmd_idx_ = 0;
		is_mqtt_enabled_ = true;
		setState_(State::kIdle);
		return;
	}

//	switch (parser_status_) {
//
//		case ATParser::Status::kAPPPDP:
//			break;
//		case ATParser::Status::kOk:
//			break;
//		case ATParser::Status::kError:
//			break;
//		case ATParser::Status::kNotFullInput:
//			break;
//		case ATParser::Status::kUnknown:
//			break;
//		default:
//			break;
//	}
//
//	if (parser_status_ == ATParser::Status::kOk) {
//		error_cnt_ = 0;
//		current_cmd_idx_++;
//	} else if (error_cnt_ >= kMaxErrorCnt) {
//		setState_(State::kFatalError);
//		return;
//	}

	comm_->rawSend(enable_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::disableMQTT_(ATParser::Status status) noexcept
{
	if (current_cmd_idx_ == disable_mqtt_cmds_.size() - 1) {
		current_cmd_idx_ = 0;
		is_mqtt_enabled_ = false;
		setState_(State::kIdle);
		return;
	}

	if (parser_status_ == ATParser::Status::kOk) {
		error_cnt_ = 0;
		current_cmd_idx_++;
	} else if (error_cnt_ >= kMaxErrorCnt) {
		setState_(State::kFatalError);
		return;
	}

	comm_->rawSend(disable_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::GNSSUpdate_(ATParser::Status status) noexcept
{

}

void SIM7000MQTT::idle_(ATParser::Status status) noexcept
{
	if (!tx_queue_.empty()) {
		if (!is_mqtt_enabled_) {
			setState_(State::kEnableMQTT);
			return;
		}
		auto [topic, message] = tx_queue_.back();
		tx_queue_.pop();
		publish_message_cmds_[0] = AT_SMPUB"\"" + topic + "\"," + std::to_string(message.size()) + ",1,1" + AT_ENDL;
		publish_message_cmds_[1] = message + AT_ENDL;
		setState_(State::kPublishMessage);
	}
}

void SIM7000MQTT::publishMessage_(ATParser::Status status) noexcept
{
	if (current_cmd_idx_ == publish_message_cmds_.size() - 1) {
		current_cmd_idx_ = 0;
		if (tx_queue_.empty()) {
			setState_(State::kDisableMQTT);
		} else {
			setState_(State::kIdle);
		}
		return;
	}

	if (parser_status_ == ATParser::Status::kOk) {
		error_cnt_ = 0;
		current_cmd_idx_++;
	} else if (error_cnt_ >= kMaxErrorCnt) {
		setState_(State::kFatalError);
		return;
	}

	comm_->rawSend(publish_message_cmds_[current_cmd_idx_]);
	setState_(State::kWaitCommunicator);
}

void SIM7000MQTT::waitCommunicator_(ATParser::Status status) noexcept
{
	if (is_received_) {
		if (parser_status_ == ATParser::Status::kOk) {
			error_cnt_ = 0;
			current_cmd_idx_++;
		} else if (error_cnt_ >= kMaxErrorCnt) {
			setState_(State::kFatalError);
			return;
		}
		setState_(prevState_);
		is_received_ = false;
	}
}

void SIM7000MQTT::fatalError_(ATParser::Status status) noexcept
{

}
