#pragma once

#include <DodoEngine/Core/InputManager.h>
#include <DodoEngine/Core/Types.h>

#include <GLFW/glfw3.h>

DODO_BEGIN_NAMESPACE

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods)
{
  InputManager &inputManager = InputManager::Get();
  const Key dodoKey = static_cast<Key>(key);

  if (action == GLFW_PRESS)
  {
    inputManager.SetKeyDown(dodoKey);
    inputManager.SetKeyPress(dodoKey);
  }

  if (action == GLFW_RELEASE)
  {
    inputManager.SetKeyUp(dodoKey);
  }
}

static void cursor_position_callback(GLFWwindow *window, double xpos,
                                     double ypos)
{
  InputManager &inputManager = InputManager::Get();
  inputManager.SetMousePos(xpos, ypos);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  InputManager &inputManager = InputManager::Get();

  if (button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if (action == GLFW_PRESS)
    {
      inputManager.SetMouseSecondaryDown();
    }

    if (action == GLFW_RELEASE)
    {
      inputManager.SetMouseSecondaryUp();
    }
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (action == GLFW_PRESS)
    {
      inputManager.SetMousePrimaryDown();
    }

    if (action == GLFW_RELEASE)
    {
      inputManager.SetMousePrimaryUp();
    }
  }

  if (button == GLFW_MOUSE_BUTTON_MIDDLE)
  {
    if (action == GLFW_PRESS)
    {
      inputManager.SetMouseMiddleDown();
      inputManager.SetMouseMiddlePressed();
    }

    if (action == GLFW_RELEASE)
    {
      inputManager.SetMouseMiddleUp();
      inputManager.SetMouseMiddleReleased();
    }
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  InputManager &inputManager = InputManager::Get();
  inputManager.SetScrollDelta(xoffset, yoffset);
}

DODO_END_NAMESPACE