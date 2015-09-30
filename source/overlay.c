#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <3ds.h>
#include "pchex.h"
#include "overlay.h"

void 	miniConsoleInit(u32 screen, PrintConsole *console)
{
  console->consoleInitialised = 1;

  gfxSetScreenFormat(screen,GSP_RGB565_OES);
  gfxSetDoubleBuffering(screen, 0);
  gfxSwapBuffers();
  gspWaitForVBlank();

  console->frameBuffer = (u16*)gfxGetFramebuffer(screen, GFX_LEFT, NULL, NULL);
  consoleSelect(console);
  consoleClear();
}


void 	drawBox(PrintConsole *console, int width, int height)
{
  console->cursorX = 0;
  console->cursorY = 0;
  printf("+");
  for (int i = 0; i < width - 2; i++)
    printf("-");
  printf("+");
  for (int i = 0; i < height - 2; i++)
  {
    printf("|");
    for (int j = 0; j < width - 2; j++)
      printf(" ");
    printf("|");
  }
  printf("+");
  for (int i = 0; i < width - 2; i++)
    printf("-");
  printf("+");
}

void 	printEditable(struct s_overlay *over)
{
  printf("\x1B[1;1H");
  printf("%s", over->title);

  if (over->dataIndex < 0)
    printf("\x1B[2;%dH^", over->offs + 1);
  printf("\x1B[3;1H%s", over->dst);
  if (over->dataIndex < 0)
  {
    printf("\x1B[2m%c\x1B[0m", over->list[over->index]);
    printf("\x1B[4;%dHv", over->offs + 1);
  }

  for (int i = 0; i < 10; i++)
  {
    if (over->foundIndex[i] < 0)
      break;
    if (over->dataIndex == i)
      printf("\x1B[7m");
    printf("\x1B[%d;1H%s", i + 5, over->data + over->foundIndex[i] * over->entrylen);
    printf("\x1B[0m");
  }
  printf("\x1B[15;1H\x1B[2m");
  if (over->dataIndex < 0)
  {
    printf("U/D:Change  Start:List\n");
    printf("\x1B[16;1HA:Add char  B:Rem Char");
  } else {
    printf("U/D:Change  A:Choose\n");
    printf("\x1B[16;1HB:Back");
  }
  printf("\x1B[0m");
}

s8	inputOverlay(struct s_overlay *over)
{
  u32 	kPressed;

  hidScanInput();
  kPressed = hidKeysDown();

  if (kPressed & KEY_DOWN)
    if (over->list[over->index + 1])
      over->index++;
  if (kPressed & KEY_UP)
    if (over->index)
      over->index--;
  if (kPressed & KEY_A)
  {
    if (over->offs < over->maxlen)
    {
      over->dst[over->offs++] = over->list[over->index];
      over->dst[over->offs] = 0;
    }
    return 1;
  }
  if (kPressed & KEY_B)
  {
    if (over->offs > 0)
      over->dst[--over->offs] = 0;
    else
      return -1;
    return 1;
  }
  if (kPressed & KEY_START)
  {
   drawBox(&over->win, 28, 18);
   if (over->data && over->foundIndex[0] >= 0)
     over->dataIndex = 0;
  }
  if (kPressed & KEY_SELECT)
    return -1;
  return 0;
}

s8	doSearch(struct s_overlay *over)
{
  s16 	cur = 0;
  u16 	found = 0;

  while (cur < over->datacount && found < 10)
  {
    if (!strncasecmp(over->dst, (char *)over->data + cur * over->entrylen, over->offs))
      over->foundIndex[found++] = cur;
    cur++;
  }
  over->foundIndex[found] = -1;
  return 0;
}

s8	inputDataSelect(struct s_overlay *over)
{
  u32 	kPressed;

  hidScanInput();
  kPressed = hidKeysDown();

  if (kPressed & KEY_UP && over->dataIndex > 0)
    over->dataIndex--;
  if (kPressed & KEY_DOWN && over->dataIndex < 9 && over->foundIndex[over->dataIndex + 1] >= 0)
    over->dataIndex++;
  if (kPressed & KEY_A)
    return -1;
  if (kPressed & KEY_B)
    over->dataIndex = -1;
  return 0;
}

void 	launchOverlay(struct s_overlay *over)
{
  PrintConsole 		*old;
  s8			ret;

  old = consoleSelect(NULL);
  consoleSetWindow(&over->win, 5, 5, 28, 18);
  miniConsoleInit(GFX_BOTTOM, &over->win);
  over->dst[0] = 0;
  drawBox(&over->win, 28, 18);
  while (aptMainLoop())
  {
    printEditable(over);
    if (over->dataIndex >= 0)
      ret = inputDataSelect(over);
    else
      ret = inputOverlay(over);
    if (ret > 0)
    {
      drawBox(&over->win, 28, 18);
      if (over->data)
	doSearch(over);

    }
    if (ret < 0)
      break;
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }
  consoleClear();
  consoleSelect(old);
}

char 	*getstr(char *dst, int maxlen)
{
  struct s_overlay	over;

  if (!dst || !maxlen)
    return NULL;

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz0123456789");
  strcpy(over.title, "Type something");
  over.dst = dst;
  over.maxlen = maxlen;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = NULL;
  over.foundIndex[0] = -1;
  memset(dst, 0, maxlen);
  launchOverlay(&over);
  return dst;
}

//Calls an overlay to select a pokemon
s16	overlayGetpkm()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz' ");
  strcpy(over.title, "Choose a pokemon");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.species[0];
  over.entrylen = 12;
  over.datacount = 722;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}

s16	overlayGetMove()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz- ");
  strcpy(over.title, "Choose a move");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.moves[0];
  over.entrylen = 17;
  over.datacount = 622;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}

s16	overlayGetAbility()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz ");
  strcpy(over.title, "Choose an ability");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.abilities[0];
  over.entrylen = 15;
  over.datacount = 192;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}

s16	overlayGetItems()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz ");
  strcpy(over.title, "Choose an item");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.items[0];
  over.entrylen = 17;
  over.datacount = 775;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}

s16	overlayGetBalls()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz ");
  strcpy(over.title, "Choose a Pokeball");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.balls[0];
  over.entrylen = 13;
  over.datacount = 26;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}

s16	overlayGetNature()
{
  struct s_overlay 	over;
  char			tmp[12];

  strcpy(over.list, "abcdefghijklmnopqrstuvwxyz");
  strcpy(over.title, "Choose a Nature");
  over.dst = tmp;
  over.maxlen = 12;
  over.offs = 0;
  over.index = 0;
  over.dataIndex = -1;
  over.data = pkData.natures[0];
  over.entrylen = 8;
  over.datacount = 25;
  over.foundIndex[0] = -1;
  memcpy(&over.win, consoleGetDefault(), sizeof(PrintConsole));
  doSearch(&over);
  launchOverlay(&over);
  if (over.dataIndex < 0)
    return -1;
  return over.foundIndex[over.dataIndex];
}
