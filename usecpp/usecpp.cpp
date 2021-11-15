// usecpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <regex>

#include "Person.h"
#include <tuple>
#include <boost/array.hpp>
#include <fmt/core.h>

#include "Point.h"
#include "Async1.h"
#include "BoostTest.h"

int main()
{

	fmt::print("支持中文吗？");
    std::cout << "Hello World!\n";


	Person* person = new Person(1,"sonygod");

	person->sayHello("hello ,from visual studio resharper");

	int arr[]{ 2,6,3,1,9,8,10,5,4,0 };



	Person::testStaticFunction("static function parms");


	person->getLotsOfGifts(arr);

	const int ELEMS = 6;
	boost::array<int, ELEMS> values1 = { 3, 1, 4, 2, 9, 8 };
	

	person->doArray(arr,values1.size());


	auto result = person->testReturnTubleParms();

	

	fmt::print("value1 {0} {1} ", std::get<0>(result), std::get<1>(result));
	Point point={ 1};

	


	
	person->walktoPoint(point);


	std::vector<string> fnames = { "foo.txt", "bar.txt", "test", "a0.txt", "AAA.txt" };
	// 在 C++ 中 \ 会被作为字符串内的转义符，为使 \. 作为正则表达式传递进去生效，需要对 \ 进行二次转义，从而有 \\.
      std::regex txt_regex("[a-z]+\\.txt");
	// for (const auto &fname : fnames)
	// 	std::cout << fname << ": " << std::regex_match(fname, txt_regex) << std::endl;


	  person->testReg(txt_regex, fnames);


	  person->readJson("./what.json");

	 // Async1::main();


	  BoostTest::testStringToNumber();
	  BoostTest::TestTryCatch();
	  BoostTest::testArray();
	  BoostTest::TestString();
	  BoostTest::TestPhoenix();
	  BoostTest::testJson();
	  BoostTest::testParseJson();
	  BoostTest::testReadJson();
	 

	//  asyncio::run( person->readTxtFile());

	
	
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
