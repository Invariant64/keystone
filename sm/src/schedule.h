#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

struct schedule_data {
  // SM managed metadata
  unsigned long ptime; // physical time
  unsigned long etime; // enclave time
  unsigned long time_debt;

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

bool check_resource_valid(unsigned long budget_cycles,
                          unsigned long period_ticks,
                          unsigned long time_debt_threshold);

#endif // __SCHEDULE_H__