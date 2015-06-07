#include <map>
#include <vector>
#include <string>
#include <boost/test/included/unit_test.hpp>

#include <redisxx/command.hpp>

BOOST_AUTO_TEST_CASE(command_string_api) {
	redis::Command cmd("set", "foo", "bar");
	auto& buf = *cmd;
	BOOST_REQUIRE(buf.size() == 3u);
	BOOST_REQUIRE(buf[0] == "set");
	BOOST_REQUIRE(buf[1] == "foo");
	BOOST_REQUIRE(buf[2] == "bar");
}

BOOST_AUTO_TEST_CASE(command_map_api) {
	std::map<std::string, int> data;
	data["asdf"] = 12;
	data["qwertz"] = -23;
	redis::Command cmd("hmset", "test", data);
	auto& buf = *cmd;
	BOOST_REQUIRE(buf.size() == 6u);
	BOOST_REQUIRE(buf[0] == "hmset");
	BOOST_REQUIRE(buf[1] == "test");
	BOOST_REQUIRE(buf[2] == "asdf");
	BOOST_REQUIRE(buf[3] == "12");
	BOOST_REQUIRE(buf[4] == "qwertz");
	BOOST_REQUIRE(buf[5] == "-23");
	
	cmd << "another" << "pair";
	BOOST_REQUIRE(buf.size() == 8u);
	BOOST_REQUIRE(buf[6] == "another");
	BOOST_REQUIRE(buf[7] == "pair");
}

BOOST_AUTO_TEST_CASE(command_vector_api) {
	std::vector<float> data{3.14f, 1.414f, -0.234f};
	redis::Command cmd("sadd", "new", data);
	auto& buf = *cmd;
	BOOST_REQUIRE(buf.size() == 5u);
	BOOST_REQUIRE(buf[0] == "sadd");
	BOOST_REQUIRE(buf[1] == "new");
	// @note: std::to_string() on float isn't that good :S
	/*
	BOOST_REQUIRE(buf[2] == "3.14f");
	BOOST_REQUIRE(buf[3] == "1.414f");
	BOOST_REQUIRE(buf[4] == "-0.234f");
	*/
	
	cmd << 12l << "helloWorld" << 0;
	BOOST_REQUIRE(buf.size() == 8u);
	BOOST_REQUIRE(buf[5] == "12");
	BOOST_REQUIRE(buf[6] == "helloWorld");
	BOOST_REQUIRE(buf[7] == "0");
}

