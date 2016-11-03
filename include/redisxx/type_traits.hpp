/** @file type_traits.hpp
 *
 * Helper declarations
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <map>
#include <set>

namespace redisxx {
namespace priv {

// assume T not to be a set
template <typename T>
struct is_set: std::false_type {
};

// assume any version of `std::unordered_set<>` to be a set
template <typename T, class Hasher, class KeyEqual, class Allocater>
struct is_set<std::unordered_set<T, Hasher, KeyEqual, Allocater>>: std::true_type {
};

// assume any version of `std::set<>` to be a set
template <typename T, class KeyCompare, class Allocator>
struct is_set<std::set<T, KeyCompare, Allocator>>: std::true_type {
};

// ---------------------------------------------------------------------------

// assume T not to be a map
template <typename T>
struct is_map: std::false_type {
};

// assume any version of `std::unordered_map<>` to be a map
template <typename Key, typename T, typename Hasher, typename KeyEqual, typename Allocator>
struct is_map<std::unordered_map<Key, T, Hasher, KeyEqual, Allocator>>: std::true_type {
};

// assume any version of `std::map<>` to be a map
template <typename Key, typename T, typename KeyCompare, typename Allocator>
struct is_map<std::map<Key, T, KeyCompare, Allocator>>: std::true_type {
};

// ---------------------------------------------------------------------------

// assume T not to be a sequence
template <typename T>
struct is_sequence: std::false_type {
};

// assume any version of `std::vector<>` to be a sequence
template <typename T, typename Allocator>
struct is_sequence<std::vector<T, Allocator>>: std::true_type {
};

// assume any version of `std::list<>` to be a sequence
template <typename T, typename Allocator>
struct is_sequence<std::list<T, Allocator>>: std::true_type {
};

// ---------------------------------------------------------------------------

// assume each class with ctor(std::string const &) to be a streaming socket
template <typename T>
struct is_stream_socket: std::is_constructible<T, std::string const &> {
};

// assume each class with ctor(std::string const &, std::uint16_t) to be a tcp socket
template <typename T>
struct is_tcp_socket: std::is_constructible<T, std::string const &, std::uint16_t> {
};

} // ::priv
} // ::redisxx

