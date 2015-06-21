/** @file error.hpp
 *
 * RedisXX Exception classes
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <stdexcept>
#include <string>
#include <cstdint>

namespace redisxx {

/// Exception for issues referring to a connection
/**
 *	This exception is thrown if an operation failed, that is based on a
 *	remote connection.
 */
class ConnectionError: public std::runtime_error {
	private:
		std::string const host, filename;
		unsigned int const port;
		
	public:
		/// Create a new error that was caused by a TCP Socket
		/**
		 *	Create a new ConnectionError for an issue that occured while working
		 *	with a TCP Socket.
		 *
		 *	@param msg Detailed error message
		 *	@param host Remote host name that could not be reached
		 *	@param port Remote port number that could not be reached
		 */
		ConnectionError(std::string const & msg, std::string const & host, unsigned int port)
			: std::runtime_error{msg}
			, host{host}
			, filename{""}
			, port{port} {
		}
		
		/// Create a new error that was caused by a Streaming Socket
		/**
		 *	Create a new ConnectionError for an issue that occured while working
		 *	with a Streaming Socket (such as a Unix Domain Socket).
		 *
		 *	@param msg Detailed error message
		 *	@param filename Name of the local stream
		 */
		ConnectionError(std::string const & msg, std::string const & filename)
			: std::runtime_error{msg}
			, host{}
			, filename{filename}
			, port{0u} {
		}

		/// Query remote host name
		/**
		 *	Return the remote host name that caused the error. Note that the
		 *	host name is only provided for errors based on TCP Sockets. Else
		 *	the returned string is empty.
		 *
		 *	@return const-reference to the host name
		 */
		inline std::string const & getHost() const {
			return host;
		}
		
		/// Query local stream name
		/**
		 *	Return the locale stream name name that caused the error. Note that
		 *	the filename is only provided for errors based on Streaming Sockets.
		 *	Else the returned string is empty.
		 *
		 *	@return const-reference to the local stream's filename
		 */
		inline std::string const & getFilename() const {
			return filename;
		}

		/// Query remote port number
		/**
		 *	Return the remote port number that caused the error. Note that the
		 *	host name is only provided for errors based on TCP Sockets. Else
		 *	the returned port is 0.
		 *
		 *	@return port number
		 */
		inline unsigned int getPort() const {
			return port;
		}
};

} // ::redisxx
