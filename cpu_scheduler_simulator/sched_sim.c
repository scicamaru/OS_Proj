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


int dimRunningList(FakeOS* os){  //dimensione della lista running
  int dim=0;
  FakePCB* pcb=(FakePCB*)os->running.first;
  while(pcb){
    ++dim;
    pcb=(FakePCB*)pcb->list.next;
  }
  return dim;
}

ListItem* minProcess(FakeOS* os){ //trova il processo con la durata minore
  
  ListItem* minProc;
  int min_duration=-1; //inizializzo la durata minima a -1;

  if (!os->ready.first) //se la lista ready è vuota
    return NULL; //ritorna NULL

  ListItem* item=os->ready.first; //considero il primo elemento della lista ready 
  
  while(item){
    FakePCB* pcb=(FakePCB*)item; //conversione
    ProcessEvent* e=(ProcessEvent*)pcb->events.first; //prendo il primo evento del processo
    if (e->duration<min_duration || min_duration==-1){ //se la durata del processo è minore
    //della durata minima o la durata minima è -1 
      min_duration=e->duration; //la durata minima diventa la durata del processo
      minProc=item; //il processo minimo è l'elemento della lista ready
    }
    item=item->next; //passo all'elemento successivo
  }
  return minProc; //ritorno il processo minimo
}

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
 
  //ciclo sulla lista dei ready
  while(os->ready.first && dimRunningList(os)<args->num_cpu){   
    //ListItem* minProc=minProcess(os); //trova il processo con la durata minore
    ListItem* item= List_detach(&os->ready, minProcess(os)); //rimuove il processo dalla lista ready
    List_pushFront(&os->running, item); //inserisce il processo in esecuzione come primo elemento
    FakePCB* pcb=(FakePCB*)item; //conversione
    os->running.first=(ListItem*)pcb; //il processo pcb è in esecuzione + conversione
    assert(pcb->events.first); //controllo
    ProcessEvent* e = (ProcessEvent*)pcb->events.first; //prendo il primo evento del processo
    assert(e->type==CPU); //controllo

    if (e->duration>args->quantum) { //spostato controllo quantum nel while
      ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent)); //nuovo evento
      qe->list.prev=qe->list.next=0; //inizializzazione
      qe->type=CPU; //tipo CPU
      qe->duration=args->quantum; //durata quantum
      e->duration-=args->quantum; //sottrazione della durata del processo
      List_pushFront(&pcb->events, (ListItem*)qe); //inserimento dell'evento nella lista del processo
    }
    
  }
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
  ssjf_args.a=0.8; //parametro per quantum prediction  (30/04/2024)
  ssjf_args.num_cpu=atoi(argv[1]); //numero di CPU
  os.schedule_args=&ssjf_args;
  os.schedule_fn=schedSJF; //puntatore a funzione di scheduling  - schedSJF
   
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
