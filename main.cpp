#include <cstdio>
#include <cstdlib>
#include <cstdint>

int main(uint16_t argc, char* argv[]) {
	// argc�̐������J��Ԃ�
	for (uint16_t i = 0; i < argc; i++) {
		// ������argv�̂�i�Ԃ�\��
		printf(argv[i]);

		// ���s
		printf("\n");
	}

	system("pause");
	return 0;
}