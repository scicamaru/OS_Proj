#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fake_os.h"
//modified by scicamaru 29/04/2024

void FakeOS_init(FakeOS* os) {
  List_init(&os->running);
  List_init(&os->ready);
  List_init(&os->waiting);
  List_init(&os->processes);
  os->timer=0;
  os->schedule_fn=0;
}

void FakeOS_createProcess(FakeOS* os, FakeProcess* p) {
  // sanity check
  assert(p->arrival_time==os->timer && "time mismatch in creation");
  // we check that in the list of PCBs there is no
  // pcb having the same pid

  /**************************************************/
  //controllo unicità pid 
  /**************************************************/
  ListItem* aux=os->running.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }

  aux=os->ready.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }

  aux=os->waiting.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }

  // all fine, no such pcb exists
  FakePCB* new_pcb=(FakePCB*) malloc(sizeof(FakePCB));
  new_pcb->list.next=new_pcb->list.prev=0;
  new_pcb->pid=p->pid;
  new_pcb->events=p->events;
  new_pcb->durataQ=0; //durata processo a zero pronto per iniziare a contare
  new_pcb->durataCPU_pre=0; //durata processo in CPU a zero pronto per iniziare a contare
  new_pcb->durataCPU_post=0; //durata processo in CPU a zero pronto per iniziare a contare
  assert(new_pcb->events.first && "process without events");

  // depending on the type of the first event
  // we put the process either in ready or in waiting
  ProcessEvent* e=(ProcessEvent*)new_pcb->events.first;
  switch(e->type){
  case CPU: //caso event CPU 
    List_pushBack(&os->ready, (ListItem*) new_pcb); //pcb in ready
    break;
  case IO: //caso event IO
    List_pushBack(&os->waiting, (ListItem*) new_pcb); //pcb in waiting
    break;
  default:
    assert(0 && "illegal resource");
    ;
  }
}

void FakeOS_simStep(FakeOS* os){
  
  printf("************** TIME: %08d **************\n", os->timer);

  //scan process waiting to be started
  //and create all processes starting now
  ListItem* aux=os->processes.first;
  while (aux){
    FakeProcess* proc=(FakeProcess*)aux;
    FakeProcess* new_process=0;
    if (proc->arrival_time==os->timer){
      new_process=proc;
    }
    aux=aux->next;
    if (new_process) {
      printf("\tcreate pid:%d\n", new_process->pid);
      new_process=(FakeProcess*)List_detach(&os->processes, (ListItem*)new_process);
      FakeOS_createProcess(os, new_process);
      free(new_process);
    }
  }

  // scan waiting list, and put in ready all items whose event terminates
  aux=os->waiting.first;

  while(aux) {
    FakePCB* pcb=(FakePCB*)aux;
    aux=aux->next;
    ProcessEvent* e=(ProcessEvent*) pcb->events.first;
    printf("\twaiting pid: %d\n", pcb->pid);
    assert(e->type==IO);
    e->duration--;
    printf("\t\tremaining time:%d\n",e->duration);

    if (e->duration==0){
      printf("\t\tpid: %d\n",pcb->pid);
      printf("\t\tend burst\n");
      List_popFront(&pcb->events);
      free(e);
      List_detach(&os->waiting, (ListItem*)pcb);
      if (! pcb->events.first) {
        // kill process
        printf("\t\tend process\n");
        free(pcb);
      } else {
        //handle next event
        e=(ProcessEvent*) pcb->events.first;
        printf("\t\tpid: %d\n",pcb->pid);
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          List_pushBack(&os->ready, (ListItem*) pcb);
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          List_pushBack(&os->waiting, (ListItem*) pcb);
          break;
        }
      }
    }
  }

  

  // decrement the duration of running
  // if event over, destroy event
  // and reschedule process
  // if last event, destroy running
  
  aux = os->running.first; //running è il primo elemento della lista running  
  
  if(!aux) printf("\t no running process\n");
//////////////////////////////////////////////////////////
//sostituisco l'if con un while perché running è una lista
//////////////////////////////////////////////////////////
  while (aux) {
    FakePCB* running=(FakePCB*)aux;
    aux=aux->next; //spostato all'inizio no segmentation fault
    ProcessEvent* e=(ProcessEvent*) running->events.first;
    assert(e->type==CPU); //controllo che l'evento sia di tipo CPU 
    e->duration--;
    running->durataQ++; // aumento durata del prcesso nel quanto
    running->durataCPU_pre++; // aumento durata del prcesso in CPU
    printf("\t\tpid: %d\n",running->pid);
    printf("\t\tremaining time:%d\n",e->duration);
    printf("\t\ttime consumed:%f\n",running->durataQ);

    if (e->duration==0){
      printf("\t\tend burst\n");
      List_popFront(&running->events);
      free(e);
      List_detach(&os->running,(ListItem*) running);
      running->durataQ=0; //reset durata processo
      running->durataCPU_pre=0; //reset durata processo in CPU

      if (! running->events.first) {
        printf("\t\tend process\n");
        free(running); // kill process
      } else {
        e=(ProcessEvent*) running->events.first;
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          List_pushBack(&os->ready, (ListItem*) running);
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          List_pushBack(&os->waiting, (ListItem*) running);
          break;
        }
      }
  
    }
  
  }


  // call schedule, if defined
  if (os->schedule_fn &&  !os->running.first){  //sostituito os->running con os->running.first
    (*os->schedule_fn)(os, os->schedule_args); 
  }

  // if running not defined and ready queue not empty
  // put the first in ready to run
  if (! os->running.first && os->ready.first) { //sostituito os->running con os->running.first
    os->running.first = List_popFront(&os->ready); //sostituito os->running con os->running.first
  }

  ++os->timer;

}

void FakeOS_destroy(FakeOS* os) {
}
