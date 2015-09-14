#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "pkx.h"

int 	printPKMScreen(struct s_pkx *pkx, PrintConsole *top, PrintConsole *bot, u16 pkSlot, struct s_pkxcalc *calc)
{
  consoleSelect(bot);
  printf("\x1B[0;0H"); //cursor home

  //name
  printf("\x1B[37;44m"); //White on Blue
  printPkmName(pkx->nickname);
  printf("\n");

  //species
  printf("\x1B[37;40m"); //White on Black
  printf("DEX No. %3d : %12s\n", pkx->species, pkData.species[pkx->species]);
  printf("Box %2d  Slot %2d\n", pkSlot / 30 + 1, pkSlot % 30 + 1);
  printf("\n");

  //Stats
  printf("Level %3d\n", calc->level);
  printf("PID %08x\n", pkx->personalityID);
  printf("HP -%3d  EV:%3d IV:%02d\n", calc->finalStats[HP], pkx->effortValues[HP],  getPkmIV(pkx->individualValues, HP));
  printf("ATK-%3d  EV:%3d IV:%02d\n", calc->finalStats[ATK], pkx->effortValues[ATK],  getPkmIV(pkx->individualValues, ATK));
  printf("DEF-%3d  EV:%3d IV:%02d\n", calc->finalStats[DEF], pkx->effortValues[DEF],  getPkmIV(pkx->individualValues, DEF));
  printf("SPE-%3d  EV:%3d IV:%02d\n", calc->finalStats[SPE], pkx->effortValues[SPE],  getPkmIV(pkx->individualValues, SPE));
  printf("SPA-%3d  EV:%3d IV:%02d\n", calc->finalStats[SPA], pkx->effortValues[SPA],  getPkmIV(pkx->individualValues, SPA));
  printf("SPD-%3d  EV:%3d IV:%02d\n", calc->finalStats[SPD], pkx->effortValues[SPD],  getPkmIV(pkx->individualValues, SPD));

  printf("\n");

  //Trainers
  printf("OT : ");
  printPkmName(pkx->trainerName);
  printf("  ID:%5d SID:%05d\n", pkx->trainerID, pkx->trainerSecretID);
  printf("Current Trainer  : ");
  printPkmName(pkx->latestHandler);
  printf("\n");

  printf("\n");

  //Moves
  for(int i = 0; i < 4; i++)
    printf("Move %d: %16s\n   PP:%2d PPups:%1d\n", i + 1, pkData.moves[pkx->moves[i]], pkx->movePP[i], pkx->movePPUps[i]);

  printf("\n");

  //Abilities
  printf("Ability : %14s (%d)\n", pkData.abilities[pkx->ability], pkx->abilityNum);
  consoleSelect(top);
  return 0;
}

void 	getPokemon(u32 offset, u8 *enc, u8 *save)
{
  memcpy(enc, save + offset, 232);
}

int 	startLoop(u8 *save, u32 boxOffset, PrintConsole *top, PrintConsole *bot)
{
  u16 	pkSlot = 0;
  u8 	refresh = 2;

  u8 	*enc = (u8 *)malloc(232);
  u8 	*dec = (u8 *)malloc(232);

  u32 	kPressed;
  struct s_pkxcalc 	calc;
  struct s_pkx 		*pkx = NULL;

  while (aptMainLoop())
  {
    hidScanInput();
    kPressed = hidKeysDown();

    if (refresh)
    {
      if (refresh > 1)
      {
	getPokemon(boxOffset + pkSlot * 232, enc, save);
	decryptPokemon(enc, dec);
	pkx = (struct s_pkx *)dec;
	calc.level = getPkmLevel(pkx->species, pkx->expPoints);
	for (int i = HP; i <= SPD; i++)
	  calc.finalStats[i] = getPkmStat(pkx->species, getPkmIV(pkx->individualValues, i), pkx->effortValues[i], pkx->nature,
					  calc.level, i);
      }
      printPKMScreen(pkx, top, bot, pkSlot, &calc);
      refresh = 0;
    }
    if (kPressed)
      refresh = 1;
    if (kPressed & KEY_L)
      if (pkSlot > 0)
      {
	pkSlot--;
	refresh = 2;
      }
    if (kPressed & KEY_R)
      if (pkSlot < 930)
      {
	pkSlot++;
	refresh = 2;
      }
    if (kPressed & KEY_START)
      break;
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }
  free(dec);
  free(enc);
  return 0;
}
