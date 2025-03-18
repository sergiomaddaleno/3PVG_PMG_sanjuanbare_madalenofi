#include "Components/hierarchy_component.h"

void HierarchyComponent::addParent(int id)
{
	parent_.push_back(id);
}

int HierarchyComponent::getParent()
{
	return parent_[0];
}

void HierarchyComponent::removeParent(int id)
{
	auto it = std::find(parent_.begin(), parent_.end(), id);
	if (it != parent_.end()) {
		parent_.erase(it);
	}
}

void HierarchyComponent::clearParents()
{
	parent_.clear();
}

void HierarchyComponent::addChild(int id)
{
	child_.push_back(id);
}

int HierarchyComponent::getChild(int id)
{
	if (!parent_.empty()) {
		return parent_[id];
	}

	return 0;
}

void HierarchyComponent::removeChild(int id)
{
	auto it = std::find(child_.begin(), child_.end(), id);
	if (it != child_.end()) {
		child_.erase(it);
	}
}

void HierarchyComponent::clearChildren()
{
	child_.clear();
}

bool HierarchyComponent::hasParent()
{
	return !parent_.empty();
}

bool HierarchyComponent::hasChildren()
{
	return !child_.empty();
}

std::vector<int> HierarchyComponent::getChildren()
{
	return child_;
}

std::vector<int> HierarchyComponent::getParents()
{
	return parent_;
}

void HierarchyComponent::setParents(std::vector<int> index)
{
	parent_ = index;
}

void HierarchyComponent::setChildren(std::vector<int> index)
{
	child_ = index;
}

nlohmann::json HierarchyComponent::serialize()
{
	nlohmann::json jd;

	jd["Parent"] = parent_;
	jd["Child"] = child_;

	return jd;
}


