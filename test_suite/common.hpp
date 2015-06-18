#include <vector>
#include <string>
#include <boost/test/included/unit_test.hpp>

#include <redisxx/common.hpp>

BOOST_AUTO_TEST_CASE(common_type2string) {
	std::vector<redis::ReplyType> types;
	
	BOOST_CHECK(redis::priv::to_string(redis::ReplyType::Array) == "array");
	
	types.push_back(redis::ReplyType::String);
	types.push_back(redis::ReplyType::Error);
	BOOST_CHECK(redis::priv::to_string(types) == "string or error");
	
	types.clear();
	types.push_back(redis::ReplyType::Null);
	types.push_back(redis::ReplyType::Error);
	types.push_back(redis::ReplyType::Status);
	BOOST_CHECK(redis::priv::to_string(types) == "null, error or status");
	
	types.clear();
	types.push_back(redis::ReplyType::Integer);
	BOOST_CHECK(redis::priv::to_string(types) == "integer");
}

