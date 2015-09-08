#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include "pkx.h"

Result 	shuffleArray(u8 *array, u8 sv)
{
  u8 aloc[] = { 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 };
  u8 bloc[] = { 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 };
  u8 cloc[] = { 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1 };
  u8 dloc[] = { 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0 };
  printf("locations loaded\n");

  u8 ord[] = {aloc[sv], bloc[sv], cloc[sv], dloc[sv]};
  u8 tmp[56];
  printf("locations found\n");
  for (int b = 0; b < 4; b++)
  {
    memcpy(tmp, array + 8 + 56 * ord[b], 56);
    memcpy(array + 8 + 56 * b, tmp, 56);
  }
  return 0;	
}

u32 seedStep(u32 seed)
{
  u32 a = 0x41C64E6D;
  u32 c = 0x00006073;

  return ((seed * a + c) & 0xFFFFFFFF);
}

Result decryptPokemon(u8 *enc, u8 *dec)
{
  memcpy(dec, enc, 232);

  u32 pv = *(u32 *)dec;
  u8 sv = (((pv & 0x3E000) >> 0xD) % 24);

  u32 seed = pv;
  u16 tmp;
  for (int i = 8; i < 232; i += 2)
  {
    memcpy(&tmp, dec + i, 2);
    tmp ^= (seedStep(seed) >> 16);
    seed = seedStep(seed);
    memcpy(dec + i, &tmp, 2);
  }
  shuffleArray(dec, sv);
  return (0);
}

void 	printPkName(u8 *str)
{
  for (int i = 0; i < 24; i += 2)
    printf("%c", str[i]);
}

void 	pokemonDataDump(u8 *dec)
{
  struct s_pkx 	*pkx;

  pkx = (struct s_pkx *)dec;
  
  printf("species no %d\n", pkx->species);
  printf("Nickname : ");
  printPkName(pkx->nickname);
  printf("\nTrainer Name : ");
  printPkName(pkx->trainerName);
  printf("\n");

  printf("EVs : %d %d %d %d %d %d\n", pkx->effortValues[HP], pkx->effortValues[ATK], pkx->effortValues[DEF],
	pkx->effortValues[SPE], pkx->effortValues[SPA], pkx->effortValues[SPD]);
}
