#include <3ds.h>
#include "../pchex.h"

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

void 	pkmGenSpecies(t_stinf *state)
{
  if (stdInputField(state, 0, 1, 0, 0)) return;
}

void 	pkmGenNickname(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (stdInputField(state, -1, 1, 0, 0)) return;
  if (kPressed & KEY_X) {setNickname((char *) pkData.species[state->pkm.pkx.species], &state->pkm); state->modded = 1;}
}

void 	pkmGenPID(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;

  u32 	kPressed = state->kPressed;
  if (kPressed & KEY_X) {rerollPID(&state->pkm); state->modded = 1;}
  if (kPressed & KEY_Y) {rerollPIDShiny(&state->pkm); state->modded = 1;}
}

void 	pkmGenEXP(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (stdInputField(state, -1, 1, 0, 0)) return;

  s8 	addLevel = 0;
  if (kPressed & KEY_UP)
    addLevel += 1;
  if (kPressed & KEY_DOWN)
    addLevel -= 1;
  if (addLevel)
  {
    setPkmLevel(&state->pkm, state->pkm.level + addLevel);
    state->modded = 1;
  }
}

void 	pkmGenOT(t_stinf *state)
{
  if (stdInputField(state, -1, 2, 0, 1)) return;
}

void 	pkmGenHandler(t_stinf *state)
{
  if (stdInputField(state, -2, 2, -1, 0)) return;
}

void 	pkmGenTrainerFrd(t_stinf *state)
{
  if (stdInputField(state, -2, 2, 0, 1)) return;

  s8 	addFrd = 0;
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_RIGHT) addFrd += 10;
  if (kPressed & KEY_LEFT) addFrd -= 10;
  if (kPressed & KEY_UP) addFrd++;
  if (kPressed & KEY_DOWN) addFrd--;
  if (addFrd) state->modded = 1;
  state->pkm.pkx.trainerFriendship = (s8)state->pkm.pkx.trainerFriendship + addFrd;
}

void 	pkmGenHandlerFrd(t_stinf *state)
{
  if (stdInputField(state, -2, 1, -1, 0)) return;

  s8 	addFrd = 0;
  u32 	kPressed = state->kPressed;

  if (kPressed & KEY_RIGHT) addFrd += 10;
  if (kPressed & KEY_LEFT) addFrd -= 10;
  if (kPressed & KEY_UP) addFrd++;
  if (kPressed & KEY_DOWN) addFrd--;
  if (addFrd) state->modded = 1;
  state->pkm.pkx.handlerFriendship = (s8)state->pkm.pkx.handlerFriendship + addFrd;
}

void 	pkmGenTrainerID(t_stinf *state)
{
  if (stdInputField(state, -2, 1, 0, 0)) return;
}

void 	pkmGenSID(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmGenMetLocation(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmGenMetDate(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmGenOrigGame(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmGenHeldItem(t_stinf *state)
{
  if (stdInputField(state, -1, 1, 0, 0)) return;
}

void 	pkmGenPokeball(t_stinf *state)
{
  if (stdInputField(state, -1, 0, 0, 0)) return;
}

void 	pkmGenInputField(t_stinf *state)
{
  switch (state->inState){
    case 1: pkmGenSpecies(state); break;
    case 2: pkmGenNickname(state); break;
    case 3: pkmGenPID(state); break;
    case 4: pkmGenEXP(state); break;
    case 5: pkmGenOT(state); break;
    case 6: pkmGenHandler(state); break;
    case 7: pkmGenTrainerFrd(state); break;
    case 8: pkmGenHandlerFrd(state); break;
    case 9: pkmGenTrainerID(state); break;
    case 10: pkmGenSID(state); break;
    case 11: pkmGenMetLocation(state); break;
    case 12: pkmGenMetDate(state); break;
    case 13: pkmGenOrigGame(state); break;
    case 14: pkmGenHeldItem(state); break;
    case 15: pkmGenPokeball(state); break;
  }
}
