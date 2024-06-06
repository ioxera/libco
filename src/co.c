#include "co.h"
#include "list.h"
#include <assert.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 1024 * 8
#define STACK_ALIGNMENT(ADDR, ALIGNMENT) ADDR - ADDR % ALIGNMENT

typedef enum _co_status { CO_NEW, CO_RUNNING, CO_WAITING, CO_DEAD } co_status;

typedef struct co {
  struct list_head co_list;
  char *name;
  void (*func)(void *);
  void *arg;
  jmp_buf context;
  struct co *waiter;
  co_status status;
  uint8_t stack[STACK_SIZE];
} co;
co *current = NULL;

void _init_current() {
  if (current == NULL) {
    current = malloc(sizeof(co));
    INIT_LIST_HEAD(&current->co_list);
    current->name = malloc(strlen("main") + 1);
    strncpy(current->name, "main", strlen("main") + 1);
    current->func = NULL;
    current->arg = NULL;
    current->waiter = NULL;
    current->status = CO_RUNNING;
  }
  assert(current);
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  _init_current();
  co *c = malloc(sizeof(co));
  c->name = malloc(strlen(name) + 1);
  strncpy((char *)c->name, name, strlen(name) + 1);
  c->func = func;
  c->arg = arg;
  c->waiter = NULL;
  c->status = CO_NEW;
  list_add(&c->co_list, &current->co_list);
  return c;
}

void _co_free(co *co) {
  list_del(&co->co_list);
  free(co->name);
  co->name = NULL;
  free(co);
}

void co_wait(struct co *co) {
  assert(current);
  current->status = CO_WAITING;
  co->waiter = current;
  do {
    if (co->status == CO_DEAD) {
      _co_free(co);
      break;
    } else
      co_yield ();
  } while (1);
  current->status = CO_RUNNING;
}

static inline co *_co_next() {
  assert(current);
  co *next = current;
  do {
    next = (co *)next->co_list.next;
  } while (next->status == CO_DEAD);
  return next;
}

//__attribute__((naked))
static inline void stack_switch_call(void *sp, void *entry, void *arg) {
  asm volatile(
#if __x86_64__
      "movq %%rsp,-0x10(%0); leaq -0x20(%0), %%rsp; movq %2, %%rdi ; call *%1; "
      "movq -0x10(%0) ,%%rsp;"
      :
      : "b"(STACK_ALIGNMENT((uintptr_t)sp, 16)), "d"(entry), "a"(arg)
      : "memory"
#else
      "movl %%esp, -0x8(%0); leal -0xC(%0), %%esp; movl %2, -0xC(%0); call "
      "*%1;movl -0x8(%0), %%esp"
      :
      : "b"(STACK_ALIGNMENT((uintptr_t)sp, 8)), "d"(entry), "a"(arg)
      : "memory"
#endif
  );
}

void co_yield () {
  _init_current();
  int flag = setjmp(current->context);
  if (flag == 0) {
    current = _co_next();
    if (current->status == CO_NEW) {
      current->status = CO_RUNNING;
      stack_switch_call(&current->stack[STACK_SIZE], (void *)current->func,
                        current->arg);
      current->status = CO_DEAD;
      if (current->waiter) {
        current = current->waiter;
        longjmp(current->context, 1);
      }
    } else
      longjmp(current->context, 1);
  }
}
