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

void Scene::DeleteEntity(const EditorEntity &_entity) { m_Registry.destroy(_entity); }

void Scene::Shutdown()
{
  m_Registry.view<MeshComponent>().each(
      [&](MeshComponent &_meshComponent) { _meshComponent.m_Model.reset(); });
}

DODO_END_NAMESPACE