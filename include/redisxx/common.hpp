/** @file common.hpp
 *
 * RedisXX Common declarations
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>
#include <vector>

namespace redisxx {

enum class ReplyType {
	Null = 0, Error, Status, String, Integer, Array 
};

// ----------------------------------------------------------------------------
namespace reply_utils {

static std::string to_string(const ReplyType& type) const
{
	switch (type) {
		case ReplyType::Null:
			return "null";
		case ReplyType::Status:
			return "status";
		case ReplyType::Error:
			return "error";
		case ReplyType::String:
			return "string";
		case ReplyType::Integer:
			return "integer";
		case ReplyType::Array:
			return "array";
        default:
            return "undefined";
	}
}

std::string to_string(std::vector<ReplyType> const & expected)
{
	if (expected.size() == 1u) {
		return to_string(expected[0]);
	}
	std::string out;
	for (auto i = 0u; i < expected.size(); ++i) {
		out += to_string(expected[i]);
		if (i < expected.size() - 2u) {
			out += ", ";
		} else if (i == expected.size() - 2u) {
			out += " or ";
		}
	}
	return out;
}

} // ::reply_utils
} // ::redisxx

