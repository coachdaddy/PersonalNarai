#pragma once

/* for queueing zones for update   */
struct reset_q_element
  {
    int zone_to_reset;		/* ref to zone_data */
    struct reset_q_element *next;
  };



/* structure for the update queue     */
struct reset_q_type
  {
    struct reset_q_element *head;
    struct reset_q_element *tail;
  }
reset_q;
