#include <map>
#include <vector>
#include <set>
#include <string>
#include <boost/test/unit_test.hpp>

#include <redisxx/command.hpp>

BOOST_AUTO_TEST_CASE(command_string_api) {
	redis::Command cmd{"set", "foulish", "barrr"};
	BOOST_CHECK(*cmd == "*3\r\n$3\r\nset\r\n$7\r\nfoulish\r\n$5\r\nbarrr\r\n");
	
	cmd.clear();
	BOOST_CHECK(*cmd == "*0\r\n");
}

BOOST_AUTO_TEST_CASE(command_map_api) {
	std::map<std::string, int> data;
	data["asdf"] = 12;
	data["qwertz"] = -23;
	redis::Command cmd{"hmset", "test", data};
	BOOST_REQUIRE(*cmd == "*6\r\n$5\r\nhmset\r\n$4\r\ntest\r\n$4\r\nasdf\r\n$2\r\n12\r\n$6\r\nqwertz\r\n$3\r\n-23\r\n");
	
	cmd << "another" << "pair";
	BOOST_CHECK(*cmd == "*8\r\n$5\r\nhmset\r\n$4\r\ntest\r\n$4\r\nasdf\r\n$2\r\n12\r\n$6\r\nqwertz\r\n$3\r\n-23\r\n$7\r\nanother\r\n$4\r\npair\r\n");
}

BOOST_AUTO_TEST_CASE(command_sequence_api) {
	std::vector<float> data{3.14f, 1.414f, -0.234f};
	redis::Command cmd{"sadd", "new", data};
	BOOST_REQUIRE(*cmd == "*5\r\n$4\r\nsadd\r\n$3\r\nnew\r\n$8\r\n3.140000\r\n$8\r\n1.414000\r\n$9\r\n-0.234000\r\n");

	cmd << 12l << "helloWorld" << 0;
	BOOST_CHECK(*cmd == "*8\r\n$4\r\nsadd\r\n$3\r\nnew\r\n$8\r\n3.140000\r\n$8\r\n1.414000\r\n$9\r\n-0.234000\r\n$2\r\n12\r\n$10\r\nhelloWorld\r\n$1\r\n0\r\n");
}

BOOST_AUTO_TEST_CASE(command_set_api) {
	std::set<std::string> data{"bob", "max", "susi"};
	redis::Command cmd{"sadd", "users", data};
	BOOST_REQUIRE(*cmd == "*5\r\n$4\r\nsadd\r\n$5\r\nusers\r\n$3\r\nbob\r\n$3\r\nmax\r\n$4\r\nsusi\r\n");

	cmd << "carl";
	BOOST_CHECK(*cmd == "*6\r\n$4\r\nsadd\r\n$5\r\nusers\r\n$3\r\nbob\r\n$3\r\nmax\r\n$4\r\nsusi\r\n$4\r\ncarl\r\n");
}

BOOST_AUTO_TEST_CASE(commandlist_vector_api) {
	redis::CommandList list;
	redis::Command cmd1{"ping"};
	redis::Command cmd2{"info"};
	BOOST_CHECK(list.empty());
	
	list << cmd1;
	BOOST_CHECK(list.size() == 1u);
	BOOST_CHECK(list.capacity() >= 1u);
	BOOST_CHECK(!list.empty());
	
	BOOST_CHECK(list.at(0) == cmd1);
	list.at(0) = cmd2;
	BOOST_CHECK(list.at(0) == cmd2);
	
	list.clear();
	BOOST_CHECK(list.empty());
	list.reserve(10u);
	BOOST_CHECK(list.size() == 0u);
	BOOST_CHECK(list.capacity() >= 10u);
}

BOOST_AUTO_TEST_CASE(commandlist_type_api) {
	redis::CommandList list;
	BOOST_CHECK(list.getBatchType() == redis::BatchType::Transaction);
	list.setBatchType(redis::BatchType::Pipeline);
	BOOST_CHECK(list.getBatchType() == redis::BatchType::Pipeline);
}

BOOST_AUTO_TEST_CASE(commandlist_pipeline_api) {
	redis::Command cmd1{"set", "foulish", "barrr"}, cmd2{"set", "lolish", "roflish"};
	redis::CommandList list{redis::BatchType::Pipeline};
	list << cmd1;
	BOOST_CHECK(*list == *cmd1);
	
	list << cmd2;
	BOOST_CHECK(*list == "*6\r\n$3\r\nset\r\n$7\r\nfoulish\r\n$5\r\nbarrr\r\n$3\r\nset\r\n$6\r\nlolish\r\n$7\r\nroflish\r\n");
}

BOOST_AUTO_TEST_CASE(commandlist_transaction_api) {
	redis::Command cmd1{"set", "foulish", "barrr"}, cmd2{"set", "lolish", "roflish"};
	redis::CommandList list{redis::BatchType::Transaction};
	list << cmd1;
	BOOST_CHECK(*list == "$5\r\nMULTI\r\n$3\r\nset\r\n$7\r\nfoulish\r\n$5\r\nbarrr\r\n$4\r\nEXEC\r\n");
	
	list << cmd2;
	BOOST_CHECK(*list == "$5\r\nMULTI\r\n$3\r\nset\r\n$7\r\nfoulish\r\n$5\r\nbarrr\r\n$3\r\nset\r\n$6\r\nlolish\r\n$7\r\nroflish\r\n$4\r\nEXEC\r\n");
}

