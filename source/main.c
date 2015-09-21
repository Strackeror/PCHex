#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

int 	main()
{
  u8 	*save = NULL;
  s32	fs;
  PrintConsole 	top, bot;
  Handle sdHandle, saveHandle;
  FS_archive sdArchive, saveArchive;
  //General Init Stuff
  gfxInitDefault(); 
  consoleInit(GFX_BOTTOM, &bot);
  consoleInit(GFX_TOP, &top);
  srand(svcGetSystemTick()); //Seeding rand to generate PIDs

  //Filesystem init
  printf("Init Filesystem...\n");
  fs = filesysInit(&sdHandle, &saveHandle, &sdArchive, &saveArchive);
  if (fs)
    printf("Init FS Failed\n");
  else
    printf("Init FS OK\n");

  //General Data loading
  if (loadData(&sdHandle, &sdArchive))
    goto end;

  //save loading, save is loaded into the array 'save'
  save = (u8 *) malloc(0xEB000);
  s8 game;
  if (fs)
    game = loadSave(save, &sdHandle, &sdArchive);
  else
    game = loadSave(save, &saveHandle, &saveArchive);
  s32 ret = 0;

  if (game >= 0)
    ret = startLoop(save, game, &top, &bot); //main loop
  consoleSelect(&top);
  if (ret)
    exportSave(save, game, &sdHandle, &sdArchive);

  end:
  //Exit stuff
  consoleSelect(&bot);
  consoleClear();
  printf("\x1B[15;2H");
  printf("Program ended, press A to finish\n");
  waitKey(KEY_A);
  free(save);
  gfxExit();
  filesysExit(&sdHandle, &saveHandle, &sdArchive, &saveArchive);
  return 0;
}
