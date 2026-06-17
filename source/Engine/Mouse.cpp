#include "Mouse.hpp"
#include "Window.hpp"

Mouse::Event Mouse::Process() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::Trim() noexcept
{
	if (buffer.size() > maxBufferSize)
	{
		buffer.pop();
	}
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int X, int Y) noexcept
{
	x = X;
	y = Y;
	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	Trim();
}

void Mouse::OnRMBPressed() noexcept
{
	rmbIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	Trim();
}

void Mouse::OnRMBReleased() noexcept
{
	rmbIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	Trim();
}

void Mouse::OnLMBPressed() noexcept
{
	lmbIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	Trim();
}

void Mouse::OnLMBReleased() noexcept
{
	lmbIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	Trim();
}

void Mouse::OnWheelPressed() noexcept
{
	wheelIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelPress, *this));
	Trim();
}

void Mouse::OnWheelReleased() noexcept
{
	wheelIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelRelease, *this));
	Trim();
}

void Mouse::OnWheelUp() noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	Trim();
}

void Mouse::OnWheelDown() noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	Trim();
}


void Mouse::OnWheelDelta(int delta) noexcept
{
	wheelDelta += delta;
	if (wheelDelta >= WHEEL_DELTA)
	{
		wheelDelta -= WHEEL_DELTA;
		OnWheelUp();
	}
	else if (wheelDelta <= -WHEEL_DELTA)
	{
		wheelDelta += WHEEL_DELTA;
		OnWheelDown();
	}
}