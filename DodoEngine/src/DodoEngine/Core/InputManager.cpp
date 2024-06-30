#include <DodoEngine/Core/InputManager.h>

#include <DodoEngine/Core/Key.h>
#include <DodoEngine/Utils/Log.h>


DODO_BEGIN_NAMESPACE

bool InputManager::IsKeyDown(Key _key) const
{
	return m_KeysDown.contains(_key);
}

void InputManager::SetKeyDown(Key _key)
{
	DODO_INFO("{}", static_cast<int>(_key));
	m_KeysDown[_key] = true;
}

void InputManager::SetKeyUp(Key _key)
{
	m_KeysDown.erase(_key);
}

bool InputManager::IsMousePrimaryDown() const
{
	return m_MousePrimaryDown;
}

bool InputManager::IsMouseSecondaryDown() const
{
	return m_MouseSecondaryDown;
}

bool InputManager::IsMiddleMouseDown() const
{
	return m_MouseMiddleDown;
}

void InputManager::SetMousePrimaryDown()
{
	m_MousePrimaryDown = true;
}

void InputManager::SetMousePrimaryUp()
{
	m_MousePrimaryDown = false;
}

void InputManager::SetMouseSecondaryDown()
{
	m_MouseSecondaryDown = true;
}

void InputManager::SetMouseSecondaryUp()
{
	m_MouseSecondaryDown = false;
}

void InputManager::SetMousePos(double _mousePosX, double _mousePosY)
{
	m_MouseDeltaX = m_MousePosX - _mousePosX;
	m_MousePosX = _mousePosX;

	m_MouseDeltaY = m_MousePosY - _mousePosY;
	m_MousePosY = _mousePosY;
}

void InputManager::SetScrollDelta(double _scrollDeltaX, double _scrollDeltaY)
{
	m_ScrollDeltaX = _scrollDeltaX;
	m_ScrollDeltaY = _scrollDeltaY;
}

void InputManager::SetMouseMiddleDown()
{
	m_MouseMiddleDown = true;
}

void InputManager::SetMouseMiddleUp()
{
	m_MouseMiddleDown = false;
}

void InputManager::ResetMouseDelta()
{
	m_MouseDeltaX = 0;
	m_MouseDeltaY = 0;
	m_ScrollDeltaX = 0;
	m_ScrollDeltaY = 0;
}

double InputManager::GetMousePosX() const
{
	return m_MousePosX;
}

double InputManager::GetMousePosY() const
{
	return m_MousePosY;
}

double InputManager::GetMouseDeltaX() const
{
	return m_MouseDeltaX;
}

double InputManager::GetMouseDeltaY() const
{
	return m_MouseDeltaY;
}

double InputManager::GetScrollDeltaX() const
{
	return m_ScrollDeltaX;
}

double InputManager::GetScrollDeltaY() const
{
	return m_ScrollDeltaY;
}

DODO_END_NAMESPACE
