#ifndef PATH_ENUMERATOR_H
#define PATH_ENUMERATOR_H

#include <goto-programs/goto_program.h>

#include <analyses/natural_loops.h>

#include "path.h"

class path_enumeratort {
 public:
  path_enumeratort(goto_programt &_goto_program,
      natural_loops_mutablet::natural_loopt &_loop,
      goto_programt::targett _loop_header) :
    goto_program(_goto_program),
    loop(_loop),
    loop_header(_loop_header)
  {
  }

  bool next(patht &path);

 protected:
  int backtrack(patht &path);
  void complete_path(patht &path, int succ);
  void extend_path(patht &path, goto_programt::targett t, int succ);
  bool is_looping(patht &path);

  goto_programt &goto_program;
  natural_loops_mutablet::natural_loopt &loop;
  goto_programt::targett loop_header;

  patht last_path;
};

#endif // PATH_ENUMERATOR_H