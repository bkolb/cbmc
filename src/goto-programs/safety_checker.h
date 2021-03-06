/*******************************************************************\

Module: Safety Checker Interface

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_SAFETY_CHECKER_H
#define CPROVER_SAFETY_CHECKER_H

// this is just an interface -- it won't actually do any checking!

#include <util/message.h>

#include "goto_trace.h"
#include "goto_functions.h"

class safety_checkert:public messaget
{
public:
  explicit safety_checkert(
    const namespacet &_ns);

  explicit safety_checkert(
    const namespacet &_ns,
    message_handlert &_message_handler);

  typedef enum { SAFE, UNSAFE, ERROR } resultt;

  // check whether all assertions in goto_functions are safe
  // if UNSAFE, then a trace is returned

  virtual resultt operator()(
    const goto_functionst &goto_functions)=0;

  // this is the counterexample  
  goto_tracet error_trace;

protected:
  // the namespace
  const namespacet &ns;
};

#endif
