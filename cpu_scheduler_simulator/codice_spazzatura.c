
  /*
  //prova
  os->running.first=List_popFront(&os->ready);
  FakePCB* pcb=(FakePCB*)os->running.first;

  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);
  */
  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
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

  /*  NO PERCHÉ DA SEGMENTATION FAULT
    //qui vedo se esiste il pcb successivo e lo assegno a running 
    aux = os->running.first; //running è il prossimo elemento della lista running
    aux = aux->next;
    running = (FakePCB*)aux;*/

        //os->running = 0;
      //NON SO COME CORREGGERLO AL MOMENTO HELP
      //tolto perché funziona
      //os->running.first= NULL; //prova solo per eliminare errori compilazione
      //non si ferma aiuto (30/04/2024)


  //assert tolto perché non funziona per più CPU
  //assert( (!os->running || os->running->pid!=p->pid) && "pid taken");
