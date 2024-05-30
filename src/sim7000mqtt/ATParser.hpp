//
// Created by vadim on 5/30/24.
//

#ifndef TESTSIM7000C_ATPARSER_HPP
#define TESTSIM7000C_ATPARSER_HPP

#include <string>

class ATParser {
public:
	enum class Status {
		kCPIN,
		kAPPPDP,
		kRDY,
		kCFUN,
		kSMSRdy,
		kOk,
		kError,
		kWaitInput,
		kNotValid,
		kNotFullInput,
		kUnknown
	};

public:
	static Status parse(const std::string& str) noexcept;

};

#endif //TESTSIM7000C_ATPARSER_HPP
