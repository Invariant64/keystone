#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

struct schedule_data {
  // SM managed metadata
  unsigned long ptime_start;
  unsigned long etime; // enclave time
  unsigned long etime_start;
  long time_debt; // may be negative
  unsigned long ptime_interrupt; // physical time at the next interrupt

  // user defined data
  unsigned long budget_cycles;
  unsigned long period_ticks;
  unsigned long time_debt_threshold;
};

struct reserved_sche_resource {
  unsigned long budget_cycles;
  unsigned long period_ticks;
  unsigned long time_debt_threshold;
  unsigned long reserved_id;
};

// bool check_resource_valid(unsigned long budget_cycles,
//                           unsigned long period_ticks,
//                           unsigned long time_debt_threshold);

void set_next_interrupt_encl_id(int eid);
int get_next_interrupt_encl_id(void);

#endif // __SCHEDULE_H__