//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include "thread.h"

void switch_vector_enclave(void){
  csr_write(mtvec, &trap_vector_enclave);
}

void switch_vector_host(void){
  csr_write(mtvec, &_trap_handler);
}

// regs -> to
// from -> regs
void rw_mstatus(struct thread_state* from, struct thread_state* to, struct sbi_trap_regs* regs) {
  uintptr_t mstatus_mask = MSTATUS_SIE | MSTATUS_SPIE | MSTATUS_SPP |
                            MSTATUS_MPP | MSTATUS_FS | MSTATUS_SUM |
                            MSTATUS_MXR;
  uintptr_t current_mstatus = regs->mstatus;
  to->prev_mstatus = (current_mstatus & ~mstatus_mask) | (current_mstatus & mstatus_mask);
  regs->mstatus = (current_mstatus & ~mstatus_mask) | from->prev_mstatus;
}

// regs -> to
// from -> regs
void rw_state(struct thread_state* from, struct thread_state* to, struct sbi_trap_regs* regs, int return_on_resume)
{
  int i;

  uintptr_t* from_prev = (uintptr_t*) &from->prev_state;
  uintptr_t* to_prev = (uintptr_t*) &to->prev_state;
  for(i=0; i<32; i++)
  {
    to_prev[i] = ((unsigned long *)regs)[i];
    ((unsigned long *)regs)[i] = from_prev[i];
  }

  to_prev[0] = !return_on_resume;

  rw_smode_csrs(from, to);

  return;
}

/* Swaps all s-mode csrs defined in 1.10 standard */
/* TODO: Right now we are only handling the ones that our test
   platforms support. Realistically we should have these behind
   defines for extensions (ex: N extension)*/
// csr -> to
// from -> csr
void rw_smode_csrs(struct thread_state* from, struct thread_state* to)
{
#define LOCAL_RW_CSR(csrname) \
  to->prev_csrs.csrname = csr_read(csrname); \
  csr_write(csrname, from->prev_csrs.csrname);

  LOCAL_RW_CSR(sstatus);
  // These only exist with N extension.
  //LOCAL_RW_CSR(sedeleg);
  //LOCAL_RW_CSR(sideleg);
  LOCAL_RW_CSR(sie);
  LOCAL_RW_CSR(stvec);
  LOCAL_RW_CSR(scounteren);
  LOCAL_RW_CSR(sscratch);
  LOCAL_RW_CSR(sepc);
  LOCAL_RW_CSR(scause);
  LOCAL_RW_CSR(sbadaddr);
  LOCAL_RW_CSR(sip);
  LOCAL_RW_CSR(satp);
#undef LOCAL_RW_CSR
}

// regs -> to
// from -> regs
void rw_mepc(struct thread_state* from, struct thread_state* to, struct sbi_trap_regs* regs)
{
  to->prev_mepc = regs->mepc;
  regs->mepc = from->prev_mepc;
}


void clean_state(struct thread_state* state){
  int i;
  uintptr_t* prev = (uintptr_t*) &state->prev_state;
  for(i=1; i<32; i++)
  {
    prev[i] = 0;
  }

  state->prev_mpp = -1; // 0x800;
  clean_smode_csrs(state);
}

void clean_smode_csrs(struct thread_state* state){

  state->prev_csrs.sstatus = 0;

  // We can't read these or set these from M-mode?
  state->prev_csrs.sedeleg = 0;
  state->prev_csrs.sideleg = 0;

  state->prev_csrs.sie = 0;
  state->prev_csrs.stvec = 0;
  // For now we take whatever the OS was doing
  state->prev_csrs.scounteren = csr_read(scounteren);
  state->prev_csrs.sscratch = 0;
  state->prev_csrs.sepc = 0;
  state->prev_csrs.scause = 0;
  state->prev_csrs.sbadaddr = 0;
  state->prev_csrs.sip = 0;
  state->prev_csrs.satp = 0;

}
