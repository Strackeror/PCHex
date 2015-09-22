#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "../pchex.h"
#include "state.h"

/*
 * General tab state
 */

char helpstringsGen[20][3][50] = {
  { "Up/Down/Left/Right : Choose field",
    "A : Select field ",
    "Start : Back" },
  {"","A : Choose Species","B : Leave field"},
  { "", "X : Set Species as Nickname", "B : Leave field" },
  {"X : Reroll","Y : Shinify","B : Leave field"},
  {"","Up/Down : Add/Substract a level","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","Up/Down/Left/Right : Modify value","B : Leave field"},
  {"","Up/Down/Left/Right : Modify value","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","","B : Leave field"},
  {"","X : Switch gender","B : Leave field"},
  {"0 : Not Infected","Up/Down : Add/Substract","B : Leave field"},
  {"0 : Cured / Not Infected","Up/Down : Add/Substract","B : Leave field"},
};

void 	pkmGeneralInit(t_stinf *state)
{
  state->inState = 1;
  state->inSel = 0;
  consoleClear();
}

void 	pkmGeneralHelp(t_stinf *state)
{
  u8	sel = state->inState;

  for (int i = 0; i < 40; i++)
    printf("-");
  if (!state->inSel)
    sel = 0;
  printf("%-40s", helpstringsGen[sel][0]);
  printf("%-40s", helpstringsGen[sel][1]);
  printf("%-40s", helpstringsGen[sel][2]);
}

void 	pkmGeneralTrainerDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;
  char 	tmp[50];

  selectColor(5, ist, sel);
  printf("OT:%-12s", getOTName(tmp, &state->pkm));
  resetColor(); printf("     ");
  selectColor(6, ist, sel);
  printf("Handler:%-12s", getHdlName(tmp, &state->pkm));

  selectColor(7, ist, sel);
  printf("Friendship:%-3d", pkx->trainerFriendship);
  resetColor(); printf("%-6s", "");
  selectColor(8, ist, sel);
  printf("Friendship:%-3d\n", pkx->handlerFriendship);

  selectColor(9, ist, sel);
  printf("ID :%05d\n", pkx->trainerID);

  selectColor(10, ist, sel);
  printf("SID:%05d\n", pkx->trainerSecretID);

  printf("\n");
}

void 	pkmGeneralOrigDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;

  selectColor(11, ist, sel);
  printf("Met Location ID: %-5d\n", pkx->metLocation);
  
  selectColor(12, ist, sel);
  printf("Met Date: %02d/%02d/%04d\n", pkx->metDate[1], pkx->metDate[2], 2000 + pkx->metDate[0]);

  selectColor(13, ist, sel);
  printf("Original Game ID: %-3d\n", pkx->gameID);

  printf("\n");
}

void 	pkmGeneralItemDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;

  selectColor(14, ist, sel);
  printf("Held Item:%-16s\n", pkData.items[pkx->item]);

  selectColor(15, ist, sel);
  printf("Pokeball:%-16s\n", pkData.balls[pkx->ballType]);
  
}

void 	pkmGeneralDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;
  char 	tmp[50];

  pkmHeader(state);
  printf("\n");

  printf("Box %-2d Slot %-2d\n", state->pkmSlot / 30 + 1, state->pkmSlot % 30 + 1);
  printf("\n");

  selectColor(1, ist, sel);
  printf("Dex No. %3d : %-12s\n", pkx->species, pkData.species[pkx->species]);

  selectColor(2, ist, sel);
  printf("Nickname : %-12s", getNickname(tmp, &state->pkm));
  printf("\n");

  selectColor(3, ist, sel);
  printf("PID : %08lx", pkx->personalityID);
  printf("    Shiny : %-3s\n", state->pkm.isShiny ? "Yes" : "No");
  resetColor();

  selectColor(16, ist, sel);
  char	genderstr[3][8] = {"Male", "Female", "None"};
  printf("Gender: %-8s\n", genderstr[state->pkm.gender]);
  printf("\n");

  resetColor();
  printf("PKRS:    ");
  selectColor(17, ist, sel);
  printf("Strain:%-2d", state->pkm.pkx.pokerus >> 4);
  resetColor(); printf("    ");
  selectColor(18, ist, sel);
  printf("Left:%-2d\n", state->pkm.pkx.pokerus % 0x10);
  selectColor(4, ist, sel);

  printf("\n");
  printf("EXP : %-7ld\nLevel : %-3d\n\n", pkx->expPoints, state->pkm.level);

  pkmGeneralTrainerDisplay(state);
  pkmGeneralOrigDisplay(state);
  pkmGeneralItemDisplay(state);

  resetColor();
  printf("\x1B[26;0H");
  pkmGeneralHelp(state);
}

void 	pkmGeneralInput(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_START)
  {	
    switchState(state, pkmSelectState);
    return;
  }
  if (kPressed & KEY_R)
  {
    switchState(state, pkmCombatState);
    return;
  }
  pkmGenInputField(state);
}

struct s_UIState pkmGeneralState = {&pkmGeneralInit, &pkmGeneralDisplay, &pkmGeneralInput};

