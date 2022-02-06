#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return m_keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (m_keybuffer.size() > 0u)
	{
		Keyboard::Event e = m_keybuffer.front();
		m_keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return m_keybuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (m_charbuffer.size() > 0u)
	{
		unsigned char charcode = m_charbuffer.front();
		m_charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return m_charbuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	m_keybuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	m_charbuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	m_autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	m_autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return m_autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	m_keystates[keycode] = true;
	m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(m_keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	m_keystates[keycode] = false;
	m_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(m_keybuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	m_charbuffer.push(character);
	TrimBuffer(m_charbuffer);
}

void Keyboard::ClearState() noexcept
{
	m_keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > m_bufferSize)
	{
		buffer.pop();
	}
}