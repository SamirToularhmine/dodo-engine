#pragma once

#include <DodoEngine/Core/Types.h>

#include <unordered_map>


DODO_BEGIN_NAMESPACE

enum Key;

class InputManager
{
public:
	bool IsKeyDown(Key _key) const;

	bool IsMousePrimaryDown() const;
	bool IsMouseSecondaryDown() const;
	bool IsMiddleMouseDown() const;

	double GetMousePosX() const;
	double GetMousePosY() const;

	double GetMouseDeltaX() const;
	double GetMouseDeltaY() const;

	double GetScrollDeltaX() const;
	double GetScrollDeltaY() const;

	void SetKeyDown(Key _key);
	void SetKeyUp(Key _key);

	void SetMousePrimaryDown();
	void SetMousePrimaryUp();

	void SetMouseSecondaryDown();
	void SetMouseSecondaryUp();

	void SetMousePos(double _mousePosX, double _mousePosY);
	void SetScrollDelta(double _scrollDeltaX, double _scrollDeltaY);

	void SetMouseMiddleDown();
	void SetMouseMiddleUp();

	void ResetMouseDelta();

	static InputManager& Get()
	{
		static InputManager instance;
		return instance;
	}

private:
	std::unordered_map<Key, bool> m_KeysDown;

	bool m_MousePrimaryDown;
	bool m_MouseSecondaryDown;
	bool m_MouseMiddleDown;

	double m_MousePosX;
	double m_MousePosY;

	double m_MouseDeltaX;
	double m_MouseDeltaY;

	double m_ScrollDeltaX;
	double m_ScrollDeltaY;
};

DODO_END_NAMESPACE