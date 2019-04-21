#include "types.h"
#include "user.h"
#include "uproc.h"

void
print_man()
{
  printf(1, "   -st    sort process table by state\n");
  printf(1, "   -st    sort process table by running-time\n");
  printf(1, "   -sz    sort process talbe by process size\n");
  printf(1, "   -n     print number of processes in the process table\n");
  printf(1, "   -s <query>     search a process by name\n");
  printf(1, "   -i <id>        search a process by id\n");
}

void
print_header()
{
  printf(1, "PID  PPID  STATE     TIME  RUNNING-TIME  SIZE  NAME\n");
}

void
print_state(int state)
{
  if(state == 1)
    printf(1, "   EMBRYO  ");
  if(state == 2)
    printf(1, "   SLEEPING  ");
  if(state == 3)
    printf(1, "   RUNNABLE  ");
  if(state == 4)
    printf(1, "   RUNNING  ");
  if(state == 5)
    printf(1, "   ZOMBIE   ");
}

void
print_proc(struct uproc* p)
{
  printf(1, " %d  ", p->pid);
  printf(1, " %d ", p->ppid);
  print_state(p->state);
  printf(1, "  %d  ", (uptime() - p->start_time));
  printf(1, "  %d  ", (p->uptime));
  printf(1, "         %d  ", p->sz);
  printf(1, "%s\n", p->name); 
}

int 
search_procname(char* query, struct uproc *table,
        int procnum, struct uproc *result)
{
  int i;
  for (i = 0; i < procnum; i++) {
    if (strcmp(query, table[i].name) == 0) {
      result->pid = table[i].pid;
      result->ppid = table[i].ppid;
      result->state = table[i].state;
      result->sz = table[i].sz;
      result->uptime = table[i].uptime;
      result->ticks = table[i].ticks;
      result->start_time = table[i].start_time;
      strcpy(result->name, table[i].name);
      return 1;
    }
  }
  return 0;
}

int
search_procid(int query_id, struct uproc *table,
        int procnum, struct uproc *result)
{
  int i;
  for (i = 0; i < procnum; i++) {
    if (query_id == table[i].pid) {
      result->pid = table[i].pid;
      result->ppid = table[i].ppid;
      result->state = table[i].state;
      result->sz = table[i].sz;
      result->uptime = table[i].uptime;
      result->ticks = table[i].ticks;
      result->start_time = table[i].start_time;
      strcpy(result->name, table[i].name);
      return 1;
    }
  }
  return 0;
}

void sort_state(struct uproc *table, int procnum)
{
  int i, j;
  struct uproc key;

  // insertion sort
  for (i = 1; i < procnum; i++) {
    key = table[i];
    j = i-1;

    while (j >= 0 && table[j].state > key.state) {
      table[j+1] = table[j];
      j--;
    }
    table[j+1] = key;
  }
}

void sort_rtime(struct uproc *table, int procnum)
{
  int i, j;
  struct uproc key;

  //insertion sort
  for (i = 1; i < procnum; i++) {
    key = table[i];
    j = i-1;

    while (j >= 0 && table[j].uptime > key.uptime) {
      table[j+1] = table[j];
      j--;
    }
    table[j+1] = key;
  }
}

void sort_size(struct uproc *table, int procnum)
{
  int i, j;
  struct uproc key;

  // insertion sort
  for (i = 1; i < procnum; i++) {
    key = table[i];
    j = i-1;

    while (j >= 0 && table[j].sz > key.sz) {
      table[j+1] = table[j];
      j--;
    }
    table[j+1] = key;
  }
}

int
main(int argc, char* argv[])
{
  int max = 64;		// NPROCS = 64
  int procnum = 0;
  struct uproc *table;

  table = (struct uproc*)malloc(64*sizeof(struct uproc));
  if(table == 0){
    printf(1, "Out of memory\n");
    return -1;
  }

  // Get the table of processes and return number of processes
  procnum = getprocs(max, table);

  // Parse command-line option
  if (argc > 1) {

    // ps help
    if (strcmp(argv[1], "-help") == 0) {
      print_man();
      free(table);
      exit();
    }
    
    // Sort by process state
    else if (strcmp(argv[1], "-st") == 0) {
      sort_state(table, procnum);
    }

    // Sort by running time
    else if (strcmp(argv[1], "-sr") == 0) {
      sort_rtime(table, procnum);
    }
    
    // Sort by process size
    else if (strcmp(argv[1], "-sz") == 0) {
      sort_size(table, procnum);
    }

    // List number of processes
    else if (strcmp(argv[1], "-n") == 0) {
      printf(1, "Number of processes: %d\n", procnum);
    }
   
    // Search process by name 
    else if (strcmp(argv[1], "-s") == 0 && argc > 2) {
      struct uproc *p = (struct uproc*)malloc(sizeof(struct uproc));

      if ((search_procname(argv[2], table, procnum, p)) == 1) {
        print_header();
        print_proc(p);
      } else {
        printf(1, "Could not find a process with that name\n");
      }

      free(table);
      exit();
    }

    // Search process by ID
    else if (strcmp(argv[1], "-i") == 0) {

      if (argc < 3) {
        printf(1, "Invalid number of arguments\n\n");
        free(table);
        exit();
      }

      struct uproc *p = (struct uproc*)malloc(sizeof(struct uproc));

      if ((search_procid(atoi(argv[2]), table, procnum, p)) == 1) {
        print_header();
        print_proc(p);
      } else {
        printf(1, "Could not find a process with that id\n");
      }

      free(table);
      exit();
    }

    else if (strcmp(argv[1], "-ir") == 0) {

      if (argc < 4) {
        printf(1, "Invalid number of arguments\n\n");
        free(table);
        exit();
      }

      int low = atoi(argv[2]);
      int high = atoi(argv[3]);

      if (low > high) {
        printf(1, "usage: invalid ranges\n\n");
        free(table);
        exit();
      } else {
        int found = 0;

        int i;
        for (i = 0; i < procnum; i++) {
          if ((table[i].pid >= low) && (table[i].pid <= high)) {
            if (found == 0) {
              print_header();
              found = 1;
            }
            
            print_proc(&table[i]);
          }
        }

        if (found == 0) {
          printf(1, "no processes found within range\n\n");
        }

        free(table);
        exit();
      }
    }
  }

  // print entire process table

  print_header();
  for(int i = 0; i < procnum; i++)
  {
    printf(1, " %d   ", table[i].pid);

    printf(1, "%d  ", table[i].ppid);
    int state = table[i].state;
    print_state(state);
    printf(1, " %d  ", (uptime() - table[i].start_time));
    printf(1, "  %d  ", (table[i].uptime));
    printf(1, "       %d  ", table[i].sz);
    printf(1, "%s\n", table[i].name);
  }
  free(table);
  exit();
}
