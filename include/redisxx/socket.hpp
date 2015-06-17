/** @file socket.hpp
 *
 * RedisXX socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once

#if defined(REDISXX_SFML_SOCKET)
	#include <redisxx/socket/sfml_tcp.hpp>
#elif defined(REDISXX_BOOST_SOCKET)
	#include <redisxx/socket/boost_tcp.hpp>
#endif

namespace redisxx {

#if defined(REDISXX_SFML_SOCKET)
using DefaultSocket = SfmlTcpSocket;
#elif defined(REDISXX_BOOST_SOCKET)
using DefaultSocket = BoostTcpSocket;
#endif

} // ::redisxx

