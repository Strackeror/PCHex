#include <stdio.h>

#include <3ds.h>
#include "state.h"

s8 	stdInputField(t_stinf *state, s8 up, s8 down, s8 left, s8 right)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
    if (kPressed & KEY_B) {state->inSel = 0; return 1;}
  if (!state->inSel)
  {
    if (kPressed & KEY_A) {state->inSel = 1; return 1;}
    if (kPressed & KEY_UP && up) state->inState += up;
    if (kPressed & KEY_DOWN && down) state->inState += down;
    if (kPressed & KEY_LEFT && left) state->inState += left;
    if (kPressed & KEY_RIGHT && right) state->inState += right;
    return 1;
  }
  return 0;
}

s8 	dirInputField(t_stinf *state, s8 up, s8 down, s8 left, s8 right)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
    if (kPressed & KEY_B) {state->inSel = 0; return 1;}
  if (!state->inSel)
  {
    if (kPressed & KEY_A) {state->inSel = 1; return 1;}
    if (kPressed & KEY_UP && up) state->inState = up;
    if (kPressed & KEY_DOWN && down) state->inState = down;
    if (kPressed & KEY_LEFT && left) state->inState = left;
    if (kPressed & KEY_RIGHT && right) state->inState = right;
    return 1;
  }
  return 0;
}

void 	debugPrint(t_stinf *state, char *debug)
{
  consoleSelect(state->console[0]);
  printf("%s", debug);
  consoleSelect(state->console[1]);
}


void 	resetColor()
{
  printf("\x1B[0m");
}

void 	selectColor(u8 tState, u8 curState, u8 selected)
{
  if (tState == curState)
    if (selected)
      printf("\x1B[7m");
    else
      printf("\x1B[4m");
  else
    printf("\x1B[0m");
  return;
}

void  	pkmHeader(t_stinf *state)
{
  state->console[1]->cursorX = 0;
  state->console[1]->cursorY = 0;

  if (state->curState.dispf == pkmGeneralState.dispf)
    printf("\x1B[0m");
  else
    printf("\x1B[2m");
  printf("General");

  state->console[1]->cursorX = 16;

  if (state->curState.dispf == pkmCombatState.dispf)
    printf("\x1B[0m");
  else
    printf("\x1B[2m");
  printf("Combat");

  state->console[1]->cursorX = 33;

  if (state->curState.dispf == pkmManageState.dispf)
    printf("\x1B[0m");
  else
    printf("\x1B[2m");
  printf("Manage");

  printf("\n");
  if (state->curState.dispf == pkmGeneralState.dispf)
    state->console[1]->cursorX = 0;
  if (state->curState.dispf == pkmCombatState.dispf)
    state->console[1]->cursorX = 16;
  if (state->curState.dispf == pkmManageState.dispf)
    state->console[1]->cursorX = 33;
  printf("\x1B[0m<<L R>>");

  state->console[1]->cursorX = 0;
  state->console[1]->cursorY = 2;
  if (state->modded)
    printf("\x1B[31mModified");
  else
    printf("%-8s", "");
  printf("\x1B[0m");
}
