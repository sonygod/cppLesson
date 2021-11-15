#include "BoostTest.h"

#include <deque>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <fmt/core.h>
#include <boost/function.hpp>
#include <functional>
#include <boost/cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/random.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <fstream>
#include <optional>
#include <unordered_set>
#include <boost/variant.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <nlohmann/json.hpp>
#include <chrono>
#include "nicer_syntax.hpp"
using namespace std::literals;
//https://theboostcpplibraries.com/boost.phoenix
void BoostTest::testStringToNumber()
{
	using boost::lexical_cast;
	int a = boost::lexical_cast<int>("123456");
	std::cout << a << std::endl;
	double b = boost::lexical_cast<double>("123.12");
	std::cout << b << std::endl;
	long double c = lexical_cast<long double>("123.11");
	std::cout << c << std::endl;


	using std::string;
	const double d = 123.12;
	auto s = boost::lexical_cast<string>(d);
	std::cout << s << std::endl;
}

void BoostTest::TestTryCatch()
{
	using std::cout;
	using std::endl;
	int i;
	try
	{
		i = boost::lexical_cast<int>("abc");
	}
	catch (boost::bad_lexical_cast& e)
	{
		cout << e.what() << endl;
		return;
	}
	cout << i << endl;
}

void BoostTest::testArray()
{
	using array = boost::array<std::string, 3>;
	array a;

	a[0] = "cat";
	a.at(1) = "shark";
	*a.rbegin() = "spider";

	std::sort(a.begin(), a.end());

	for (const std::string& s : a)
		std::cout << s << '\n';

	std::cout << a.size() << '\n';
	std::cout << a.max_size() << '\n';
}

//https://theboostcpplibraries.com/boost.regex
void BoostTest::TestString()
{
}

bool is_odd(int i) { return i % 2 == 1; }

void BoostTest::TestPhoenix()
{
	fmt::print("start test phonix\n");
	std::vector<int> v{1, 2, 3, 4, 5};

	std::cout << std::count_if(v.begin(), v.end(), is_odd) << '\n';

	auto lambda = [](int i) { return i % 2 == 1; };
	std::cout << std::count_if(v.begin(), v.end(), lambda) << '\n';

	using namespace boost::phoenix::placeholders;
	auto phoenix = arg1 % 2 == 1;
	std::cout << std::count_if(v.begin(), v.end(), phoenix) << '\n';
}

struct world
{
	void hello(std::ostream& os)
	{
		os << "Hello, world!\n";
	}
};

void mytestFunctionparm(void (world::* hello)(std::ostream&))
{
}

void BoostTest::testFunctionParm()
{
	mytestFunctionparm(&world::hello);

	boost::function<void(world*, std::ostream&)> f = &world::hello;
	world w;
	f(&w, std::ref(std::cout));
}

void BoostTest::testForEach()
{
	std::array<int, 4> a{{0, 1, 2, 3}};

	BOOST_FOREACH(int &i, a)
		i *= i;

	BOOST_REVERSE_FOREACH(int i, a)
	{
		std::cout << i << '\n';
	}

	for (int& i : a)
	{
		std::cout << i << '\n';
	}
}

struct base1
{
	virtual ~base1() = default;
};

struct base2
{
	virtual ~base2() = default;
};

struct derived : public base1, public base2
{
};

void downcast(base1* b1)
{
	auto d = dynamic_cast<derived*>(b1);
}

void crosscast(base1* b1)
{
	auto b2 = dynamic_cast<base2*>(b1);
}

void BoostTest::testCastClass()
{
	auto d = new derived;
	downcast(d);

	base1* b1 = new derived;
	crosscast(b1);
}


using namespace boost::system;

void fail(error_code& ec)
{
	ec = make_error_code(errc::not_supported);
}

void BoostTest::testErrorCode()
{
	error_code ec;
	fail(ec);
	std::cout << ec.value() << '\n';
}

void BoostTest::testMinMax()
{
	int i = 2;
	int j = 1;

	boost::tuple<const int&, const int&> t = boost::minmax(i, j);

	std::cout << t.get<0>() << '\n';
	std::cout << t.get<1>() << '\n';


	using array = std::array<int, 4>;
	array a{{2, 3, 0, 1}};


	std::pair<array::iterator, array::iterator> p =
		boost::minmax_element(a.begin(), a.end());

	std::cout << *p.first << '\n';
	std::cout << *p.second << '\n';
}


void BoostTest::testRandom()
{
	boost::random::mt19937 rng; // produces randomness out of thin air
	// see pseudo-random number generators
	boost::random::uniform_int_distribution<> six(1, 6);
	// distribution that maps to 1..6
	// see random number distributions
	int x = six(rng); // simulate rolling a die

	std::time_t now = std::time(nullptr);
	boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
	boost::random::uniform_int_distribution<> dist{1, 100};
	std::cout << dist(gen) << '\n';
}

using namespace boost::archive;

void save()
{
	std::ofstream file{"archive.txt"};
	text_oarchive oa{file};
	int i = 1;
	oa << i;
}

void load()
{
	std::ifstream file{"archive.txt"};
	text_iarchive ia{file};
	int i = 0;
	ia >> i;
	std::cout << i << '\n';
}

void BoostTest::testLoadAndSaveFile()
{
	save();
	load();
}

struct Quit
{
};

struct Move
{
	int32_t x;
	int32_t y;
};

struct Write
{
	std::string s;
};

struct ChangeColor
{
	int32_t r;
	int32_t g;
	int32_t b;
};

struct output : public boost::static_visitor<>
{
	// void operator()(double d) const { std::cout << d << '\n'; }
	// void operator()(char c) const { std::cout << c << '\n'; }
	// void operator()(std::string s) const { std::cout << s << '\n'; }

	void operator()(const Quit& q) const { fmt::print("quite \n"); }
	void operator()(const Move& m) const { fmt::print("move x={0} y={1}\n", m.x, m.y); }
	void operator()(const Write& w) const { fmt::print("write s={0} \n", w.s); }
	void operator()(const ChangeColor& c) const { fmt::print("color r={0} g={1} b={2} \n", c.r, c.g, c.b); }
};

void BoostTest::testRustEnum()
{
	//boost::variant<Quit, Move, Write, ChangeColor> myEnum;

	using Message = boost::variant<Quit, Move, Write, ChangeColor>;


	Message m1{Quit{}};
	Message m2{Move{1, 2}};
	Message m3{Write{"a"}};
	Message m4{ChangeColor{1, 2, 3}};

	apply_visitor(output{}, m1);
	apply_visitor(output{}, m2);
	apply_visitor(output{}, m3);
	apply_visitor(output{}, m4);
}

std::optional<int> funct1(const std::string& key)
{
	// use iterator to look for key in a map
	std::optional<int> ret;


	return ret;
}


struct my_struct
{
	int x;
	int y;

	int z;
};


void testObject()
{
	my_struct m = {1, 2};
}

void BoostTest::testJson()
{
	using namespace nlohmann;
	json j; //首先创建一个空的json对象
	j["pi"] = 3.141; //然后通过名称/值对的方式进行初始化，此时名称"pi"对应的数值就是3.141
	j["happy"] = true; //将名称"happy"赋值为true
	j["name"] = "Niels"; //将字符串"Niels"存储到"name"
	j["nothing"] = nullptr; //"nothing"对应的是空指针
	j["answer"]["everything"] = 42; //对对象中的对象进行初始化
	j["list"] = {1, 0, 2}; //使用列表初始化的方法对"list"数组初始化
	j["object"] = {{"currency", "USD"}, {"value", 42.99}}; //对对象进行初始化

	json j2 = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{
			"answer", {
				{"everything", 42}
			}
		},
		{"list", {1, 0, 2}},
		{
			"object", {
				{"currency", "USD"},
				{"value", 42.99}
			}
		}
	};

	auto j3 = json::parse(R"({"happy": true, "pi": 3.141})");


	std::vector<int> c_vector{1, 2, 3, 4};
	json j_vec(c_vector);
	// [1, 2, 3, 4]

	std::deque<double> c_deque{1.2, 2.3, 3.4, 5.6};
	json j_deque(c_deque);
	// [1.2, 2.3, 3.4, 5.6]

	std::list<bool> c_list{true, true, false, true};
	json j_list(c_list);
	// [true, true, false, true]

	std::forward_list<int64_t> c_flist{12345678909876, 23456789098765, 34567890987654, 45678909876543};
	json j_flist(c_flist);
	// [12345678909876, 23456789098765, 34567890987654, 45678909876543]

	std::array<unsigned long, 4> c_array{{1, 2, 3, 4}};
	json j_array(c_array);
	// [1, 2, 3, 4]

	std::set<std::string> c_set{"one", "two", "three", "four", "one"};
	json j_set(c_set); // only one entry for "one" is used
	// ["four", "one", "three", "two"]

	std::unordered_set<std::string> c_uset{"one", "two", "three", "four", "one"};
	json j_uset(c_uset); // only one entry for "one" is used
	// maybe ["two", "three", "four", "one"]

	std::multiset<std::string> c_mset{"one", "two", "one", "four"};
	json j_mset(c_mset); // both entries for "one" are used
	// maybe ["one", "two", "one", "four"]

	std::unordered_multiset<std::string> c_umset{"one", "two", "one", "four"};
	json j_umset(c_umset); // both entries for "one" are used
	// maybe ["one", "two", "one", "four"]

	std::map<std::string, int> c_map{{"one", 1}, {"two", 2}, {"three", 3}};
	json j_map(c_map);
	// {"one": 1, "three": 3, "two": 2 }

	std::unordered_map<const char*, double> c_umap{{"one", 1.2}, {"two", 2.3}, {"three", 3.4}};
	json j_umap(c_umap);
	// {"one": 1.2, "two": 2.3, "three": 3.4}

	std::multimap<std::string, bool> c_mmap{{"one", true}, {"two", true}, {"three", false}, {"three", true}};
	json j_mmap(c_mmap); // only one entry for key "three" is used
	// maybe {"one": true, "two": true, "three": true}

	std::unordered_multimap<std::string, bool> c_ummap{{"one", true}, {"two", true}, {"three", false}, {"three", true}};
	json j_ummap(c_ummap); // only one entry for key "three" is used
	// maybe {"one": true, "two": true, "three": true}

	std::string ss = j_ummap.dump();

	// std::ifstream i("./file_ummap.json");
	//
	// i >> j_ummap;


	std::ofstream o("./file_ummap_pretty.json");
	o << std::setw(4) << j_ummap << std::endl;
}

using json = nlohmann::json;

// namespace ns
// {
// 	// a simple struct to model a person
// 	struct person
// 	{
// 		std::string name;
// 		std::string address;
// 		int age;
// 	};
//
// 	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(person, name, address, age)
// }

namespace ns2
{
	struct cue
	{
		std::float_t start;
		std::float_t end;
		std::string text;


		std::string translate;

		std::string id;

		std::string upLoader;

	
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(cue, start, end, text, translate, id, upLoader);
}


void BoostTest::testParseJson()
{
	// ns::person p{"Ned Flanders", "744 Evergreen Terrace", 60};
	// json j = p;
	// std::cout << j << std::endl;
	// // {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}
	//
	// // conversion: json -> person
	// auto p2 = j.get<ns::person>();
	//
	// // that's it
	// assert(p == p2);
	//
	// fmt::print("p2 {0}\n", p2.address);
	//
	//
	// std::ofstream ofs("./message.data", std::ios::out | std::ios::binary);
	// const auto msgpack = nlohmann::json::to_msgpack(j);
	//
	//
	// auto j3 = nlohmann::json::from_msgpack(msgpack);
	//
	// auto p3 = j3.get<ns::person>();
	//
	// fmt::print("p3 {0}\n", p3.address);
	//
	// ofs.write(reinterpret_cast<const char*>(msgpack.data()), msgpack.size() * sizeof(uint8_t));
	// ofs.close();
}

auto getTimeSince(const time_point<high_resolution_clock>& start) {

	auto end = high_resolution_clock::now();
	duration<double> elapsed = end - start;
	return elapsed.count();

}
void BoostTest::testReadJson()
{
	using namespace std;
	std::ifstream in("./what.json");
	nlohmann::json j;
	in >> j;
	std::cout << "ok read ok\n" << std::endl;

	auto start = high_resolution_clock::now();

	using VTTResult = vector<vector<vector<ns2::cue>>>;
	auto cueResult = j.get<VTTResult>();

	fmt::print("ok spend time {0}",getTimeSince(start));

	int len = 0;

	for(auto arr1 :cueResult)
	{

		for(auto arr2 :arr1)
		{

			for(auto c : arr2)
			{


				if(len>100)
				{

					break;
				}

				
				//auto s = boost::lexical_cast<string>(d);
				//fmt::print("cue start={0} end={1} text={3} \n", boost::lexical_cast<string>(c.start));
				std::cout << c.start <<" " << c.end <<"  "<<c.text  <<"  "<<c.translate<<'\n';
				len++;
			}
		}
	}

}
