#ifndef PKX_H
#define PKX_H

#include <3ds.h>

enum STAT
{
  HP,
  ATK,
  DEF,
  SPE,
  SPA,
  SPD
};

struct s_data
{
  u8 	pkmData[722][14];
  u8	abilities[192][15];
  u8 	moves[622][17];
  u8 	species[722][12];
  u8 	items[775][17];
} pkData;

struct s_pkxcalc
{
  u8	level;
  u16 	finalStats[6];
};

struct 	__attribute__((__packed__)) s_pkx
{
  u32 	encryptKey;	
  u16 	sanity;
  u16 	checksum;

  //Block A
  u16 	species; //0x8
  u16 	item;
  u16 	trainerID;
  u16 	trainerSecretID; //0xE
  u32 	expPoints; //12
  u8 	ability;
  u8 	abilityNum; //14	
  u16 	trainingBagHitsLeft;
  u32 	personalityID;
  u8 	nature;
  u8 	formFlags; //22
  u8 	effortValues[6]; //28
  u8 	contestStats[6]; //34
  u8 	markings;
  u8 	pokerus;
  u32 	superTrainingFlags; //40
  u8 	ribbonFlags[6]; //46
  u16 	unused1; //48
  u8 	contestMemoryRibbon; //49
  u8 	battleMemoryRibbon; //50
  u8 	superTrainingDistributionFlags; //51
  u8 	unused2[5]; //56

  //Block B
  u8 	nickname[26]; //26
  u16 	moves[4]; //34
  u8 	movePP[4]; //38
  u8 	movePPUps[4]; //42
  u16 	relearnMoves[4]; //50
  u8 	secretSuperTrainingFlag; //51
  u8 	unused3; //52
  u32 	individualValues; //56

  //Block C
  u8 	latestHandler[26]; //26
  u8 	handlerGender; //27
  u8 	currentHandler; //28
  u16 	geolocation[5]; //38
  u32	unused4; //42
  u8 	handlerFriendship;
  u8 	handlerAffection;
  u8 	handlerMemoryIntensity;
  u8 	handlerMemoryLine;
  u8  	handlerMemoryFeel;
  u8 	unused5; //48
  u16 	handlerTextVar; //50
  u32 	unused6; //54
  u8 	fullness; //55
  u8	enjoyment; //56

  //Block D
  u8 	trainerName[26]; //26
  u8 	trainerFriendship;
  u8 	trainerAffection;
  u8 	trainerMemoryIntensity;
  u8 	trainerMemoryLine; //30
  u16 	trainerTextVar;
  u8 	trainerMemoryFeel; //33
  u8 	eggDate[3];
  u8	metDate[3];
  u8 	unused7;
  u16 	eggLocation;
  u16 	metLocation;
  u8 	ballType;
  u8 	encounterLevel;
  u8 	encounterType;
  u8 	trainerGameID;
  u8 	countryID;
  u8 	regionID;
  u8 	region3DSID;
  u8 	languageID;
  u32 	unused8;
};

struct s_pkm
{
  struct s_pkx pkx;
  struct s_pkxcalc calc;
};

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

Result  decryptPokemon(u8 *enc, u8 *dec);
Result 	encryptPokemon(u8 *dec, u8 *enc);
u8 	getPkmIV(u32 individualValues, u8 stat);
void 	pokemonDataDump(u8 *dec);
void 	printPkmName(u8 *str);
u8 	getPkmLevel(u16 species, u32 exp);
s8 	setPkmLevel(struct s_pkm *, u8 level);
u16 	getPkmStat(u16 species, u8 IV, u8 EV, u8 nature, u8 level, u8 stat);
s8 	pkmRecalc(struct s_pkm *);

s32	loadPokemon(t_stinf *state, u16 slot, u8 *dest);
s32	savePokemon(t_stinf *state, u16 slot, u8 *src);
s32 	switchState(t_stinf *state, struct s_UIState newst);
s32 	startLoop(u8 *, u8, PrintConsole *, PrintConsole *);


#endif /* end of include guard: PKX_H */
