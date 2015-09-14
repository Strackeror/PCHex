#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "pkx.h"

s8 	loadPokemon(t_stinf *state, u16 slot, u8 *dest)
{
  u8 	tmp[232];
  u32 	offs = state->game ? 0x22600 : 0x33000;

  memcpy(tmp, state->save + offs + 232 * slot, 232);
  decryptPokemon(tmp, dest);
  return 0;
}

s8 	switchState(t_stinf *state, struct s_UIState newst)
{
  state->curState = newst;
  state->curState.initf(state);
  return (0);
}

int 	startLoop(u8 *save, u8 game, PrintConsole *top, PrintConsole *bot)
{
  t_stinf state;

  state.save = save;
  state.console[0] = top; 
  state.console[1] = bot;
  state.game = game;
  state.curState = pkmSelectState;
  state.curState.initf(&state);
  state.cont = 1;
  state.inState = 1;
  while (state.cont && aptMainLoop())
  {
    hidScanInput();
    state.kPressed = hidKeysDown();
    state.curState.dispf(&state);
    state.curState.inputf(&state);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }
  return 0;
}
