#ifndef __HIERARCHY_COMPONENT__
#define __HIERARCHY_COMPONENT__

#include <string>
#include "../deps/nlohmann/json.hpp"

struct HierarchyComponent {

public:
	void addParent(int id);
	int getParent();
	void removeParent(int id);
	void clearParents();

	void addChild(int id);
	int getChild(int id);
	void removeChild(int id);
	void clearChildren();

	bool hasParent();
	bool hasChildren();

	std::vector<int> getChildren();
	std::vector<int> getParents();

	void setParents(std::vector<int> index);
	void setChildren(std::vector<int> index);

	nlohmann::json serialize();

private:
	std::vector<int> parent_;
	std::vector<int> child_;
};

#endif // !__HIERARCHY_COMPONENT__