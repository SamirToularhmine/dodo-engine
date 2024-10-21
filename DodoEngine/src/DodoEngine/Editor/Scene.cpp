#include <DodoEngine/Editor/Component/Component.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Renderer/Entity.h>

DODO_BEGIN_NAMESPACE

EditorEntity Scene::AddEntity(const char *_name)
{
  const EditorEntity entity = m_Registry.create();
  m_Registry.emplace<EntityComponent>(entity,
                                      EntityComponent{entity, std::make_shared<Entity>(_name)});
  m_Registry.emplace<TransformComponent>(entity,
                                         TransformComponent{{0, 0, 0}, {0, 0, 0}, {1, 1, 1}});

  return entity;
}

void Scene::DeleteEntity(const EditorEntity &_entity)
{
  m_Registry.destroy(_entity);

  if (_entity == m_SelectedEntity->m_EditorEntity)
  {
    UnselectEntity();
  }
}

void Scene::Shutdown()
{
  m_Registry.view<MeshComponent>().each(
      [&](MeshComponent &_meshComponent) { _meshComponent.m_Model.reset(); });
}

void Scene::SelectEntity(EntityComponent &_entityComponent)
{
  if (m_SelectedEntity)
  {
    m_SelectedEntity->m_Entity->m_Selected = false;
  }
  _entityComponent.m_Entity->m_Selected = true;
  m_SelectedEntity = std::make_shared<EntityComponent>(_entityComponent);
}

DODO_END_NAMESPACE