#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Core/Key.h>

#include <unordered_map>

DODO_BEGIN_NAMESPACE

class InputManager
{
public:
  bool IsKeyDown(Key _key) const;
  bool IsKeyPressed(Key _key) const;

  bool IsMousePrimaryDown() const;
  bool IsMouseSecondaryDown() const;
  bool IsMiddleMouseDown() const;
  bool IsMiddleMousePressed() const;
  bool IsMiddleMouseReleased() const;

  double GetMousePosX() const;
  double GetMousePosY() const;

  double GetMouseDeltaX() const;
  double GetMouseDeltaY() const;

  double GetScrollDeltaX() const;
  double GetScrollDeltaY() const;

  void SetKeyDown(Key _key);
  void SetKeyPress(Key _key);
  void SetKeyUp(Key _key);

  void SetMousePrimaryDown();
  void SetMousePrimaryUp();

  void SetMouseSecondaryDown();
  void SetMouseSecondaryUp();

  void SetMousePos(double _mousePosX, double _mousePosY);
  void SetScrollDelta(double _scrollDeltaX, double _scrollDeltaY);

  void SetMouseMiddleDown();
  void SetMouseMiddlePressed();
  void SetMouseMiddleUp();
  void SetMouseMiddleReleased();

  void Reset();

  static InputManager &Get()
  {
    static InputManager instance;
    return instance;
  }

private:
  std::unordered_map<Key, bool> m_KeysDown;
  std::unordered_map<Key, bool> m_KeysPressed;

  bool m_MousePrimaryDown;
  bool m_MouseSecondaryDown;
  bool m_MouseMiddleDown;
  bool m_MouseMiddlePressed;
  bool m_MouseMiddleReleased;

  double m_MousePosX;
  double m_MousePosY;

  double m_MouseDeltaX;
  double m_MouseDeltaY;

  double m_ScrollDeltaX;
  double m_ScrollDeltaY;
};

DODO_END_NAMESPACE