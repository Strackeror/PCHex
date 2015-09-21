#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "state.h"

/*
 * Manage tab state
 */

char helpstringsMan[10][3][40] = {
  {"Up/Down : Select Field","A : Use Field", "Start : Back"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
};

void 	pkmManageInit(t_stinf *state)
{
  state->inSel = 0;
  state->inState = 1;
  consoleClear();
}

void 	pkmManageHelp(t_stinf *state)
{
  u8	sel = state->inState;

  printf("\x1B[26;0H");
  for (int i = 0; i < 40; i++)
    printf("-");
  if (!state->inSel)
    sel = 0;
  printf("%-40s", helpstringsMan[sel][0]);
  printf("%-40s", helpstringsMan[sel][1]);
  printf("%-40s", helpstringsMan[sel][2]);
}

void 	pkmManageDisplay(t_stinf *state)
{
  u8	ist = state->inState;
  u8 	sel = state->inSel;

  pkmHeader(state);

  printf("\n\n\n");
  selectColor(1, ist, sel);
  printf("Save\n");

  printf("\n");

  selectColor(2, ist, sel);
  printf("Copy\n");
  selectColor(3, ist, sel);
  printf("Paste\n");
  resetColor();
  printf("In clipboard : ");
  if (state->cpy.pkx.species == 0)
    printf("%-12s", "Empty");
  else
    printf("%-12s", pkData.species[state->cpy.pkx.species]);
  pkmManageHelp(state);
}

void 	pkmManageInput(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_START)
  {
    switchState(state, pkmSelectState);
    return;
  }
  if (kPressed & KEY_L)
  {
    switchState(state, pkmCombatState);
    return;
  }
  if (kPressed & KEY_UP && state->inState > 1)
    state->inState--;
  if (kPressed & KEY_DOWN && state->inState < 3)
    state->inState++;
  if (kPressed & KEY_A)
  {
    switch(state->inState)
    {
      case 1:
	savePokemon(state, state->pkmSlot, (u8 *) &state->pkm.pkx);
	state->modded = 0;
	break;
      case 2:
	state->cpy = state->pkm;
	break;
      case 3:
	state->pkm = state->cpy;
	state->modded = 1;
	break;
    }
  }
}

struct s_UIState pkmManageState = {&pkmManageInit, &pkmManageDisplay, &pkmManageInput};
