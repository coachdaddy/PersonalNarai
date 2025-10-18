
#include <stdio.h>
#include <string.h>

#include "structs.h"

int main()
{
	struct char_file_u st;
	char name[20];
	long exp;
	int qnum;
	long offset;
	FILE *FL;

	if (!(FL = fopen("players", "r+"))) {
		printf("Error open 'players' file\n");
		exit(-1);
	}

	printf("Name? ");
	scanf("%s", name);
	printf("Exp? ");
	scanf("%ld", &exp);
	printf("Solve quest? ");
	scanf("%d", &qnum);

	printf("%s %ld\n", name, exp);

	while (1) {
		fread(&st, sizeof(struct char_file_u), 1, FL);
		if (strcmp(name, st.name) == 0) {
			printf("found\n");

			printf("Changing\n");
			st.points.exp = exp;
			st.quest.solved = qnum;

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
