#include "Pad.h"

Pad* Pad::GetInstance()
{
	// インスタンス生成
	static Pad inst;

	// インスタンスを返す
	return &inst;
}

void Pad::Initialize()
{
	XInputGetState(0, &state_);
}

void Pad::Update()
{
	oldState_ = state_;

	XInputGetState(0, &state_);
}

Vector2 Pad::GetLStick(int16_t deadZone)
{
	Vector2 result = { (float)state_.Gamepad.sThumbLX, (float)state_.Gamepad.sThumbLY };

	if (state_.Gamepad.sThumbLX < deadZone && state_.Gamepad.sThumbLX > -deadZone) {
		result.x = 0.0f;
	}

	if (state_.Gamepad.sThumbLY < deadZone && state_.Gamepad.sThumbLY > -deadZone) {
		result.y = 0.0f;
	}

	result.x /= 32767.0f;
	result.y /= 32767.0f;

	return result;
}

Vector2 Pad::GetRStick(int16_t deadZone)
{
	Vector2 result = { (float)state_.Gamepad.sThumbRX, (float)state_.Gamepad.sThumbRY };

	if (state_.Gamepad.sThumbRX < deadZone && state_.Gamepad.sThumbRX > -deadZone) {
		result.x = 0.0f;
	}

	if (state_.Gamepad.sThumbRY < deadZone && state_.Gamepad.sThumbRY > -deadZone) {
		result.y = 0.0f;
	}

	result.x /= 32767.0f;
	result.y /= 32767.0f;

	return result;
}

Pad::Pad() {}

Pad::~Pad() {}
