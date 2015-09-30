#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

#include "../pchex.h"
#include "state.h"

/*
 * Combat tab state
 */

char helpstringsCom[30][3][50] = {
  { "Up/Down/Left/Right : Choose field",
    "A : Select field ",
    "Start : Back" },
  {"","Up/Down : Previous/Next Ability","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","Up/Down : +/- 1 | Left/Right : Min/Max","B : Leave field"},
  {"X : +1","Up/Down : +/- 4 | Left/Right : Min/Max","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Move","B : Leave field"},
  {"","A : Choose Nature","B : Leave field"},
};

void 	pkmCombatHelp(t_stinf *state)
{
  u8	sel = state->inState;

  for (int i = 0; i < 40; i++)
    printf("-");
  if (!state->inSel)
    sel = 0;
  printf("%-40s", helpstringsCom[sel][0]);
  printf("%-40s", helpstringsCom[sel][1]);
  printf("%-40s", helpstringsCom[sel][2]);
}

void 	pkmCombatInit(t_stinf *state)
{
  state->inState = 1;
  state->inSel = 0;
  consoleClear();
}

void 	moveField(t_stinf *state, s8 tstate, s8 move)
{
  if (move > 7)
    return;
  selectColor(tstate, state->inState, state->inSel);
  if (move <= 3)
    printf("Move %d:%-16s\n", move + 1, pkData.moves[state->pkm.pkx.moves[move]]);
  else
    printf("Egg Move %d:%-16s\n", move - 3, pkData.moves[state->pkm.pkx.relearnMoves[move - 4]]);
}

void 	statField(t_stinf *state, s8 tstate, s8 stat)
{
  char	str[6][4] = {"HP", "ATK", "DEF", "SPE", "SPA", "SPD"};

  resetColor();
  printf("%-3s: %-3d\n", str[stat], state->pkm.stat[stat]);
  printf("    ");
  selectColor(tstate, state->inState, state->inSel);
  printf("IV : %-2d", getPkmIV(state->pkm.pkx.individualValues, stat));

  resetColor();printf("    ");
  selectColor(tstate + 1, state->inState, state->inSel);
  printf("EV : %-3d\n", state->pkm.pkx.effortValues[stat]);
}


void 	pkmCombatDisplay(t_stinf *state)
{
  struct s_pkx *pkx = &state->pkm.pkx;
  u8	ist = state->inState, sel = state->inSel;
  char 	tmp[9];

  pkmHeader(state);
  printf("\n");
  
  selectColor(1, ist, sel);
  pkx->abilityNum == 4 ? sprintf(tmp, "(H)") : sprintf(tmp, "(%d)", pkx->abilityNum);
  printf("Ability : %14s %s\n", pkData.abilities[pkx->ability], tmp);
  selectColor(22, ist, sel);
  printf("Nature : %8s\n", pkData.natures[pkx->nature]);

  for (int i = 0; i < 6; i++)
    statField(state, (i + 1) * 2, i);

  printf("\n");

  for (int i = 0; i < 8; i++)
    moveField(state, 14 + i, i);

  resetColor();
  printf("\x1B[26;0H");
  pkmCombatHelp(state);
}

void 	pkmCombatInput(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_START)
  {	
    switchState(state, pkmSelectState);
    return;
  }
  if (kPressed & KEY_L)
  {
    switchState(state, pkmGeneralState);
    return;
  }
  if (kPressed & KEY_R)
  {
    switchState(state, pkmManageState);
    return;
  }
  pkmComInputField(state);
}

struct s_UIState pkmCombatState = {&pkmCombatInit, &pkmCombatDisplay, &pkmCombatInput};
