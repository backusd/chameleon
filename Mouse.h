#pragma once
#include "pch.h"

#include <queue>

class Mouse
{
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			LDoubleClick,
			RPress,
			RRelease,
			MPress,
			MRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type m_type;
		bool m_leftIsPressed;
		bool m_rightIsPressed;
		int m_x;
		int m_y;
	public:
		Event() noexcept
			:
			m_type(Type::Invalid),
			m_leftIsPressed(false),
			m_rightIsPressed(false),
			m_x(0),
			m_y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			m_type(type),
			m_leftIsPressed(parent.m_leftIsPressed),
			m_rightIsPressed(parent.m_rightIsPressed),
			m_x(parent.m_x),
			m_y(parent.m_y)
		{}
		bool IsValid() const noexcept
		{
			return m_type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return m_type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return{ m_x,m_y };
		}
		int GetPosX() const noexcept
		{
			return m_x;
		}
		int GetPosY() const noexcept
		{
			return m_y;
		}
		bool LeftIsPressed() const noexcept
		{
			return m_leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return m_rightIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool MiddleIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return m_buffer.empty();
	}
	void Flush() noexcept;

	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnLeftDoubleClick(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnMiddlePressed(int x, int y) noexcept;
	void OnMiddleReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
private:
	static constexpr unsigned int m_bufferSize = 16u;
	int m_x = 0;
	int m_y = 0;
	bool m_leftIsPressed = false;
	bool m_rightIsPressed = false;
	bool m_middleIsPressed = false;
	bool m_isInWindow = false;
	int m_wheelDeltaCarry = 0;
	std::queue<Event> m_buffer;
};