#include <cstdio>
#include <cstdlib>
#include <cstdint>

int main(uint16_t argc, char* argv[]) {
	// argc‚Ì”‚¾‚¯ŒJ‚è•Ô‚·
	for (uint16_t i = 0; i < argc; i++) {
		// •¶š—ñargv‚Ì‚Ìi”Ô‚ğ•\¦
		printf(argv[i]);

		// ‰üs
		printf("\n");
	}

	system("pause");
	return 0;
}