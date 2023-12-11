#pragma once

namespace Easing {
	float lerp(float start, float end, float t);

	namespace Back {
		float easeIn(float start, float end, float t);
		float easeOut(float start, float end, float t);
		float easeInOut(float start, float end, float t);
	}

	namespace Bounce {
		float easeIn(float t, float b, float c, float d);
		float easeOut(float t, float b, float c, float d);
		float easeInOut(float t, float b, float c, float d);
	}

	namespace Circ {
		float easeIn(float start, float end, float t);
		float easeOut(float start, float end, float t);
		float easeInOut(float start, float end, float t);
	}

	namespace Quint {
		float easeIn(float start, float end, float t);
		float easeOut(float start, float end, float t);
		float easeInOut(float start, float end, float t);
	}

	namespace Cubic {
		float easeIn(float start, float end, float t);
		float easeOut(float start, float end, float t);
		float easeInOut(float start, float end, float t);
	}
}
