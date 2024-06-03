//
// Created by vadim on 6/1/24.
//

#include "ATCommunicator.hpp"

#include <cstring>

ATCommunicator::ATCommunicator(UART_HandleTypeDef *huart) : huart_(huart)
{
}

ATParser::Status ATCommunicator::rawSend(const std::string& str) noexcept
{
	HAL_UART_Transmit(huart_, reinterpret_cast<const uint8_t *>(str.c_str()), str.size(), 1000);

//	char temp[100]{};
//	sprintf(temp, "Try: %d {%s}\n", 0, str.c_str());
//	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);

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

void ATCommunicator::rxCallback(uint16_t size) noexcept
{
	static uint16_t rx{0};
	rx++;
	rx_size_ = size;

	char temp[100]{};
	sprintf(temp, "Get: {%s}\n", rx_raw_buffer_);
	HAL_UART_Transmit(&huart3, reinterpret_cast<const uint8_t *>(temp), strlen(temp), 100);

	rx_raw_queue_.emplace(rx_raw_buffer_, rx_raw_buffer_ + size);

	memset(rx_raw_buffer_, 0, rx_size_);
	if (rx < 3)
		HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
}

void ATCommunicator::start() noexcept
{
	HAL_UARTEx_ReceiveToIdle_IT(huart_, reinterpret_cast<uint8_t *>(rx_raw_buffer_), sizeof(rx_raw_buffer_));
}

void ATCommunicator::process() noexcept
{
	if (!rx_raw_queue_.empty()) {
		current_resp_ += rx_raw_queue_.back();
		rx_raw_queue_.pop();

		auto temp = ATParser::parse(current_resp_);
		if (temp != ATParser::Status::kNotFullInput) {
//			out_queue_.push(temp);
			for (const auto& [events, callback] : event_handlers_) {
				if (std::find(events.begin(), events.end(), temp) != events.end()) {
					callback(temp);
				}
			}
			current_resp_.clear();
		}
	}
}

void ATCommunicator::subscribe(const std::vector<ATParser::Status>& events,
							   const std::function<void(ATParser::Status)>& callback) noexcept
{
	event_handlers_.emplace_back(events, callback);
}

//ATParser::Status ATCommunicator::get() noexcept
//{
//	uint8_t buf[30]{};
//	switch (res) {
//		case ATParser::Status::kCPIN:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kCPIN\n\r");
//			break;
//		case ATParser::Status::kAPPPDP:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kAPPPDP\n\r");
//			break;
//		case ATParser::Status::kRDY:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kRDY\n\r");
//			break;
//		case ATParser::Status::kCFUN:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kCFUN\n\r");
//			break;
//		case ATParser::Status::kSMSRdy:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kSMSRdy\n\r");
//			break;
//		case ATParser::Status::kOk:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kOk\n\r");
//			break;
//		case ATParser::Status::kError:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kError\n\r");
//			break;
//		case ATParser::Status::kWaitInput:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kWaitInput\n\r");
//			break;
//		case ATParser::Status::kNotValid:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kNotValid\n\r");
//			break;
//		case ATParser::Status::kNotFullInput:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kNotFullInput\n\r");
//			break;
//		case ATParser::Status::kUnknown:
//			sprintf(reinterpret_cast<char *>(buf), "Status: kUnknown\n\r");
//			break;
//	}
//	HAL_UART_Transmit(&huart3, buf, strlen(reinterpret_cast<const char *>(buf)), 100);
//	out_queue_.pop();
//	return res;
//}
//
//bool ATCommunicator::isAvailable() const noexcept
//{
//	return !out_queue_.empty();
//}
