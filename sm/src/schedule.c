#include "schedule.h"

int next_interrupt_encl_id = -1;

void set_next_interrupt_encl_id(int eid)
{
  next_interrupt_encl_id = eid;
}

int get_next_interrupt_encl_id(void)
{
  return next_interrupt_encl_id;
}