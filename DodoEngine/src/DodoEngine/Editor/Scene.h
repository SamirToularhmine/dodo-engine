#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Renderer/Entity.h>

#include <entt/entt.hpp>
#include <string>
#include <unordered_map>

DODO_BEGIN_NAMESPACE

using EditorEntity = entt::entity;

class Scene
{
  using EntitiesKeyType = EditorEntity;
  using EntitiesValueType = Entity;

public:
  using EntityPair = std::pair<const EntitiesKeyType, EntitiesValueType>;

  EditorEntity AddEntity(const char *_name);
  void SelectEntity(EntityComponent &_entityComponent);
  void UnselectEntity() { m_SelectedEntity.reset(); };
  Ref<EntityComponent> &GetSelectedEntity() { return m_SelectedEntity; };
  void DeleteEntity(const EditorEntity &_entity);
  void Shutdown();

  template <typename ComponentType>
  void AddComponentToEntity(const EditorEntity &_entity, ComponentType _component)
  {
    m_Registry.emplace<ComponentType>(_entity, _component);
  }

  template <typename ComponentType>
  ComponentType *TryGetComponentFromEntity(const EditorEntity &_entity)
  {
    return m_Registry.try_get<ComponentType>(_entity);
  }

  template <typename ComponentType>
  ComponentType &GetComponentFromEntity(const EditorEntity &_entity)
  {
    return m_Registry.get<ComponentType>(_entity);
  }

  template <typename ComponentType> void RemoveComponentFromEntity(const EditorEntity &_entity)
  {
    m_Registry.remove<ComponentType>(_entity);
  }

  template <typename... ComponentTypes> auto ViewAll() const
  {
    return m_Registry.view<const ComponentTypes...>();
  }

  template <typename... ComponentTypes> auto GetAll()
  {
    return m_Registry.view<ComponentTypes...>();
  }

private:
  Ref<EntityComponent> m_SelectedEntity;
  entt::registry m_Registry;
};

DODO_END_NAMESPACE