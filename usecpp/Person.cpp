#include "Person.h"

#include <json/json.h>
#include <fstream>
#include <future>
#include <iostream>
#include <fmt/core.h>







using namespace fmt;

using namespace  std;


Person::Person(int age, std::string name)
{
	this->age = age;
	this->name = name;
}


void Person::getLotsOfGifts(int* p)
{
	for (int i = 0; i < 5; i++)
	{
		cout << "*(p + " << i << ") : ";
		cout << *(p + i) << endl;
	}


}

void Person::doArray(int* arr, int elems)
{
}

tuple<int, int> Person::testReturnTubleParms()
{

	return { 1,2 };
}

std::optional<Point> Person::walktoPoint(const Point& point)
{


	print("\n point x={0},y={1} \n", point.x, point.y);



	if(point.x>10)
	{
		return nullopt;
	}

	return point;
}

void Person::testReg(const regex& regex, std::vector<string> vec)
{



	//std::vector<int> vec = { 1, 2, 3, 4 };
	
	// for (auto element : vec)
	// 	std::cout << element << std::endl; // read only
	// for (auto &element : vec) {
	// 	element += 1;                      // writeable
	// }

	for(  auto v :vec)
	{
		if(std::regex_match(v,regex))
		{

			print("\n match v={0}", v);
		}

	}






}

void Person::readJson(string str)
{
	//ifstream in("demo.json", ios::binary);


	ifstream is(str, ios::binary);

	if (!is.is_open())
	{
		cout << "open json file failed." << endl;
		return ;
	}


	
	is.close();
	


}

void Person::testStaticFunction(string parm)
{
	print("hello ,this is static function\n");
}

void Person::sayHello(string str)
{
	print(format("from {0}", str));
}



void Person::readTxtFile()
{
	
	ifstream in("./what.json", ios::binary);

	
}


