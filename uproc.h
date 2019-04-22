struct uproc {
  int pid;
  int ppid;
  int state;
  uint sz;
  int start_time;
  int uptime;
  int ticks;
  char name[16];
};
