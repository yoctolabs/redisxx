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

namespace redis {

enum class ReplyType {
	Null, Status, Error, String, Array
};

// ----------------------------------------------------------------------------
// API to debug-dump reply-type(s)

namespace priv {

std::string to_string(ReplyType type) {
	switch (type) {
		case ReplyType::Status:
			return "status";
		case ReplyType::Error:
			return "error";
		case ReplyType::String:
			return "string";
		case ReplyType::Array:
			return "array";
		case ReplyType::Null:
			return "null";
	}
}

std::string to_string(std::vector<ReplyType> const & expected) {
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

} // ::priv

} // ::redis

