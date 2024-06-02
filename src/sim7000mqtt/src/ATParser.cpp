//
// Created by vadim on 5/30/24.
//

#include "ATParser.hpp"

ATParser::Status ATParser::parse(const std::string& str) noexcept
{
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
			return Status::kAPPPDP;
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
