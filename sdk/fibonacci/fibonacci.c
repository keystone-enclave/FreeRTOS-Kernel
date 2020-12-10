//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "eapp_utils.h"
#include "printf.h"

char arr[1024] = {0};

unsigned long fibonacci_rec(unsigned long in){
  if( in <= 1)
    return 1;
  else
    return fibonacci_rec(in-1)+fibonacci_rec(in-2);
}

void EAPP_ENTRY eapp_entry(){
  int arg = 35;
  int ret = fibonacci_rec(arg);
  printf("Enclave 1: %d\n", ret); 
  syscall_task_return();
  
}