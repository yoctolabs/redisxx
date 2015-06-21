/** @file sdlnet_tcp.hpp
 *
 * RedisXX Socket wrapper using SDL_net's TCP socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <cstdint>
#include <cassert>
#include <string>
#include <SDL/SDL_net.h>

#include <redisxx/error.hpp>

#define REDISXX_SDLNET_SOCKET 1

namespace redisxx {

/// Socket wrapper using SDL_net's TCP socket
class SdlNetTcpSocket {
	private:
		SDLNet_SocketSet set;
		TCPsocket socket;
		std::string const host;
		std::uint16_t const port;
	
		std::size_t _read(char* data, std::size_t num_bytes) {
			int ret = SDLNet_TCP_Recv(socket, data, num_bytes);
			if (ret < 0) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
			return static_cast<std::size_t>(ret);
		}
	
	public:
		SdlNetTcpSocket(std::string const & host, std::uint16_t port)
			: set{nullptr}
			, socket{nullptr}
			, host{host}
			, port{port} {
			IPaddress addr;
			if (SDLNet_ResolveHost(&addr, host.c_str(), port) < 0) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
			socket = SDLNet_TCP_Open(&addr);
			if (socket == nullptr) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
			set = SDLNet_AllocSocketSet(1);
			if (set == nullptr) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
			int ret = SDLNet_TCP_AddSocket(set, socket);
			if (ret != 1) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
		}
		
		~SdlNetTcpSocket() {
			if (socket != nullptr) {
				SDLNet_TCP_Close(socket);
			}
			if (set != nullptr) {
				SDLNet_FreeSocketSet(set);
			}
		}

		void write(char const * data, std::size_t num_bytes) {
			int ret = SDLNet_TCP_Send(socket, data, num_bytes);
			if (static_cast<std::size_t>(ret) != num_bytes) {
				throw ConnectionError{SDLNet_GetError(), host, port};
			}
		}

		void read_block(char* data, std::size_t num_bytes) {
			std::size_t left = num_bytes;
			char* ptr = data;
			// read until all necessary bytes are read
			while (true) {
				// try to read all left bytes
				auto ret = _read(ptr, left);
				if (ret == 0u) {
					// socket was closed
					throw ConnectionError{"Read "
						+ std::to_string(num_bytes-left) + " / "
						+ std::to_string(num_bytes) + " bytes but socket was"
						+ " closed", host, port
					};
				}
				if (ret == left) {
					// finished
					break;
				}
				// prepare next read
				assert(ret >= left);
				left -= ret;
				ptr += ret;
			}
		}
		
		std::size_t read_some(char* data, std::size_t num_bytes) {
			int ret = SDLNet_CheckSockets(set, 0);
			if (ret < 0) {
				throw redisxx::ConnectionError{SDLNet_GetError(), host, port};
			} else if (ret > 0 && SDLNet_SocketReady(socket)) {
				return _read(data, num_bytes);
			} else {
				return 0u;
			}
		}
};

} // ::redisxx

