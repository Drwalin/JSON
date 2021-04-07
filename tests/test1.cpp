
#include "JSON.hpp"
#include <map>
#include <fstream>

int main() {
	std::cout << sizeof(JSON) << " " << sizeof(JSON::Type) << "\n";
	
	const int ar[] = {123, 456, 789};
	std::map<std::string, int> o = {{"abc", 17}, {"def", 18}, {"ghi", 23}};
	{
		JSON json(o.begin(), o.end());
		json["tablice"] = JSON(ar, 3);
		json["tablice"][7] = JSON(ar, 2);
		json["tablica"][13]["json"] = 25.453;
		
		json.WriteBeautyfull(std::cout<<"\n\n");
		
		std::cout << "\n\n";
	}
	
	{
		JSON json;
		std::ifstream file("test.json");
		file >> json;
		json.WriteBeautyfull(std::cout<<"\n\n\n");
	}
	
	return 0;
}

