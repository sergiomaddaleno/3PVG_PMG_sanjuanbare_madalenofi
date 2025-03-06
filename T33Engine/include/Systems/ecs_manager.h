/**
 * @file ecs_manager.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __ECS_MANAGER__
#define __ECS_MANAGER__

#include <optional>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <any>

 /**
  * @struct ComponentListBase
  * @brief Base class for component lists in the ECS.
  */
struct ComponentListBase {
	virtual ~ComponentListBase() {}

	virtual void remove_entity(size_t entity_id) = 0;
	virtual void remove_all_entities() = 0;

	virtual size_t get_entity(size_t entity_id) = 0;
	virtual size_t get_entities() = 0;
};

/**
 * @struct ComponentListDerived
 * @brief Derived class for type-specific component lists.
 */
template<typename T>
struct ComponentListDerived : ComponentListBase {
	std::vector<std::optional<T>> component_list_;

	void remove_entity(size_t entity_id) override {
		if (entity_id < component_list_.size()) {
			component_list_[entity_id] = std::nullopt;
		}
	}

	void remove_all_entities() {
		for (size_t i = 0; i < component_list_.size(); ++i) {
			component_list_[i] = std::nullopt;
		}
	}

	size_t get_entity(size_t entity_id) override {
		if (entity_id < component_list_.size() && component_list_[entity_id].has_value()) {
			return entity_id;
		}
		return 0;
	}

	size_t get_entities() override {
		return component_list_.size();
	}
};

/**
 * @class ECSManager
 * @brief Manages entities and components in the ECS.
 */
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

	void remove_entities() {
		for (auto& [key, component_list] : component_list_map_) {
			auto* derived_list = dynamic_cast<ComponentListBase*>(component_list.get());
			if (derived_list) {
				derived_list->remove_all_entities();
			}
		}
	}

	size_t getEntity(size_t entity_id) {
		for (auto& [key, component_list] : component_list_map_) {
			auto* derived_list = static_cast<ComponentListDerived<std::any>*>(component_list.get());
			if (derived_list) {
				return derived_list->get_entity(entity_id);
			}
		}
		return 0;
	}

	size_t getEntityCount() {
		if (!component_list_map_.empty()) {
			auto& component_list = component_list_map_.begin()->second;
			auto* derived_list = static_cast<ComponentListDerived<std::any>*>(component_list.get());
			if (derived_list) {
				return derived_list->get_entities();
			}
		}
		return 0;
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
	void delete_component(size_t entity) {
		size_t hash = typeid(T).hash_code();

		auto it = component_list_map_.find(hash);
		if (it == component_list_map_.end()) {
			return;
		}

		ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());

		if (entity < cld->component_list_.size()) {
			cld->component_list_[entity] = std::nullopt;
		}
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

	template<typename T>
	bool has_component(size_t entity) {
		size_t hash = typeid(T).hash_code();

		
		auto it = component_list_map_.find(hash);
		if (it == component_list_map_.end()) {
			return false;
		}

		
		auto* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
		if (entity >= cld->component_list_.size()) {
			return false; 
		}

		
		return cld->component_list_[entity].has_value();
	}

	std::vector<size_t> getAllEntities() {
		std::vector<size_t> entities;

		// Recorremos todos los tipos de componentes en el mapa.
		for (auto& [key, component_list] : component_list_map_) {
			// Intentamos acceder a la lista de componentes de cualquier tipo.
			// Aquí asumimos que si hay una lista de componentes, todas las entidades han sido asignadas de manera similar.
			auto* derived_list = dynamic_cast<ComponentListBase*>(component_list.get());
			if (derived_list) {
				size_t entity_count = derived_list->get_entities();

				// Recorremos todas las posibles entidades (por el número de elementos en la lista de componentes)
				for (size_t entity_id = 0; entity_id < entity_count; ++entity_id) {
					// Si al menos uno de los componentes de la entidad no es nulo, lo añadimos a la lista de entidades.
					if (this->has_component<std::any>(entity_id)) {
						entities.push_back(entity_id);
					}
				}
			}
		}

		return entities;
	}


private:
	typedef std::unordered_map<size_t, std::unique_ptr<ComponentListBase>> map_type;
	map_type component_list_map_;
	size_t last_entity;

};

#endif // !__ECS_MANAGER__





