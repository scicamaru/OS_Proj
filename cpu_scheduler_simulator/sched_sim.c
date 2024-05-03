#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;
//modified by scicamaru 29/04/2024

//struct per RR
typedef struct { //parametro round Robin
  int quantum;
} SchedRRArgs;


//nuova struct per più CPU e per SJF
typedef struct { 
  int quantum;
  int num_cpu;
  float a; //parametro per quantum prediction  (30/04/2024)
} SchedSJFArgs;


//non viene più usata 
void schedRR(FakeOSRR* os, void* args_){
  SchedRRArgs* args=(SchedRRArgs*)args_;

  // look for the first process in ready
  // if none, return
  if (! os->ready.first)
    return;

 
  FakePCB* pcb=(FakePCB*) List_popFront(&os->ready);
  os->running = pcb; //sostituisce il processo in esecuzione con il primo della lista ready
  
  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);

  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
  if (e->duration>args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&pcb->events, (ListItem*)qe);
  }
};

//funzione di scheduling SJF by scicamaru 29/04/2024
void schedSJF(FakeOS* os, void* args_){
  SchedSJFArgs* args=(SchedSJFArgs*)args_;
  // look for the first process in ready
  // if none, return
  if (! os->ready.first) 
    return;

  //trovo il processo più corto e lo metto all'inizio
  //prendo la lista ready da fakeok
  //trovo il minimo dela lista ready con list item (conversione)
  //lo metto in coda
  //il sistema prende automaticamente il primo della coda


  //FakePCB* pcb=(FakePCB*) List_popFront(&os->ready); //rimuove il primo elemento della lista ready
  //os->running=pcb; //il processo pcb è in esecuzione
  //os->running.first = pcb; //sostituisce il processo in esecuzione con il primo della lista ready
 
  //prova
  os->running.first=List_popFront(&os->ready);
  FakePCB* pcb=(FakePCB*)os->running.first;

  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);

  

  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
  
  if (e->duration>args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&pcb->events, (ListItem*)qe);
  }
  
  /*****************************************/
  //QUI QUANTUM PREDITTIVO
  //non so se è corretto spero di sì altrimenti mi sparo
  //non si ferma mai lol - scicamaru 30/04/2024
  /*****************************************/

/*  if (e->duration > args->quantum) { //se la durata del processo è maggiore del quantum
    ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent)); //alloca memoria per il nuovo evento
    qe->list.prev = qe->list.next = 0; //inizializza i puntatori a 0
    qe->type = CPU; //tipo evento CPU
    qe->duration = args->quantum; //durata del nuovo evento uguale al quantum
    e->duration -= args->quantum; //sottrai al vecchio evento la durata del quantum
    List_pushFront(&pcb->events, (ListItem*)qe); //inserisci il nuovo evento in testa alla lista di eventi del processo
  } else if (e->duration < args->a) { //se la durata del processo è maggiore di a
    ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent)); //alloca memoria per il nuovo evento
    qe->list.prev = qe->list.next = 0; //inizializza i puntatori a 0
    qe->type = CPU; //tipo evento CPU
    qe->duration = e->duration;  //durata del nuovo evento uguale alla durata del vecchio evento
    e->duration = args->a; //la durata del vecchio evento diventa a
    List_pushFront(&pcb->events, (ListItem*)qe); //inserisci il nuovo evento in testa alla lista di eventi del processo
  } else if (e->duration > args->a) {   
    ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent)); //alloca memoria per il nuovo evento
    qe->list.prev = qe->list.next = 0; //inizializza i puntatori a 0
    qe->type = CPU; //tipo evento CPU
    qe->duration = e->duration;  //durata del nuovo evento uguale alla durata del vecchio evento
    e->duration = 0; //la durata del vecchio evento diventa 0
    List_pushFront(&pcb->events, (ListItem*)qe); //inserisci il nuovo evento in testa alla lista di eventi del processo
  } 
*/
  //prova -> magari si ferma ?? - scicamaru 30/04/2024
  //non si ferma neanche così
  //non serve più perché adesso si ferma
  /*if(e->duration <= 0 && !pcb->events.first) { //List_empty(&pcb->events) mi da undefined references
    //se la durata del vecchio evento è minore o uguale a 0 e la lista di eventi è vuota
    free(pcb); //libera la memoria del processo
    return;
  }*/
  
}; 


int main(int argc, char** argv) {
  FakeOS_init(&os);
  /*SchedRRArgs srr_args;
  srr_args.quantum=5; //quanto RR
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR; //puntatore a funzione di scheduling  - schedRR
  */

  SchedSJFArgs ssjf_args;
  ssjf_args.quantum=5; //quanto RR
  os.schedule_args=&ssjf_args;
  os.schedule_fn=schedSJF; //puntatore a funzione di scheduling  - schedSJF
  //ssjf_args.a=0.8; //parametro per quantum prediction  (30/04/2024) 
  //deve essere < 1
  for (int i=1; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d\n",
           argv[i], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("num processes in queue %d\n", os.processes.size);
  while(os.running.first 
        || os.ready.first
        || os.waiting.first
        || os.processes.first){ //sostituito running con running.first
    FakeOS_simStep(&os);
  }
}
