#pragma once
#include <regex>
#include <string>
#include <boost/array.hpp>
#include <tuple>

#include "Point.h"
#include <cppcoro/read_only_file.hpp>
using namespace std;
using namespace boost;


class Person
{
public:

	Person(int age, string name);
	~Person();
	void getLotsOfGifts(int* arr);
	void doArray(int* arr, int elems);
	tuple<int, int> testReturnTubleParms();
	std::optional<Point> walktoPoint(const Point& point);
	void testReg(const regex& regex, std::vector<string> fnames);
	void readJson(string str);


	static  void testStaticFunction(string parm);
	void sayHello(string str);
	void  readTxtFile();


private:
	int age;
	std::string name;
};

