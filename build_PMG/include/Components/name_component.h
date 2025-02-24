#ifndef __NAME_COMPONENT__
#define __NAME_COMPONENT__

#include <string>
#include "../deps/nlohmann/json.hpp"

struct NameComponent {

	std::string name;

	// To save data
	nlohmann::json serialize() {
		nlohmann::json json_data;
		json_data["Entity_name"] = name;
		return json_data;
	}
};

#endif //__NAME_COMPONENT__