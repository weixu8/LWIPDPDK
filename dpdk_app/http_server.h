#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H
int main(int argc, char **argv);
int init_dpdk(int argc, char **argv);
__attribute__((noreturn)) int network_stack(__attribute__((unused)) void*);
#endif
