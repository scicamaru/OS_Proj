#pragma once
#include "linked_list.h"

typedef enum {CPU=0, IO=1} ResourceType;

// event of a process, is in a list
typedef struct {
  ListItem list;
  ResourceType type;
  int duration;
//l'evento deve misurare la durata di prima e dopo
//credo(?) - scicamaru 21/06/2024
  float durata_pre;
  float durata_post;
} ProcessEvent;

// fake process
typedef struct {
  ListItem list;
  int pid; // assigned by us
  int arrival_time;
  ListHead events;
} FakeProcess;

int FakeProcess_load(FakeProcess* p, const char* filename);

int FakeProcess_save(const FakeProcess* p, const char* filename);
