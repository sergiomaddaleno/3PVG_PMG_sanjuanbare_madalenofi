#ifndef __NAME_COMPONENT__
#define __NAME_COMPONENT__

#include <string>
#include "../deps/nlohmann/json.hpp"

struct NameComponent {

	std::string name;

	nlohmann::json serialize();
};

#endif //__NAME_COMPONENT__