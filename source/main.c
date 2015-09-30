#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <3ds.h>

#include "pchex.h"

//Basic line separation to array of strings
s8 	loadLines(u8 *src, u8 *dst, u8 strlen,  u32 size)
{
  u16 	readnum = 3;
  u16 	line = 0, chr; 

  while (readnum < size)
  {
    chr = 0;
    while (readnum < size && src[readnum] != '\n')
    {
      dst[line * strlen + chr] = src[readnum];
      readnum++;
      chr++;
    }
    dst[line * strlen + chr] = 0;
    readnum++;
    line++;
  }
  return 0;
}

/*
 * loads general pokemon data like names and species data
 */
s8 	loadData(Handle *sdHandle, FS_archive *sdArchive)
{
  u8	tmp[12000];
  u32 	bytesRead;
  Result ret;

  printf("Loading species data...");
  ret = loadFile("/3ds/PCHex/data/personal", tmp, sdArchive, sdHandle, 12000, &bytesRead); 
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  memcpy(pkData.pkmData, tmp, bytesRead);
  printf(" OK\n");

  printf("Loading species names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Species_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.species[0], 12, bytesRead);
  printf(" OK\n");

  printf("Loading move names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Moves_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.moves[0], 17, bytesRead);
  printf(" OK\n");

  printf("Loading ability names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Abilities_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.abilities[0], 15, bytesRead);
  printf(" OK\n");

  printf("Loading item names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Items_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.items[0], 17, bytesRead);
  printf(" OK\n");

  printf("Loading ball names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Balls_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.balls[0], 13, bytesRead);
  printf(" OK\n");

  printf("Loading nature names...");
  ret = loadFile("/3ds/PCHex/data/text/text_Natures_en.txt", tmp, sdArchive, sdHandle, 12000, &bytesRead);
  if (ret) { printf("loading failed : error code %ld\n", ret); return ret; }
  loadLines(tmp, pkData.natures[0], 8, bytesRead);
  printf(" OK\n");
  return 0;
}

void 	waitKey(u32 keyWait)
{
 while (aptMainLoop())
  {
    hidScanInput();

    u32 kPressed = hidKeysDown();
    if (kPressed & keyWait) break;

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  } 
}

int 	pchexinit(struct s_pchex *pch)
{
  int 	fs;

  //General Init
  srand(time(NULL));
  gfxInitDefault();
  consoleInit(GFX_BOTTOM, &pch->bot);
  consoleInit(GFX_TOP, &pch->top);

  //Filesystem Init
  printf("Init Filesystem...\n");
  fs = filesysInit(&pch->sd.handle, &pch->sav.handle, &pch->sd.arch, &pch->sav.arch);
  if (fs)
    printf("Init FS Failed\n");
  else
    printf("Init FS OK\n");

  //Load Pokemon Data
  if (loadData(&pch->sd.handle, &pch->sd.arch))
    return -1;

  //save loading, save is loaded into the array 'save'
  pch->save = malloc(0xEB000);
  pch->game = loadSave(pch->save, &pch->sav.handle, &pch->sav.arch);
  if (pch->game < 0)
    pch->game = loadSave(pch->save, &pch->sd.handle, &pch->sd.arch);
  if (pch->game < 0)
    return -1;

  if (backupSave(pch->save, pch->game, &pch->sd.handle, &pch->sd.arch))
  {
    printf("Backup Failed, Exiting\n");
    return -1;
  }
  return 0;
}

int 	pchexexit(struct s_pchex *pch)
{
  consoleSelect(&pch->bot);
  consoleClear();
  printf("\x1B[15;2H");
  printf("Program ended, press A to finish\n");
  waitKey(KEY_A);
  free(pch->save);
  gfxExit();
  filesysExit(&pch->sd.handle, &pch->sav.handle, &pch->sd.arch, &pch->sav.arch);
  return (0);
}

int 	main()
{
  struct s_pchex pch;
  
  if (pchexinit(&pch) >= 0)
    startLoop(&pch); //main loop
    //exportSave(save, game, &saveHandle, &saveArchive);
  return pchexexit(&pch);
}
