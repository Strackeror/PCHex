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

struct 	s_pkx
{
  u32 	encryptKey;	
  u16 	sanity;
  u16 	checksum;

  //Block A
  u16 	species;
  u16 	item;
  u16 	trainerID;
  u16 	trainerSecretID;
  u32 	expPoints;
  u8 	ability;
  u8 	abilityNum;	
  u16 	trainingBagHitsLeft;
  u16 	personalityID;
  u8 	nature;
  u8 	formFlags;
  u8 	effortValues[6];
  u8 	contestStats[6];
  u8 	markings;
  u8 	pokerus;
  u32 	superTrainingFlags;
  u8 	ribbonFlags[6];
  u16 	unused1;
  u8 	contestMemoryRibbon;
  u8 	battleMemoryRibbon;
  u8 	superTrainingDistributionFlags;
  u8 	unused2[5];

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
  u8 	fullness; //56
  u8	enjoyment;

  //Block D
  u8 	trainerName[26];
  u8 	trainerFriendship;
  u8 	trainerAffection;
  u8 	trainerMemoryIntensity;
  u8 	trainerMemoryLine;
  u16 	trainerTextVar;
  u8 	trainerMemoryFeel;
  u32 	eggDate;
  u32 	metDate;
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

Result decryptPokemon(u8 *enc, u8 *dec);
void 	pokemonDataDump(u8 *dec);

#endif /* end of include guard: PKX_H */
