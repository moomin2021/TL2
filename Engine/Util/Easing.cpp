#include "Easing.h"

#include <math.h>

#pragma region Back
float Easing::Back::easeIn(float start, float end, float t)
{
	float c1 = 1.70158f;
	float c3 = c1 + 1.0f;
	float time = (c3 * t * t * t - c1 * t * t);
	return start * (1.0f - time) + end * time;
}

float Easing::Back::easeOut(float start, float end, float t)
{
	float c1 = 1.70158f;
	float c3 = c1 + 1.0f;
	float time = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
	return start * (1.0f - time) + end * time;
}

float Easing::Back::easeInOut(float start, float end, float t)
{
	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;
	float time = 0.0f;
	if (t < 0.5f) time = (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2;
	else time = (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
	return start * (1.0f - time) + end * time;
}
#pragma endregion

#pragma region Bounce
float Easing::Bounce::easeIn(float t, float b, float c, float d)
{
	return c - Easing::Bounce::easeOut(d - t, 0, c, d) + b;
}

float Easing::Bounce::easeOut(float t, float b, float c, float d)
{
	if ((t /= d) < (1 / 2.75f)) {
		return c * (7.5625f * t * t) + b;
	}
	else if (t < (2 / 2.75f)) {
		float postFix = t -= (1.5f / 2.75f);
		return c * (7.5625f * (postFix)*t + .75f) + b;
	}
	else if (t < (2.5 / 2.75)) {
		float postFix = t -= (2.25f / 2.75f);
		return c * (7.5625f * (postFix)*t + .9375f) + b;
	}
	else {
		float postFix = t -= (2.625f / 2.75f);
		return c * (7.5625f * (postFix)*t + .984375f) + b;
	}
}

float Easing::Bounce::easeInOut(float t, float b, float c, float d)
{
	if (t < d / 2) return Easing::Bounce::easeIn(t * 2, 0, c, d) * .5f + b;
	else return Easing::Bounce::easeOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}
#pragma endregion

#pragma region Circ
float Easing::Circ::easeIn(float start, float end, float t)
{
	float time = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
	return start * (1.0f - time) + end * time;
}

float Easing::Circ::easeOut(float start, float end, float t)
{
	float time = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
	return start * (1.0f - time) + end * time;
}

float Easing::Circ::easeInOut(float start, float end, float t)
{
	float time;
	if (t < 0.5f) time = 16.0f * t * t * t * t * t;
	else time = 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	return start * (1.0f - time) + end * time;
}
#pragma endregion

#pragma region Quint
float Easing::Quint::easeIn(float start, float end, float t)
{
	float time = t * t * t * t * t;
	return start * (1.0f - time) + end * time;
}

float Easing::Quint::easeOut(float start, float end, float t)
{
	float time = 1.0f - powf(1 - t, 5.0f);
	return start * (1.0f - time) + end * time;
}

float Easing::Quint::easeInOut(float start, float end, float t)
{
	float time;
	if (t < 0.5f) time = 4.0f * t * t * t * t * t;
	else time = 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	return start * (1.0f - time) + end * time;
}
#pragma endregion

#pragma region Cubic
float Easing::Cubic::easeIn(float start, float end, float t)
{
	float time = t * t * t;
	return start * (1.0f - time) + end * time;
}

float Easing::Cubic::easeOut(float start, float end, float t)
{
	float time = 1.0f - powf(1.0f - t, 3.0f);
	return start * (1.0f - time) + end * time;
}

float Easing::Cubic::easeInOut(float start, float end, float t)
{
	float time;
	if (t < 0.5f) time = 4.0f * t * t * t;
	else time = 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
	return start * (1.0f - time) + end * time;
}
#pragma endregion

float Easing::lerp(float start, float end, float t)
{
	return (1.0f - t) * start + t * end;
}
