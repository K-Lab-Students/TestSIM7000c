//
// Created by vadim on 6/1/24.
//

#ifndef TESTSIM7000C_ATCOMMUNICATOR_HPP
#define TESTSIM7000C_ATCOMMUNICATOR_HPP

#include <string>
#include <queue>

#include "ATParser.hpp"

#include "usart.h"

class ATCommunicator {
public:
	ATCommunicator(UART_HandleTypeDef *huart);

	void rawSend(const std::string& str) noexcept;
	void rxCallback(uint16_t size) noexcept;
	void process() noexcept;

	[[nodiscard]] ATParser::Status get() noexcept;

	[[nodiscard]] bool isAvailable() const noexcept;

private:
	UART_HandleTypeDef *huart_{};

	uint8_t rx_raw_buffer_[256]{};
	uint16_t rx_size_{};

	std::string current_resp_;
	std::queue<std::string> rx_raw_queue_;
	std::queue<ATParser::Status> out_queue_;
};

#endif //TESTSIM7000C_ATCOMMUNICATOR_HPP
