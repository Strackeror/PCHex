#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "state.h"

void 	pkmComStatIV(t_stinf *state, u8 stat)
{
  s8 	up = -2;
  if (stat == 0)
    up = -1;
  if (stdInputField(state, up, 2, 0, 1)) return;

  u32 	kPressed = state->kPressed;
  s8 	IV = getPkmIV(state->pkm.pkx.individualValues, stat);
  if (kPressed & KEY_UP)
    IV++;
  if (kPressed & KEY_DOWN)
    IV--;
  if (kPressed & KEY_LEFT)
    IV = 0;
  if (kPressed & KEY_RIGHT)
    IV = 31;
  if (IV < 0)
    IV = 0;
  if (IV > 31)
    IV = 31;
  if (IV != getPkmIV(state->pkm.pkx.individualValues, stat))
  {
    setPkmIV(IV, stat, &state->pkm);
    pkmRecalc(&state->pkm);
    state->modded = 1;
  }
}

void 	pkmComStatEV(t_stinf *state, u8 stat)
{
  s8	down = 2;

  if (stat == 5)
    down = 1;
  if (stdInputField(state, -2, down, -1, 0)) return;

  u32 kPressed = state->kPressed;
  s16 EV = state->pkm.pkx.effortValues[stat];

  if (kPressed & KEY_UP)
    EV += 4;
  if (kPressed & KEY_DOWN)
    EV -= 4;
  if (kPressed & KEY_LEFT)
    EV = 0;
  if (kPressed & KEY_RIGHT)
    EV = 252;
  if (kPressed & KEY_X)
    EV++;
  if (EV < 0)
    EV = 0;
  if (EV > 252)
    EV = 252;
  if (EV != state->pkm.pkx.effortValues[stat])
  {
    state->pkm.pkx.effortValues[stat] = EV;
    pkmRecalc(&state->pkm);
    state->modded = 1;
  }
}

void 	pkmComAbility(t_stinf *state)
{
  if (stdInputField(state, 0, 1, 0, 0)) return;

  u32	kPressed = state->kPressed;
  s8	abilNum = state->pkm.pkx.abilityNum;
  if (kPressed & KEY_UP)
  {
    if (abilNum == 2)
      abilNum = 4;
    else if (abilNum < 4)
      abilNum++;
  }
  if (kPressed & KEY_DOWN)
  {
    if (abilNum == 4)
      abilNum = 2;
    else if (abilNum > 0)
      abilNum--;
  }
  if (abilNum != state->pkm.pkx.abilityNum)
  {
    setPkmAbilityNum(&state->pkm, abilNum);
    state->modded = 1;
  }
}

void 	pkmComMove(t_stinf *state, u8 move)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmComInputField(t_stinf *state)
{
  if (state->inState < 14 && state->inState >= 2)
  {
    if (state->inState % 2)
      pkmComStatEV(state, (state->inState - 2) / 2);
    else
      pkmComStatIV(state, (state->inState - 2) / 2);
    return;
  }
  if (state->inState >= 14 && state->inState <= 21)
    pkmComMove(state, state->inState - 14);
 switch (state->inState)
 {
  case 1:
    pkmComAbility(state); break;
  case 22:
    stdInputField(state, -1, 0, 0, 0); break;
 }
}
