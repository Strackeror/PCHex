#ifndef STATE_H
#define STATE_H

#include "../pchex.h"

void 	resetColor();
void 	selectColor(u8, u8, u8);
void 	pkmGenInputField(t_stinf *);
void 	pkmComInputField(t_stinf *);

s8 	stdInputField(t_stinf *state, s8 up, s8 down, s8 left, s8 right);
s8 	dirInputField(t_stinf *state, s8 up, s8 down, s8 left, s8 right);

void 	debugPrint(t_stinf *, char *);
#endif /* end of include guard: STATE_H */
