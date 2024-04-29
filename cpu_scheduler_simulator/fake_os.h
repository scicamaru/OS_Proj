#include "fake_process.h"
#include "linked_list.h"
#pragma once

//not modified just commented
//by scicamaru 29/04/2024

typedef struct {
  ListItem list;
  int pid;
  ListHead events;
} FakePCB;

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
  FakePCB* running; //coda running di PCB
  ListHead ready; //coda ready di PCB - primo evento CPU
  ListHead waiting; //coda waiting di PCB - primo evento IO
  int timer;
  ScheduleFn schedule_fn; //puntatore a funzione di scheduling  - schedRR o schedSJF**
  void* schedule_args; //argomenti per la funzione di scheduling

  ListHead processes; //coda di PCB
} FakeOS;

void FakeOS_init(FakeOS* os); //inizializza FakeOS
void FakeOS_simStep(FakeOS* os); //simula un passo di tempo di FakeOS
void FakeOS_destroy(FakeOS* os); //dealloca FakeOS

//** funzione schedSJF in sched_sim.c non implementata 