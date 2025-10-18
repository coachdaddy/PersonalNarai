
#include <stdio.h>
#include <string.h>

#include "structs.h"

int main()
{
	struct char_file_u st;
	char name[20];
	long hit;
	long mana;
	long move;
	long offset;
	FILE *FL;

	if (!(FL = fopen("players", "r+"))) {
		printf("Error open 'players' file\n");
		exit(-1);
	}

	printf("Name? ");
	scanf("%s", name);
	printf("Max hit point? ");
	scanf("%ld", &hit);
	printf("Max mana point? ");
	scanf("%ld", &mana);
	printf("Max move point? ");
	scanf("%ld", &move);

	printf("%s %ld %ld %ld\n", name, hit, mana, move);

	while (1) {
		fread(&st, sizeof(struct char_file_u), 1, FL);
		if (strcmp(name, st.name) == 0) {
			printf("found\n");

			printf("Changing\n");
			st.points.max_hit = hit;
			st.points.max_mana = mana;
			st.points.max_move = move;

			offset = ftell(FL);
			offset -= sizeof(struct char_file_u);
			rewind(FL);
			fseek(FL, offset, 0);
			fwrite(&st, sizeof(struct char_file_u), 1, FL);
			printf("successful\n");

			break;
		}
	}
	fclose(FL);
}
