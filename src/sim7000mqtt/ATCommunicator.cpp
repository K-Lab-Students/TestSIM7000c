//
// Created by vadim on 6/1/24.
//

#include "ATCommunicator.hpp"

#include <cstring>

ATCommunicator::ATCommunicator(UART_HandleTypeDef *huart) : huart_(huart)
{
	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
}

void ATCommunicator::rawSend(const std::string& str) noexcept
{
	HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.length(), 1000);

	char temp[100]{};
	sprintf(temp, "Try: {%s}\n", str.c_str());
	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);
}

void ATCommunicator::rxCallback(uint16_t size) noexcept
{
	rx_size_ = size;

	char temp[100]{};
	sprintf(temp, "Get: {%s}\n", rx_raw_buffer_);
	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);

	rx_raw_queue_.emplace(rx_raw_buffer_, rx_raw_buffer_ + size);

	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
	memset(rx_raw_buffer_, 0, rx_size_);
}

void ATCommunicator::process() noexcept
{
	if (!rx_raw_queue_.empty()) {
		current_resp_ += rx_raw_queue_.back();

		auto temp = ATParser::parse(current_resp_);
		if (temp != ATParser::Status::kNotFullInput) {
			out_queue_.push(temp);
			current_resp_.clear();
		}
	}
}

ATParser::Status ATCommunicator::get() noexcept
{
	auto res = out_queue_.back();
	out_queue_.pop();
	return res;
}

bool ATCommunicator::isAvailable() const noexcept
{
	return !out_queue_.empty();
}
