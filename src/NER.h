#ifndef NER_H
#define NER_H

#include <nlohmann/json.hpp>



class NER {
private:
	std::string data;

public:
	NER(const std::string& data);

	nlohmann::json GetJSON();
};


#endif
