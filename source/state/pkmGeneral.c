#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "../pkx.h"

int 	selectColor(u8 sel)
{
  if (sel)
    printf("\x1B[7m");
  else
    printf("\x1B[0m");
  return 0;
}

void 	pkmGeneralInit(t_stinf *state)
{
  state->inState = 0;
  consoleClear();
}

void 	pkmGeneralDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  printf("\x1B[0;0H");
  printf("General\n\n");
  printf("Box %-2d Slot %-2d\n", state->pkmSlot / 30 + 1, state->pkmSlot % 30 + 1);
  
  printf("\n");

  selectColor(state->inState == 1);
  printf("Dex No. %3d : %-12s\n", pkx->species, pkData.species[pkx->species]);

  selectColor(state->inState == 2);
  printf("Nickname : ");
  printPkmName(pkx->nickname);
  printf("\n");

  selectColor(state->inState == 3);
  printf("PID %8lx\n", pkx->personalityID);

  printf("\n");

  selectColor(state->inState == 4);
  printf("EXP : %-7ld\nLevel : %-3d\n\n", pkx->expPoints, state->pkm.calc.level);

  selectColor(0);
  printf("Friendship :\n");
  selectColor(state->inState == 5);
  printf("OT:%3d\n", pkx->trainerFriendship);
  selectColor(state->inState == 6);
  printf("Handler:%3d\n", pkx->handlerFriendship);
  selectColor(0);
}

void 	decSelect(t_stinf *state)
{
  switch(state->inState)
  {
    case 4:
      setPkmLevel(&state->pkm, state->pkm.calc.level - 1);
      break;
    case 5:
      if (state->pkm.pkx.trainerFriendship > 0)
	state->pkm.pkx.trainerFriendship--;
      break;
    case 6:
      if (state->pkm.pkx.handlerFriendship > 0)
	state->pkm.pkx.handlerFriendship--;
      break;
  }
}

void 	incSelect(t_stinf *state)
{
  switch(state->inState)
  {
    case 4:
      setPkmLevel(&state->pkm, state->pkm.calc.level + 1);
      break;
    case 5:
      if (state->pkm.pkx.trainerFriendship < 0xFF)
	state->pkm.pkx.trainerFriendship++;
      break;
    case 6:
      if (state->pkm.pkx.handlerFriendship < 0xFF)
	state->pkm.pkx.handlerFriendship++;
      break;
  }
}

void 	pkmGeneralInput(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_B)
  {	
    switchState(state, pkmSelectState);
    return;
  }
  if (kPressed & KEY_START)
    savePokemon(state, state->pkmSlot, (u8 *)&state->pkm.pkx);
  if (kPressed & KEY_UP)
    if (state->inState > 0)
      state->inState--;
  if (kPressed & KEY_DOWN)
    if (state->inState < 6)
      state->inState++;
  if (kPressed & KEY_LEFT)
    decSelect(state);//
  if (kPressed & KEY_RIGHT)
    incSelect(state);
}

struct s_UIState pkmGeneralState = {&pkmGeneralInit, &pkmGeneralDisplay, &pkmGeneralInput};
