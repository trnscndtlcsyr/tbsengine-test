#pragma once
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelPress,
			WheelRelease,
			WheelUp,
			WheelDown,
			Move,
			Invalid
		};
	private:
		Type type;
		bool lmbIsPressed;
		bool rmbIsPressed;
		bool wheelIsPressed;
		float x;
		float y;
	public:
		Event() noexcept
			:
			type{ Type::Invalid },
			lmbIsPressed{ false },
			rmbIsPressed{ false },
			wheelIsPressed{ false },
			x{ 0.0f }, y{ 0.0f }
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type{ type },
			lmbIsPressed{ parent.lmbIsPressed },
			rmbIsPressed{ parent.rmbIsPressed },
			wheelIsPressed{ parent.wheelIsPressed },
			x{ parent.x }, y{ parent.y }
		{}
		bool IsValid() const noexcept { return type != Type::Invalid; }
		Type GetType() const noexcept { return type; }
		std::pair<float, float> GetPos() const noexcept { return{ x,y }; }
		float GetPosX() const noexcept { return x; }
		float GetPosY() const noexcept { return y; }
		bool LeftIsPressed() const noexcept { return lmbIsPressed; }
		bool RightIsPressed() const noexcept { return rmbIsPressed; }
		bool WheelIsPressed() const noexcept { return wheelIsPressed; }
	};


	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<float, float> GetPos() const noexcept { return {x, y}; }
	float GetPosX() const noexcept { return x; }
	float GetPosY() const noexcept { return y; }
	bool RMBIsPressed() const noexcept { return rmbIsPressed; }
	bool LMBIsPressed() const noexcept { return lmbIsPressed; }
	bool WheelIsPressed() const noexcept { return wheelIsPressed; }
	bool IsEmpty() const noexcept { return buffer.empty(); };
	Mouse::Event Process() noexcept;
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnRMBPressed() noexcept;
	void OnRMBReleased() noexcept;
	void OnLMBPressed() noexcept;
	void OnLMBReleased() noexcept;
	void OnWheelPressed() noexcept;
	void OnWheelReleased() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void Trim() noexcept;
	void OnWheelDelta(int delta) noexcept;
private: 
	float x;
	float y;
	bool rmbIsPressed = false;
	bool lmbIsPressed = false;
	bool wheelIsPressed = false;
	int wheelDelta = 0;
	static constexpr unsigned int maxBufferSize = 16u;
	std::queue<Event> buffer;
};

using mouseEvents_t = Mouse::Event::Type;



