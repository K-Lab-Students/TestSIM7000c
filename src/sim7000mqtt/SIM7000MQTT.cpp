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

SIM7000MQTT::SIM7000MQTT(UART_HandleTypeDef *huart, URL url, Port port,
						 CliendID client_id, Username username, Password password) :
		huart_(huart),
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

	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
}

void SIM7000MQTT::enableMQTT() noexcept
{
}

void SIM7000MQTT::setupMQTT() noexcept
{
}

void SIM7000MQTT::disableMQTT() noexcept
{
}

void SIM7000MQTT::process() noexcept
{
	switch (state_) {
		case State::kWaitSIMInit:
			waitSIMInit_();
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
		case State::kPublishMessage:
			publishMessage_();
			break;
		case State::kCheckReceive:
			checkReceive_();
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
	tx_queue_.emplace(topic, message);
}

void SIM7000MQTT::rxCallback(uint16_t size) noexcept
{
	is_ready_ = true;
	rx_size_ = size;

	char temp[100]{};
	sprintf(temp, "Get: {%s}\n", rx_raw_buffer_);
	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);

	rx_queue_.emplace(rx_raw_buffer_, rx_raw_buffer_ + size);

	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
	memset(rx_raw_buffer_, 0, rx_size_);
}

void SIM7000MQTT::waitSIMInit_() noexcept
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
		setState_(State::kSetupMQTT);
		return;
	}
	setState_(State::kCheckReceive);
}

void SIM7000MQTT::setupMQTT_() noexcept
{
	if (current_cmd_idx_ == setup_mqtt_cmds_.size() - 1) {
		current_cmd_idx_ = 0;
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

	rawSend_(setup_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kCheckReceive);
}

void SIM7000MQTT::enableMQTT_() noexcept
{
	if (current_cmd_idx_ == enable_mqtt_cmds_.size() - 1 && parser_status_ == ATParser::Status::kAPPPDP) {
		current_cmd_idx_ = 0;
		is_mqtt_enabled_ = true;
		setState_(State::kIdle);
		return;
	}

	switch (parser_status_) {

		case ATParser::Status::kAPPPDP:
			break;
		case ATParser::Status::kOk:
			break;
		case ATParser::Status::kError:
			break;
		case ATParser::Status::kNotFullInput:
			break;
		case ATParser::Status::kUnknown:
			break;
		default:
			break;
	}

	if (parser_status_ == ATParser::Status::kOk) {
		error_cnt_ = 0;
		current_cmd_idx_++;
	} else if (error_cnt_ >= kMaxErrorCnt) {
		setState_(State::kFatalError);
		return;
	}

	rawSend_(enable_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kCheckReceive);
}

void SIM7000MQTT::disableMQTT_() noexcept
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
//		error_cnt_ = 0;
//
//		current_cmd_idx_++;
	}

	rawSend_(disable_mqtt_cmds_[current_cmd_idx_]);
	setState_(State::kCheckReceive);
}

void SIM7000MQTT::GNSSUpdate_() noexcept
{

}

void SIM7000MQTT::idle_() noexcept
{
	if (!tx_queue_.empty()) {
		if (!is_mqtt_enabled_) {
			setState_(State::kEnableMQTT);
			return;
		}
		auto [topic, message] = tx_queue_.back();
		tx_queue_.pop();
		publish_message_cmds_[0] = topic;
		publish_message_cmds_[1] = message;
		setState_(State::kPublishMessage);
	}
}

void SIM7000MQTT::publishMessage_() noexcept
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

	rawSend_(publish_message_cmds_[current_cmd_idx_]);
	setState_(State::kCheckReceive);
}

void SIM7000MQTT::checkReceive_() noexcept
{
	if (!rx_queue_.empty()) {
		current_response_ += rx_queue_.back();
		rx_queue_.pop();

		parser_status_ = ATParser::parse(current_response_);
		if (parser_status_ != ATParser::Status::kNotFullInput) {
			setState_(prevState_);
			current_response_.clear();
		}
	}
}

void SIM7000MQTT::fatalError_() noexcept
{

}

void SIM7000MQTT::rawSend_(const std::string& str) noexcept
{
	HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.length(), 1000);

	char temp[100]{};
	sprintf(temp, "Try: {%s}\n", str.c_str());
	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);
}
