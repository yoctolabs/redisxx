# RedisXX

RedisXX is a Header-Only C++11-based Redis Client. Our awesome API is currently under development.

## Introduction

RedisXX is designed to be a fast Redis Client using C++11 with an extremly flexible API for easy usage. To achieve this, RedisXX makes heavy use of template metaprogramming. For instance: Our `redisxx::Command` API can be called with common types (`std::string` and various primitive types like `float` or `std::uint64_t`) and STL-based types (such as `std::vector<>`, `std::unordered_set<>` and many others) in order make your data become part of your redis commands. This is achieved by much use of *compile-time polymorphism* (including techniques based on SFINAE). So most of the RedisXX code is already pure header code that is very easy to be added to your application.

In order to allow both synchronous and asynchronous communication, we're using `std::async` inside RedisXX. At default, each request is done asynchronously and a `std::future<>` is returned. If you want your requests to be synchronous, just add `.get()` to the end of each call to explicitly block until this request is done.

To get the maximum of flexibility, RedisXX isn't based on a single socket implementation. Each communication is performed through a very thin abstraction layer. You can either use one of the socket wrappers that are shipped with RedisXX - or write your own. See below for further information about socket wrappers.

## How to get started

After you downloaded redisXX you can include our main header, add the dependencies (see below) to your build process and start coding:

```c++
#include <redisxx/redisxx.hpp>
```

If you want to use an existing socket wrapper, just include it
```c++
#include <redisxx/socket/sfml_tcp.hpp> // enable SFML's TCP-Socket
```

## Dependencies

Which dependencies are required depends on the used features. Here's a full list of dependencies and the versions we've tested. That doesn't mean other versions aren't supported - but we haven't tested them, yet. Feel free to report about your experiences with other versions of the dependencies!

Feature							| Dependencies (tested versions)
--------------------------------| ---------------------------------------------
Unit Testing					| boost_unit_test_framework (1.54)
Boost Aso Unix Domain Socket	| boost_system, boost_thread (1.54)
Boost Asio TCP Socket			| boost_system, boost_thread (1.54)
SFML TCP Socket					| sfml-system, sfml-network (2.2)
SDLnet TCP Socket				| SDL, SDL_net (1.2.8)

**Tested Compilers:**
- *g++*: 4.8.2
- *clang++*: 3.5.0

**Note:** Linking against *boost_thread* will require to build with **-pthread**

## How to compile Test-Suite

RedisXX is developed using unit testing. To build the test suite, you can use our Scons configuration:

```shell
scons test
```

**Note:** Building the test suite will require *all* dependencies to be satisfied.

## Socket Wrapper API

In order to achieve a lightweight socket abstraction, we're using an implicit socket API. The actual socket implementation, that should be used in your code, is specified as template argument. In order work correctly, your socket implementation needs to *fully* implement the following API. See **include/redisXX/socket/** for example implementations.

```c++
class MySocket {
	private:
		// depends on what you actual need
		
	public:
		MySocket(std::string const & host, std::uint16_t port) {
			// connect to the remote socket
			// or throw redisxx::ConnectionError
		}
		
		~MySocket() {
			// disconnect from the remote socket
			// do NOT throw ANYTHING!
		}
		
		void write(char const * data, std::size_t num_bytes) {
			// write data to the remote socket
			// or throw redisxx::ConnectionError
			// precondition: data is allocated with num_bytes
		}
		
		void read_block(char* data, std::size_t num_bytes) {
			// read (in blocking mode) until num_bytes are read
			// or throw redisxx::ConnectionError
			// precondition: data is allocated with num_bytes
		}
		
		std::size_t read_some(char* data, std::size_t num_bytes) {
			// read up to num_bytes and return the number of received bytes
			// or throw redixx::ConnectionError
			// precondition: data is allocated with num_bytes
			// postcondition: return_value <= num_bytes
		}
};
```

**Note:** There's another version of supported constructors using `std::string const & host` as the only parameter. This one is e.g. used by the unix domain socket implementation. But for most cases the constructor using host and port will be best. Do implement **either** one **or** the other one! 

