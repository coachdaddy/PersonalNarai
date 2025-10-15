

#include <stdio.h>
#include <string.h>

#include "structs.h"

int
main ()
{
  struct char_file_u st;
  char name[20];
  int nac = -104;
  int ndr = 105;
  int nhr = 105;
  long offset;
  FILE *FL;

  if (!(FL = fopen ("players", "r+")))
    {
      printf ("Error open 'players' file\n");
      exit (-1);
    }

  printf ("Name? ");
  scanf ("%s", name);
  printf ("ac ? ");
  scanf ("%d", &nac);
  printf ("dr ? ");
  scanf ("%d", &ndr);
  printf ("hr ? ");
  scanf ("%d", &nhr);

  printf ("%s %d %d %d \n", name, nac, ndr, nhr);

  while (1)
    {
      fread (&st, sizeof (struct char_file_u), 1, FL);
      if (strcmp (name, st.name) == 0)
	{
	  printf ("found\n");

	  printf ("Changing\n");

	  st.points.armor = nac;
	  st.points.damroll = ndr;
	  st.points.hitroll = nhr;

	  offset = ftell (FL);
	  offset -= sizeof (struct char_file_u);
	  rewind (FL);
	  fseek (FL, offset, 0);
	  fwrite (&st, sizeof (struct char_file_u), 1, FL);
	  printf ("successful\n");

	  break;
	}
    }
  fclose (FL);
}
