#ifndef PCHEX_H
#define PCHEX_H

#include <3ds.h>
#include "pkx.h"

struct s_stateInfo;

struct s_UIState
{
  void (*initf)(struct s_stateInfo *);
  void (*dispf)(struct s_stateInfo *);
  void (*inputf)(struct s_stateInfo *);
};

struct s_stateInfo
{
  struct s_UIState curState;
  u32 	kPressed;
  u8 	inState;
  s16 	pkmSlot;
  u8 	game;
  u8 	*save;
  PrintConsole *console[2];
  s8 	cont;
  struct s_pkm pkm;
};

typedef struct s_stateInfo t_stinf;

extern struct s_UIState pkmSelectState;
extern struct s_UIState pkmGeneralState;

s32     saveFile(char *path, void *src, u64 size, FS_archive *archive, Handle *fsHandle, u32 *bytesWritten);
s32 	loadFile(char *path, void *dst, FS_archive *fsarch, Handle *fshdl, u64 maxSize, u32 *bytesRead);
s32 	filesysInit(Handle *, Handle *, FS_archive *, FS_archive *);
s32 	filesysExit(Handle *, Handle *, FS_archive *, FS_archive *);

s32 	exportSave(u8 *save, u8 game, Handle *fshdl, FS_archive *fsarch);
s32 	loadSave(u8 *save, Handle *fshdl, FS_archive *fsarch);

s32	loadPokemon(t_stinf *state, u16 slot, u8 *dest);
s32	savePokemon(t_stinf *state, u16 slot, u8 *src);
u32 	getCHKOffset(u8 game, u8 type, u8 index);
s32 	switchState(t_stinf *state, struct s_UIState newst);
s32 	startLoop(u8 *, u8, PrintConsole *, PrintConsole *);

#endif /* end of include guard: PCHEX_H */
