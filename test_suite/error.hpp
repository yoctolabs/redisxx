#include <vector>
#include <string>
#include <boost/test/included/unit_test.hpp>

#include <redisxx/error.hpp>

BOOST_AUTO_TEST_CASE(error_replyerror) {
	redis::ReplyError error{
		{redis::ReplyType::Array},
		redis::ReplyType::Status
	};
	// note: assertion will fail without string construction
	BOOST_CHECK(std::string{error.what()} == "Expected array but got status");
	
	error = redis::ReplyError{
		{redis::ReplyType::Array, redis::ReplyType::Null},
		redis::ReplyType::Status
	};
	// note: assertion will fail without string construction
	BOOST_CHECK(std::string{error.what()} == "Expected array or null but got status");
}

