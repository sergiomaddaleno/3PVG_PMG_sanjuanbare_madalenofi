#ifndef __IDENTIFIER_COMPONENT__
#define __IDENTIFIER_COMPONENT__

#include <string>
#include "../deps/nlohmann/json.hpp"

struct IdentifierComponent {

	int id;
	std::string name;

	nlohmann::json serialize();

	IdentifierComponent();
	~IdentifierComponent();
};

#endif // !__IDENTIFIER_COMPONENT__