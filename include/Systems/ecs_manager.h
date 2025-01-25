
#ifndef ECS
#define ECS

#include <optional>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <any>

struct ComponentListBase {
	virtual ~ComponentListBase() {}

	virtual void remove_entity(size_t entity_id) = 0;
};

template<typename T>
struct ComponentListDerived : ComponentListBase {
	std::vector<std::optional<T>> component_list_;

	void remove_entity(size_t entity_id) override {
		if (entity_id < component_list_.size()) {
			component_list_[entity_id] = std::nullopt;
		}
	}
};

class ECSManager {
public:
	ECSManager() : last_entity(-1) {}

	size_t create_entity() {
		// TODO Incrementar todas las listas
		for (auto& [key, component_list] : component_list_map_) {
			auto* derived_list = static_cast<ComponentListDerived<std::any>*>(component_list.get());
			derived_list->component_list_.push_back(std::nullopt);
		}

		return last_entity++;
	}

	void destroy_entity(size_t entity_id) {

		for (auto& [key, component_list] : component_list_map_) {
			auto* derived_list = static_cast<ComponentListDerived<std::any>*>(component_list.get());
			if (derived_list) {
				derived_list->remove_entity(entity_id);
			}
		}
		last_entity--;
	}

	// delete_entity
	template<typename T>
	void add_component_type() {

		size_t key = typeid(T).hash_code();
		// Equivalente con raw pointers
		//std::vector<ComponentListBase*> all_component_lists;
		//ComponentListDerived<WheelComponent>* wheel_list_ptr = new ComponentListDerived<WheelComponent>;
		//all_component_lists.push_back(wheel_list_ptr);

		std::unique_ptr<ComponentListDerived<T>> value = std::make_unique<ComponentListDerived<T>>();
		// TODO: Inicializar la lista de componentes con la cantidad de entidades que ya hay
		value->component_list_.resize(last_entity, std::nullopt);

		component_list_map_.emplace(key, std::move(value));
	}

	template<typename T>
	std::optional<T>& get_component(size_t entity) {
		size_t hash = typeid(T).hash_code();

		auto it = component_list_map_.find(hash);

		ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());

		return cld->component_list_[entity];
	}


	template<typename T>
	std::optional<T>& add_component(size_t entity) {
		size_t hash = typeid(T).hash_code();

		auto it = component_list_map_.find(hash);
		if (it == component_list_map_.end()) {
			add_component_type<T>();
			it = component_list_map_.find(hash);
		}

		ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());

		if (cld->component_list_.size() <= entity) {
			cld->component_list_.resize(entity + 1);
		}

		cld->component_list_[entity] = T();

		return cld->component_list_[entity];
	}

	template<typename T>
	std::vector<std::optional<T>>& getComponentList() {
		size_t hash = typeid(T).hash_code();
		auto it = component_list_map_.find(hash);
		if (it == component_list_map_.end()) {
			throw std::runtime_error("Component type not found.");
		}
		auto* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
		return cld->component_list_;
	}

private:
	typedef std::unordered_map<size_t, std::unique_ptr<ComponentListBase>> map_type;
	map_type component_list_map_;
	size_t last_entity;

};

#endif // ECS





