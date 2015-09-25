#include <3ds.h>
#include "state.h"

void 	pkmGenSpecies(t_stinf *state)
{
  if (dirInputField(state, 0, 2, 0, 19)) return;
  if (state->kPressed & KEY_A)
  {
    u16 target = overlayGetpkm();
    if (target < 0)
      return;
    setPkmSpecies(&state->pkm, target);
    state->modded = 1;
  }
}

void 	pkmGenForm(t_stinf *state)
{
  if (dirInputField(state, 0, 2, 1, 0)) return;

  s8 add = 0;
  s8 form = state->pkm.pkx.formFlags >> 3;
  if (state->kPressed & KEY_UP)
    add = 1;
  if (state->kPressed & KEY_DOWN)
    add = -1;
  form += add;
  if (form < 0)
    form = 0;
  if (form > pkData.pkmData[state->pkm.pkx.species][0xC])
    form = pkData.pkmData[state->pkm.pkx.species][0xC];
  if (form != state->pkm.pkx.formFlags >> 3)
  {
    setPkmForm(&state->pkm, form);
    state->modded = 1;
  }
}

void 	pkmGenNickname(t_stinf *state)
{
  u32 	kPressed = state->kPressed;

  if (stdInputField(state, -1, 1, 0, 0)) return;
  if (kPressed & KEY_X) {setNickname((char *) pkData.species[state->pkm.pkx.species], &state->pkm); state->modded = 1;}
}

void 	pkmGenPID(t_stinf *state)
{
  if (dirInputField(state, 2, 16, 0, 0)) return;

  u32 	kPressed = state->kPressed;
  if (kPressed & KEY_X) {rerollPIDspe(&state->pkm, 2, state->pkm.gender); state->modded = 1;}
  if (kPressed & KEY_Y) {rerollPIDspe(&state->pkm, 1, state->pkm.gender); state->modded = 1;}
}

void 	pkmGenGender(t_stinf *state) //16
{
  if (dirInputField(state, 3, 17, 0, 0)) return;

  u32 kPressed = state->kPressed;
  if (kPressed & KEY_X)
  {
    if (state->pkm.gender == 2)
      return;
    if (pkData.pkmData[state->pkm.pkx.species][0xA] == 0 ||
	pkData.pkmData[state->pkm.pkx.species][0xA] >= 254)
      return;
    setPkmGender(&state->pkm, !state->pkm.gender);
    state->modded = 1;
  }
}

void 	pkmGenPKRSStrain(t_stinf *state) //17
{
  if (dirInputField(state, 16, 4, 0, 18)) return;

  u32	kPressed = state->kPressed;
  s8	strain = state->pkm.pkx.pokerus >> 4;

  if (kPressed & KEY_UP)
    strain += 1;
  if (kPressed & KEY_DOWN)
    strain -= 1;
  if (strain < 0)
    strain = 0;
  if (strain > 15)
    strain = 15;
  if (strain != state->pkm.pkx.pokerus >> 4)
  {
    state->pkm.pkx.pokerus &= 0xF;
    state->pkm.pkx.pokerus ^= strain << 4;
    state->modded = 1;
  }
}

void 	pkmGenPKRSLeft(t_stinf *state) //18
{
  if (dirInputField(state, 16, 4, 17, 0)) return;

  u32	kPressed = state->kPressed;
  s8	strain = state->pkm.pkx.pokerus & 0xF;

  if (kPressed & KEY_UP)
    strain += 1;
  if (kPressed & KEY_DOWN)
    strain -= 1;
  if (strain < 0)
    strain = 0;
  if (strain > 15)
    strain = 15;
  if (strain != (state->pkm.pkx.pokerus & 0xF))
  {
    state->pkm.pkx.pokerus &= 0xF0;
    state->pkm.pkx.pokerus ^= strain;
    state->modded = 1;
  }
}

void 	pkmGenEXP(t_stinf *state)
{
  u32 	kPressed = state->kPressed;
  if (dirInputField(state, 17, 5, 0, 0)) return;

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
  if (state->kPressed & KEY_A)
  {
    s16 target = overlayGetItems();
    if (target < 0)
      return;
    state->pkm.pkx.item = target;
    state->modded = 1;
    state->inSel = 0;
  }
}

void 	pkmGenPokeball(t_stinf *state)
{
  if (stdInputField(state, -1, 0, 0, 0)) return;
  if (state->kPressed & KEY_A)
  {
    s8 target = overlayGetBalls();
    if (target < 0)
      return;
    state->pkm.pkx.ballType = target;
    state->modded = 1;
    state->inSel = 0;
  }
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
    case 16: pkmGenGender(state); break;
    case 17: pkmGenPKRSStrain(state); break;
    case 18: pkmGenPKRSLeft(state); break;
    case 19: pkmGenForm(state); break;
  }
}
