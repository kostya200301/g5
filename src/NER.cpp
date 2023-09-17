#include <iostream>
//#include <nlohmann/json.hpp>
#include <string>

#include "NER.h"


NER::NER(const std::string& data) {
	this->data = data;
}

nlohmann::json NER::GetJSON() {
	nlohmann::json json_obj;
	try {
		json_obj = nlohmann::json::parse(this->data);

	} catch (const std::exception& e) {
		std::cerr << "Error parsing JSON: " << e.what() << std::endl;
	}
	return json_obj;
}
