//
// Created by vadim on 5/30/24.
//

#include "ATParser.hpp"

ATParser::Status ATParser::parse(const std::string& str) noexcept
{
	if (str.find("\r\n") == std::string::npos)
		return Status::kNotFullInput;

	std::string nwtr{str, str.find("\r\n"), str.size() - str.find("\r\n")};
	if (nwtr[0] != '\r' || nwtr[1] != '\n') {
		return Status::kNotValid;
	}

	if (*(nwtr.end() - 1) == '>') {
		return Status::kWaitInput;
	}

	if ((*(nwtr.end() - 2) != '\r' && *(nwtr.end() - 1) != '\n') || nwtr.size() == 2) {
		return Status::kNotFullInput;
	}

	if (nwtr[2] == '+') {
		if (nwtr[3] == 'C') {
			if (nwtr[4] == 'P') {
				return Status::kCPIN;
			} else if (nwtr[4] == 'F') {
				return Status::kCFUN;
			} else {
				return Status::kUnknown;
			}
		}
		if (nwtr[3] == 'A') {
			return Status::kAPPPDPActive;
		}
		return Status::kUnknown;
	} else if (nwtr[2] == 'S' && nwtr[3] == 'M' && nwtr[4] == 'S') {
		return Status::kSMSRdy;
	} else if (nwtr[2] == 'R' && nwtr[3] == 'D' && nwtr[4] == 'Y') {
		return Status::kRDY;
	} else if (nwtr[2] == 'O' && nwtr[3] == 'K') {
		return Status::kOk;
	} else if (nwtr[2] == 'E' && nwtr[4] == 'R' && nwtr[5] == 'R') {
		return Status::kError;
	}

	return Status::kUnknown;
}

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
//
	return Status::kNotFullInput;
//	return Status::kUnknown;
}
