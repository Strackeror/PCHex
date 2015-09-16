#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "../pchex.h"

char helpstrings[10][3][50] = {
  { "Up/Down/Left/Right : Choose field",
    "A : Select field | Start : Save",
    "Select : Back" },
  {"","","B : Leave field"},
  { "",
    "X : Set Species as Nickname",
    "B : Leave field" },
  {"","","B : Leave field"},
  {"","Up/Down : Add/Substract a level","B : Leave field"}
};

void 	resetColor()
{
  printf("\x1B[0m");
}

int 	selectColor(u8 tState, u8 curState, u8 selected)
{
  if (tState == curState)
    if (selected)
      printf("\x1B[7m");
    else
      printf("\x1B[4m");
  else
    printf("\x1B[0m");
  return 0;
}

void 	pkmGeneralInit(t_stinf *state)
{
  state->inState = 1;
  state->inSel = 0;
  state->modded = 1;
  consoleClear();
}

void 	pkmGeneralHelp(t_stinf *state)
{
  u8	sel = state->inState;

  for (int i = 0; i < 40; i++)
    printf("-");
  if (!state->inSel)
    sel = 0;
  printf("%-40s", helpstrings[sel][0]);
  printf("%-40s", helpstrings[sel][1]);
  printf("%-40s", helpstrings[sel][2]);
}

void 	pkmGeneralDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;
  char 	tmp[50];

  printf("\x1B[0;0H");
  printf("General\n\n");
  printf("Box %-2d Slot %-2d\n", state->pkmSlot / 30 + 1, state->pkmSlot % 30 + 1);
  
  printf("\n");

  selectColor(1, ist, sel);
  printf("Dex No. %3d : %-12s\n", pkx->species, pkData.species[pkx->species]);

  selectColor(2, ist, sel);
  printf("Nickname : %-12s", getNickname(tmp, &state->pkm));
  printf("\n");

  selectColor(3, ist, sel);
  printf("PID %8lx\n", pkx->personalityID);

  printf("\n");

  selectColor(4, ist, sel);
  printf("EXP : %-7ld\nLevel : %-3d\n\n", pkx->expPoints, state->pkm.level);

  selectColor(5, ist, sel);
  printf("OT:%-12s", getOTName(tmp, &state->pkm));
  resetColor(); printf("     ");

  selectColor(6, ist, sel);
  printf("Handler:%-12s\n", getHdlName(tmp, &state->pkm));

  resetColor();
  printf("\x1B[26;0H");
  pkmGeneralHelp(state);
}

void 	pkmGenSpecies(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
  {
    if (kPressed & KEY_B) {state->inSel = 0; return;}
  }
  else
  {
    if (kPressed & KEY_A) {state->inSel = 1; return;}
    if (kPressed & KEY_DOWN) state->inState++;
  }
}

void 	pkmGenNickname(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
  {
    if (kPressed & KEY_B) {state->inSel = 0; return;}
    if (kPressed & KEY_X) {setNickname((char *) pkData.species[state->pkm.pkx.species], &state->pkm);}
  }
  else
  {
    if (kPressed & KEY_A) {state->inSel = 1; return;}
    if (kPressed & KEY_UP) state->inState--;
    if (kPressed & KEY_DOWN) state->inState++;
  }
}

void 	pkmGenPID(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
  {
    if (kPressed & KEY_B) {state->inSel = 0; return;}
  }
  else
  {
    if (kPressed & KEY_A) {state->inSel = 1; return;}
    if (kPressed & KEY_UP) state->inState--;
    if (kPressed & KEY_DOWN) state->inState++;
  }
}

void 	pkmGenEXP(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (state->inSel)
  {
    if (kPressed & KEY_B) {state->inSel = 0; return;}
    s8 	addLevel = 0;
    if (kPressed & KEY_UP)
      addLevel += 1;
    if (kPressed & KEY_DOWN)
      addLevel -= 1;
    setPkmLevel(&state->pkm, state->pkm.level + addLevel);
  }
  else
  {
    if (kPressed & KEY_A) {state->inSel = 1; return;}
    if (kPressed & KEY_UP) state->inState--;
    //if (kPressed & KEY_DOWN) state->inState++;
  }
}

void 	pkmGeneralInput(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_SELECT)
  {	
    switchState(state, pkmSelectState);
    return;
  }
  if (kPressed & KEY_START)
    savePokemon(state, state->pkmSlot, (u8 *)&state->pkm.pkx);
  switch (state->inState){
    case 1: pkmGenSpecies(state); break;
    case 2: pkmGenNickname(state); break;
    case 3: pkmGenPID(state); break;
    case 4: pkmGenEXP(state); break;
  }
}

struct s_UIState pkmGeneralState = {&pkmGeneralInit, &pkmGeneralDisplay, &pkmGeneralInput};
