#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Entity.h>

#include <entt/entt.hpp>

#include <string>
#include <unordered_map>

DODO_BEGIN_NAMESPACE

using EditorEntity = entt::entity;

class Scene {
  using EntitiesKeyType = EditorEntity;
  using EntitiesValueType = Entity;

 public:
  using EntityPair = std::pair<const EntitiesKeyType, EntitiesValueType>;
  using Entities = std::unordered_map<EntitiesKeyType, EntitiesValueType>;

  EditorEntity AddEntity(const char* _name);
  void Shutdown();

  template <typename ComponentType>
  void AddComponentToEntity(const EditorEntity& _entity, ComponentType _component) {
    m_Registry.emplace<ComponentType>(_entity, _component);
  }

  template <typename ComponentType>
  ComponentType& GetComponentFromEntity(const EditorEntity& _entity) {
    return m_Registry.get<ComponentType>(_entity);
  }

  template <typename ... ComponentTypes>
  auto ForAll() const {
    return m_Registry.view<const ComponentTypes...>();
  }

  Entities& GetEntities() { return m_Entities; };


 private:
  Entities m_Entities;
  entt::registry m_Registry;
};

DODO_END_NAMESPACE