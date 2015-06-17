/** @file socket.hpp
 *
 * RedisXX socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>
#include <stdexcept>

#if defined(REDISXX_SFML_SOCKET)
	#include <redisxx/socket/sfml_tcp.hpp>
#elif defined(REDISXX_BOOST_SOCKET)
	#include <redisxx/socket/boost_tcp.hpp>
#endif

namespace redisxx {
namespace priv {

// forward declaration
template <typename SocketImpl>
void fetch_unknown(SocketImpl& socket, std::string& out);

// fetch a simple string / error / integer
template <typename SocketImpl>
inline void fetch_simple(SocketImpl& socket, std::string& out) {
	char buffer[2u];
	while (true) {
		// read next two bytes
		if (socket.read(buffer, 2u) < 2u) {
			// eof
			break;
		}
		// append them to the output buffer
		out.push_back(buffer[0u]);
		out.push_back(buffer[1u]);
		// check whether \r was found
		if (buffer[0u] == '\r') {
			if (buffer[1u] != '\n') {
				throw std::runtime_error("Protocol error: found \\r that is"
					" not followed by \\n"
				);
			}
			// reached end
			break;
		
		} else if (buffer[1u] == '\r') {
			// expecting next char to be '\n'
			char tmp;
			if (socket.read(&tmp, 1u) < 1u) {
				// eof
				break;
			}
			if (tmp != '\n') {
				throw std::runtime_error("Protocol error: found \\r that is"
					" not followed by \\n"
				);
			}
			// append final \n
			out.push_back(tmp);
			break;
		}
	}
}

// fetch a bulk string
template <typename SocketImpl>
inline void fetch_bulk(SocketImpl& socket, std::string& out) {
	// read size
	std::string number;
	fetch_simple(socket, number);
	out += number;
	number.resize(number.size() - 2u); // strip \r\n
	if (number == "-1") {
		// detected null reply
		return;
	}
	std::size_t size;
	try {
		// convert to integer
		size = std::stoul(number);
	} catch (std::invalid_argument const & e) {
		throw std::runtime_error("Protocol error: invalid bulk size '"
			+ number + "'"
		);
	} catch (std::out_of_range const & e) {
		throw std::runtime_error("Too large bulk size " + number);
	}
	// read bulk
	size += 2; // to fetch \r\n after bulk
	auto pos = out.size();
	out.resize(out.size() + size);
	if (socket.read(&out[pos], size) < size) {
		// eof
		throw std::runtime_error("Protocol error: bulk smaller than expected");
	}
}

// fetch an array
template <typename SocketImpl>
inline void fetch_array(SocketImpl& socket, std::string& out) {
	// read size
	std::string number;
	fetch_simple(socket, number);
	out += number;
	number.resize(number.size() - 2u); // strip \r\n
	std::size_t size;
	try {
		// convert to integer
		size = std::stoul(number);
	} catch (std::invalid_argument const & e) {
		throw std::runtime_error("Protocol error: invalid arra size '"
			+ number + "'"
		);
	} catch (std::out_of_range const & e) {
		throw std::runtime_error("Too large array size " + number);
	}
	// read all elements
	for (auto i = 0u; i < size; ++i) {
		fetch_unknown(socket, out);
	}
}

/// fetch the unknown :D
template <typename SocketImpl>
inline void fetch_unknown(SocketImpl& socket, std::string& out) {
	// read protocol description
	char desc;
	if (socket.read(&desc, 1u) != 1u) {
		// eof
		return;
	}
	out += desc;
	switch (desc) {
		case '+':
		case '-':
		case ':':
			// handle errors and integers as simple strings
			fetch_simple(socket, out);
			break;
		case '$':
			fetch_bulk(socket, out);
			break;
		case '*':
			fetch_array(socket, out);
			break;
		default:
			throw std::runtime_error("Protocol error: Unknown description '"
				+ std::string{desc} + "'"
			);
	}
}

template <typename SocketImpl>
inline std::string process(SocketImpl& socket, std::string const & string) {
	// write request
	socket.write(string.c_str(), string.size());
	// read reply
	std::string out;
	// tba: reserve memory for out-string based on a heuristic using the request string
	fetch_unknown(socket, out);
	return out;
}

} // ::priv
} // ::redisxx

