//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "user_utils.h"

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
  ret = ret - arr[0]; 
  arr[0] = 5; 
  syscall_task_return();
  
}