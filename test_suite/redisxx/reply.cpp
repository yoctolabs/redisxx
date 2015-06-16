/** @file test_suite/redisxx/reply.cpp
 *
 */
#include <vector>
#include <string>
#include <boost/test/included/unit_test.hpp>

#include <redisxx/reply.hpp>
#include <redisxx/error.hpp>

BOOST_AUTO_TEST_SUITE(redisxx_test_reply)

BOOST_AUTO_TEST_CASE(reply_null) {
	auto reply = redis::Reply::createNull();
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::Null);
	BOOST_REQUIRE(reply == redis::Reply::createNull());
	
	BOOST_CHECK_THROW(reply.getSize(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getStatus(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getString(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getArray(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[0], redis::ReplyError);
}

BOOST_AUTO_TEST_CASE(reply_status) {
	auto reply = redis::Reply::createStatus(false);
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::Status);
	BOOST_CHECK(reply.getStatus() == false);
	BOOST_CHECK(reply == redis::Reply::createStatus(false));
	
	BOOST_CHECK_THROW(reply.getSize(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getString(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getArray(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[0], redis::ReplyError);
}

BOOST_AUTO_TEST_CASE(reply_string) {
	auto reply = redis::Reply::createString("hello world");
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::String);
	BOOST_CHECK(reply.getString() == "hello world");
	BOOST_CHECK(reply == redis::Reply::createString("hello world"));
	
	BOOST_CHECK_THROW(reply.getSize(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getStatus(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getArray(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[0], redis::ReplyError);
}

BOOST_AUTO_TEST_CASE(reply_error) {
	auto reply = redis::Reply::createError("something went wrong");
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::Error);
	BOOST_CHECK(reply.getString() == "something went wrong");
	BOOST_CHECK(reply == redis::Reply::createError("something went wrong"));
	
	BOOST_CHECK_THROW(reply.getSize(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getStatus(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getArray(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[0], redis::ReplyError);
}

BOOST_AUTO_TEST_CASE(reply_array) {
	std::vector<redis::Reply> children;
	children.push_back(redis::Reply::createStatus(true));
	children.push_back(redis::Reply::createString("test test"));
	children.push_back(redis::Reply::createError("oops"));
	
	auto reply = redis::Reply::createArray(children);
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::Array);
	auto& array = reply.getArray();
	BOOST_REQUIRE(reply.getSize() == 3u);
	BOOST_REQUIRE(array.size() == 3u);
	for (auto i = 0u; i < 3u; ++i) {
		BOOST_CHECK(reply[i] == array[i]);
	}
	
	BOOST_CHECK_THROW(reply.getString(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getStatus(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[4], std::out_of_range);
}

BOOST_AUTO_TEST_CASE(reply_integer) {
	auto reply = redis::Reply::createInteger(123u);
	BOOST_REQUIRE(reply.getType() == redis::ReplyType::Integer);
	BOOST_CHECK(reply.getInteger<int>() == 123);
	BOOST_CHECK(reply.getInteger<unsigned long>() == 123ul);
	
	BOOST_CHECK_THROW(reply.getSize(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getStatus(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getString(), redis::ReplyError);
	BOOST_CHECK_THROW(reply.getArray(), redis::ReplyError);
	BOOST_CHECK_THROW(reply[0], redis::ReplyError);
}

BOOST_AUTO_TEST_SUITE_END()
