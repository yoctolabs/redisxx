#include <map>
#include <vector>
#include <set>
#include <string>
#include <boost/test/included/unit_test.hpp>

#include <redisxx/command.hpp>

BOOST_AUTO_TEST_CASE(command_string_api) {
	redis::Command cmd("set", "foo", "bar");
	BOOST_REQUIRE(*cmd == "set foo bar");
}

BOOST_AUTO_TEST_CASE(command_map_api) {
	std::map<std::string, int> data;
	data["asdf"] = 12;
	data["qwertz"] = -23;
	redis::Command cmd("hmset", "test", data);
	BOOST_REQUIRE(*cmd == "hmset test asdf 12 qwertz -23");
	
	cmd << "another" << "pair";
	BOOST_REQUIRE(*cmd == "hmset test asdf 12 qwertz -23 another pair");
}

BOOST_AUTO_TEST_CASE(command_sequence_api) {
	std::vector<float> data{3.14f, 1.414f, -0.234f};
	redis::Command cmd("sadd", "new", data);
	BOOST_REQUIRE(*cmd == "sadd new 3.140000 1.414000 -0.234000");

	cmd << 12l << "helloWorld" << 0;
	BOOST_REQUIRE(*cmd == "sadd new 3.140000 1.414000 -0.234000 12 helloWorld 0");
}

BOOST_AUTO_TEST_CASE(command_set_api) {
	std::set<std::string> data{"bob", "max", "susi"};
	redis::Command cmd("sadd", "users", data);
	BOOST_REQUIRE(*cmd == "sadd users bob max susi");

	cmd << "carl" << "red" << "chris";
	BOOST_REQUIRE(*cmd == "sadd users bob max susi carl red chris");
}

