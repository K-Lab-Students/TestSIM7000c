//
// Created by vadim on 6/1/24.
//

#include "ATCommunicator.hpp"

#include <cstring>

ATParser::Status ATParser::parse(const uint8_t *str, uint8_t size) noexcept
{
	uint8_t idx{};

	while (str[idx] != '\r' && idx < size)
		idx++;
	if (str[idx + 1] == '\r')
		idx++;

	idx += 2;

	if (idx >= size) {
		return Status::kNotFullInput;
	}

	if (str[idx] == '>') {
		return Status::kWaitInput;
	}
	if (str[idx] == '+') {
		if (str[idx + 1] == 'C') {
			if (str[idx + 2] == 'P') {
				return Status::kCPIN;
			} else if (str[idx + 2] == 'F') {
				return Status::kCFUN;
			} else {
				return Status::kUnknown;
			}
		}
		if (str[idx + 1] == 'A') {
			if (str[idx + 10] == 'A') {
				return Status::kAPPPDPActive;
			} else if (str[idx + 10] == 'D') {
				return Status::kAPPPDPDeactive;
			} else {
				return Status::kUnknown;
			}
		}
		return Status::kUnknown;
	} else if (str[idx] == 'S' && str[idx + 1] == 'M' && str[idx + 2] == 'S') {
		return Status::kSMSRdy;
	} else if (str[idx] == 'R' && str[idx + 1] == 'D' && str[idx + 2] == 'Y') {
		return Status::kRDY;
	} else if (str[idx] == 'O' && str[idx + 1] == 'K') {
		return Status::kOk;
	} else if (str[idx] == 'E' && str[idx + 1] == 'R' && str[idx + 2] == 'R') {
		return Status::kError;
	}

	return Status::kNotFullInput;
}

ATCommunicator::ATCommunicator(UART_HandleTypeDef *huart) : huart_(huart)
{
}

ATParser::Status ATCommunicator::rawSend(const std::string& str) noexcept
{
	HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.size(), 1000);
	return waitResponse();
}

ATParser::Status ATCommunicator::waitResponse() noexcept
{
	uint16_t size, idx{};
	static uint8_t resp[256];
	ATParser::Status res;

	do {
		HAL_UARTEx_ReceiveToIdle(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_),
								 sizeof(rx_raw_buffer_), &size, 10000);
		memcpy(&resp[idx], rx_raw_buffer_, size);
		idx += size;
	}
	while ((res = ATParser::parse(resp, idx)) == ATParser::Status::kNotFullInput);
	memset(rx_raw_buffer_, 0, sizeof(rx_raw_buffer_));

	HAL_UART_Transmit(&huart3, resp, idx, 100);
	memset(resp, 0, sizeof(resp));

	return res;
}
