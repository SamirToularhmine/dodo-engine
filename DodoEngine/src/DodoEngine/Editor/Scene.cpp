#include <DodoEngine/Editor/Scene.h>

#include <DodoEngine/Editor/Component.h>
#include <DodoEngine/Renderer/Entity.h>

DODO_BEGIN_NAMESPACE

EditorEntity Scene::AddEntity(const char* _name) {
  const EditorEntity entity = m_Registry.create();
  m_Entities[entity] = {.m_Name = _name};

  return entity;
}

void Scene::Shutdown() {
  m_Registry.view<MeshComponent>().each([&](MeshComponent& _meshComponent) { _meshComponent.m_Model.reset(); });
}

DODO_END_NAMESPACE