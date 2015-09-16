#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "pchex.h"

s32	loadPokemon(t_stinf *state, u16 slot, u8 *dest)
{
  u8 	tmp[232];
  u32 	offs = state->game ? 0x33000 : 0x22600;

  memcpy(tmp, state->save + offs + 232 * slot, 232);
  decryptPokemon(tmp, dest);
  return 0;
}

s32 	rewritechk(u8 *dec)
{
  u16 	chk = 0;

  for (int i = 8; i < 232; i += 2)
    chk += *(u16 *)(dec + i);
  memcpy(dec + 6, &chk, 2);
  return 0;
}

s32	savePokemon(t_stinf *state, u16 slot, u8 *src)
{
  u8 	enc[232];
  u32 	offs = state->game ? 0x33000 : 0x22600;

  rewritechk(src);
  encryptPokemon(src, enc);
  memcpy(state->save + offs + 232 * slot, enc, 232);
  return 0;
}

s32	switchState(t_stinf *state, struct s_UIState newst)
{
  state->curState = newst;
  state->curState.initf(state);
  return (0);
}

s32 	startLoop(u8 *save, u8 game, PrintConsole *top, PrintConsole *bot)
{
  t_stinf state;

  state.save = save;
  state.console[0] = top;
  state.console[1] = bot;
  state.game = game;
  state.curState = pkmSelectState;
  state.cont = 1;
  state.inState = 1;
  state.curState.initf(&state);
  while (state.cont > 0 && aptMainLoop())
  {
    hidScanInput();
    state.kPressed = hidKeysDown();
    state.curState.dispf(&state);
    state.curState.inputf(&state);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }
  if (state.cont == -1)
    return 1;
  return 0;
}
