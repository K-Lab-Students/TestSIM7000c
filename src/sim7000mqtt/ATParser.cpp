//
// Created by vadim on 5/30/24.
//

#include "ATParser.hpp"

ATParser::Status ATParser::parse(const std::string& str) noexcept
{
	if (str[0] != '\r' || str[1] != '\n') {
		return Status::kNotValid;
	}

	if (*(str.end() - 1) == '>') {
		return Status::kWaitInput;
	}

	if (*(str.end() - 2) != '\r' && *(str.end() - 1) != '\n') {
		return Status::kNotFullInput;
	}

	if (str[2] == '+') {
		if (str[3] == 'C') {
			if (str[4] == 'P') {
				return Status::kCPIN;
			} else if (str[4] == 'F') {
				return Status::kCFUN;
			} else {
				return Status::kUnknown;
			}
		}
		if (str[3] == 'A') {
			return Status::kAPPPDP;
		}
		return Status::kUnknown;
	} else if (str[2] == 'S' && str[3] == 'M' && str[4] == 'S') {
		return Status::kSMSRdy;
	} else if (str[2] == 'R' && str[3] == 'D' && str[4] == 'Y') {
		return Status::kRDY;
	} else if (str[2] == 'O' && str[3] == 'K') {
		return Status::kOk;
	} else if (str[2] == 'E' && str[4] == 'R' && str[5] == 'R') {
		return Status::kError;
	}

	return Status::kUnknown;
}
