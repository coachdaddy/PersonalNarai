
#include "structs.h"
#include "limit.h"

char *spell_wear_off_msg[] =
{
	"RESERVED DB.C",
	"You feel less protected.",	/* 1 */
	"!Teleport!",
	"You feel less righteous.",
	"You feel a cloak of blindness dissolve.",
	"!Burning Hands!",
	"!Call Lightning",
	"You feel more self-confident.",
	"!Chill Touch!",
	"!Reanimate!",
	"!Color Spray!",
	"!Relocate!",		/*11 */
	"!Create Food!",
	"!Create Water!",
	"!Cure Blind!",
	"!Cure Critic!",
	"!Cure Light!",
	"You feel better.",
	"You sense the red in your vision disappear.",
	"The detect invisible wears off.",
	"The detect magic wears off.",
	"!PREACH!",		/* 21 */
	"!Dispel Evil!",
	"!Earthquake!",
	"!Enchant Weapon!",
	"!Energy Drain!",
	"!Fireball!",
	"!Harm!",
	"!Heal",
	"You feel yourself exposed.",
	"!Lightning Bolt!",
	"!Locate object!",	/* 31 */
	"!Magic Missile!",
	"You feel less sick.",
	"You feel less protected.",
	"!Remove Curse!",
	"The white aura around your body fades.",
	"!Shocking Grasp!",
	"You feel les tired.",
	"You feel weaker.",
	"!Summon!",
	"!Ventriloquate!",	/* 41 */
	"!Word of Recall!",
	"!Remove Poison!",
	"You feel less aware of your surroundings.",
	"!Sunburst!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",		/* 51 */
	"!UNUSED!",
	"!Identify!",
	"!UNUSED!",
	"",			/* NO MESSAGE FOR SNEAK */
	"!Hide!",
	"!Steal!",
	"!Backstab!",
	"!Pick Lock!",
	"!Kick!",
	"!Bash!",		/* 61 */
	"!Rescue!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"!UNUSED!",
	"You feel less brave.",	/* 71 */
	"!energy flow!",
	"!mana boost!",
	"!vitalize!",
	"!full fire!",
	"!throw!",
	"!fire storm!",
	"You feel less shirink.",
	"!full heal!",
	"!trick!",
	"You feel things return to normal.",	/* 81 */
	"You feel things return to normal.",
	"!far look!",
	"!all heal!",
	"!tornado!",
	"!light move!",
	"!parry!",
	"!flash!",
	"!multi kick!",
	"!UNUSED90!",
	"!UNUSED91!",		/* 91 */
	"Your eyes are less sensitive now.",	/* 92 ; infra */
	"!UNUSED93!",
	"!UNUSED94!",
	"!UNUSED95!",
	"!UNUSED96!",
	"!UNUSED97!",		/* 97 ; mirror image */
	"Your illusion color fades.",	/* 98 ; mirror image */
	"!UNUSED99!",
	"!UNUSED100!",		/* 100 */
	"!UNUSED101!",
	"!UNUSED102!",
	"!UNUSED103!",
	"You are normalized again!",	/* 104: love */
	"!UNUSED105!",
	"You feel less angelic!",	/* reraise 106  */
	"!UNUSED107!",
	"!UNUSED108!",
	"!UNUSED109!",
	"You have been killed!!",
	"!UNUSED110!",
	"Your blocking clouds got fade!!",
	"!UNUSED112!",
	"Your shadow fades!",
	"!UNUSED114!",
	"!UNUSED115!",
	"Your holy shield fades!",
	"Your reflecting shield fades!",
	"!UNUSED118",
	"Your reflecting shield fades!",
	"You can speak now!!",
	"!jjup!",
	"!jjup!",
	"!jjup!",
	"\n",
};

int rev_dir[] =
{
	2, 3, 0, 1, 5, 4
};

int movement_loss[] =
{
	1,			/* Inside     */
	2,			/* City       */
	2,			/* Field      */
	3,			/* Forest     */
	4,			/* Hills      */
	6,			/* Mountains  */
	4,			/* Swimming   */
	1,			/* Unswimable */
	1			/* Sky */
};

char *dirs[] =
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down",
	"\n"
};

char *weekdays[7] =
{
	"the Day of the Moon",
	"the Day of the Bull",
	"the Day of the Deception",
	"the Day of Thunder",
	"the Day of Freedom",
	"the day of the Great Gods",
	"the Day of the Sun"};

char *month_name[17] =
{
	"Month of Winter",	/* 0 */
	"Month of the Winter Wolf",
	"Month of the Frost Giant",
	"Month of the Old Forces",
	"Month of the Grand Struggle",
	"Month of the Spring",
	"Month of Nature",
	"Month of Futility",
	"Month of the Dragon",
	"Month of the Sun",
	"Month of the Heat",
	"Month of the Battle",
	"Month of the Dark Shades",
	"Month of the Shadows",
	"Month of the Long Shadows",
	"Month of the Ancient Darkness",
	"Month of the Great Evil"
};

int sharp[] =
{
	0,
	0,
	0,
	1,			/* Slashing */
	0,
	0,
	0,
	0,			/* Bludgeon */
	0,
	0,
	0,
	0};			/* Pierce   */

char *where[] =
{
	"<used as light>      ",
	"<worn on finger>     ",
	"<worn on finger>     ",
	"<worn around neck>   ",
	"<worn around neck>   ",
	"<worn on body>       ",
	"<worn on head>       ",
	"<worn on legs>       ",
	"<worn on feet>       ",
	"<worn on hands>      ",
	"<worn on arms>       ",
	"<worn as shield>     ",
	"<worn about body>    ",
	"<worn about waist>   ",
	"<worn around wrist>  ",
	"<worn around wrist>  ",
	"<wielded>            ",
	"<held>               ",
	"<worn around knee>   ",
	"<worn around knee>   ",
	"<worn about legs>    ",
	"<worn on back>       ",
};

char *drinks[] =
{
	"water",
	"beer",
	"wine",
	"ale",
	"dark ale",
	"whisky",
	"lemonade",
	"firebreather",
	"local speciality",
	"slime mold juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt water",
	"sundew",
	"nectar",
	"golden nectar",
	"makoli",
	"condition"
};

char *drinknames[] =
{
	"water",
	"beer",
	"wine",
	"ale",
	"ale",
	"whisky",
	"lemonade",
	"firebreather",
	"local",
	"juice",
	"milk",
	"tea",
	"coffee",
	"blood",
	"salt",
	"sundew",
	"nectar",
	"golden nectar",
	"makoli",
	"condition"
};

int drink_aff[][3] =
{
	{0, 1, 10},		/* Water    */
	{2, 2, 5},		/* beer     */
	{1, 3, 4},		/* wine     */
	{3, 2, 5},		/* ale      */
	{3, 2, 5},		/* ale      */
	{6, 1, 4},		/* Whiskey  */
	{0, 1, 8},		/* lemonade */
	{10, 0, 0},		/* firebr   */
	{3, 3, 3},		/* local    */
	{0, 4, -8},		/* juice    */
	{0, 3, 6},		/* milk     */
	{0, 1, 6},		/* tea      */
	{0, 1, 6},		/* coffee   */
	{0, 2, -1},		/* blood    */
	{0, 1, -2},		/* salt     */
	{0, 3, 8},		/* sundew   */
	{5, 5, 5},		/* nectar   */
	{8, 8, 8},		/* golden nectar */
	{5, 1, 2},		/* makoli 막걸리 */
	{-4, 1, 1}		/* condition */
};

char *color_liquid[] =
{
	"clear",
	"brown",
	"clear",
	"brown",
	"dark",
	"golden",
	"red",
	"green",
	"clear",
	"light green",
	"white",
	"brown",
	"black",
	"red",
	"clear",
	"white",
	"yellow",
	"golden",
	"white"
};

char *fullness[] =
{
	"less than half ",
	"about half ",
	"more than half ",
	""
};

struct title_type titles[4][IMO + 4] =
{
	{
		{"the Man", "the Woman", 0},
		{"the Apprentice of Magic", "the Apprentice of Magic", 1},	/* 1 */
		{"the Spell Student", "the Spell Student", 1300},
		{"the Scholar of Magic", "the Scholar of Magic", 2600},
		{"the Delver in Spells", "the Delveress in Spells", 7800},
		{"the Medium of Magic", "the Medium of Magic", 13000},
		{"the Scribe of Magic", "the Scribess of Magic", 20800},
		{"the Seer", "the Seeress", 26000},
		{"the Sage", "the Sage", 36400},
		{"the Illusionist", "the Illusionist", 52000},
		{"the Abjurer", "the Abjuress", 78000},		/* 10 */
		{"the Invoker", "the Invoker", 117000},
		{"the Enchanter", "the Enchantress", 182000},
		{"the Conjurer", "the Conjuress", 260000},
		{"the Magician", "the Witch", 364000},
		{"the Creator", "the Creator", 520000},
		{"the Savant", "the Savant", 780000},
		{"the Magus", "the Craftess", 1040000},
		{"the Wizard", "the Wizard", 1430000},
		{"the Warlock", "the War Witch", 1820000},
		{"the Sorcerer", "the Sorceress", 2340000},	/* 20 */
		{"the Man of Hecate", "the Lady of Hecate", 2860000},
		{"the Apparantice Archmage", "the Apparantice Archmage", 3640000},
		{"the Archmage of Fire", "the Archmage of Fire", 4550000},
		{"the Archmage of Water", "the Archmage of Water", 5850000},
		{"the Archmage of Earth", "the Archmage of Earth", 7800000},
		{"the Archmage of Wind", "the Archmage of Wind", 10400000},
		{"the Mage Guru", "the Mage Guru", 14300000},
		{"the Apparantice MageLord", "the Apparantice MageLord", 19500000},
		{"the MageLord of Fire", "the MageLord of Fire", 26000000},
		{"the MageLord of Water", "the MageLord of Water", 39000000},	/* 30 */
		{"the MageLord of Earth", "the MageLord of Earth", 58500000},
		{"the MageLord of Wind", "the MageLord of Wind", 78000000},
		{"the MageLord of Moon", "the MageLord of Moon", 104000000},
		{"the MageLord of Sun", "the MageLord of Sun", 130000000},
		{"the MageLord of Galaxy", "the MageLord of Galaxy", 156000000},
		{"the MageLord of Universe", "the MageLord of Universe", 182000000},
		{"the MageLord of Metaphysics", "the MageLord of Metaphysics", 208000000},
		{"the Metaphysician", "the Metaphysicians", 234000000},
		{"the Master of MageLord", "the Master of MageLord", 260000000},
		{"the Dangun", "the Dangun", 280000000},	/* 40 */
		{"the Dangun", "the Dangun", 305000000},
		{"the Dangun", "the Dangun", 330000000},
		{"the Dangun", "the Dangun", 355000000},
		{"the Dangun", "the Dangun", 380000000},
		{"the Dangun", "the Dangun", 405000000},
		{"the Dangun", "the Dangun", 430000000},
		{"the Dangun", "the Dangun", 455000000},
		{"the Dangun", "the Dangun", 480000000},
		{"the Dangun", "the Dangun", 505000000},
		{"the Dangun", "the Dangun", 530000000},	/* 50 */
		{"the Dangun", "the Dangun", 560000000},
		{"the Dangun", "the Dangun", 590000000},
		{"the Dangun", "the Dangun", 620000000},
		{"the Dangun", "the Dangun", 650000000},
		{"the Dangun", "the Dangun", 680000000},
		{"the Dangun", "the Dangun", 715000000},
		{"the Dangun", "the Dangun", 750000000},
		{"the Dangun", "the Dangun", 785000000},
		{"the Dangun", "the Dangun", 820000000},
		{"the Dangun", "the Dangun", 855000000},	/* 60 */
		{"the Immortal Warlock", "the Immortal Enchantress", 1500000000},
		{"the Avatar of Magic", "the Empress of Magic", 2000000000},
		{"the God of magic", "the Goddess of magic", 3000000000},
		{"the Implementator", "the Implementress", 4000000000}},
	{
		{"the Man", "the Woman", 0},
		{"the Believer", "the Believer", 1},	/* 1 */
		{"the Attendant", "the Attendant", 1000},
		{"the Acolyte", "the Acolyte", 2000},
		{"the Novice", "the Novice", 6000},
		{"the Missionary", "the Missionary", 10000},
		{"the Adept", "the Adept", 16000},
		{"the Deacon", "the Deaconess", 20000},
		{"the Vicar", "the Vicaress", 28000},
		{"the Priest", "the Priestess", 40000},
		{"the Minister", "the Lady Minister", 60000},	/* 10 */
		{"the Canon", "the Canon", 90000},
		{"the Levite", "the Levitess", 140000},
		{"the Curate", "the Curess", 200000},
		{"the Monk", "the Nunne", 280000},
		{"the Healer", "the Healess", 400000},
		{"the Chaplain", "the Chaplain", 600000},
		{"the Expositor", "the Expositress", 800000},
		{"the Bishop", "the Bishop", 1100000},
		{"the Arch Bishop", "the Arch Lady of the Church", 1400000},
		{"the Patriarch", "the Matriarch", 1800000},	/* 20 */
		{"the Lesser Doctor", "the Lesser Doctor", 2200000},
		{"the Doctor", "the Doctor", 2800000},
		{"the Master Doctor", "the Master Doctor", 3500000},
		{"the Healer of GOD", "the Healer of GOD", 4500000},
		{"the Cleric", "the Cleric", 6000000},
		{"the Cleric for people", "the Cleric for people", 8000000},
		{"the Chief Doctor", "the Chief Doctor", 11000000},
		{"the Great Doctor", "the Great Doctor", 15000000},
		{"the Doctor in Wind", "the Doctor in Wind", 20000000},
		{"the Doctor in Fire", "the Doctor in Fire", 30000000},		/* 30 */
		{"the Doctor of earth", "the Doctor of earth", 45000000},
		{"the Doctor of Universe", "the Doctor of Universe", 60000000},
		{"the Famous Doctor", "the Famous Doctor", 80000000},
		{"the Prayer", "the Prayer", 100000000},
		{"the Healer", "the Healer", 120000000},
		{"the Healer of Lords", "the Healer of Lords", 140000000},
		{"the Healer of God", "the Healer of God", 160000000},
		{"the Great Prayer", "the Great Prayer", 180000000},
		{"the Prayer for Dangun", "the Prayer for Dangun", 200000000},
		{"the Dangun", "the Dangun", 220000000},	/* 40 */
		{"the Dangun", "the Dangun", 245000000},
		{"the Dangun", "the Dangun", 270000000},
		{"the Dangun", "the Dangun", 295000000},
		{"the Dangun", "the Dangun", 320000000},
		{"the Dangun", "the Dangun", 345000000},
		{"the Dangun", "the Dangun", 370000000},
		{"the Dangun", "the Dangun", 395000000},
		{"the Dangun", "the Dangun", 420000000},
		{"the Dangun", "the Dangun", 445000000},
		{"the Dangun", "the Dangun", 470000000},	/* 50 */
		{"the Dangun", "the Dangun", 500000000},
		{"the Dangun", "the Dangun", 530000000},
		{"the Dangun", "the Dangun", 560000000},
		{"the Dangun", "the Dangun", 590000000},
		{"the Dangun", "the Dangun", 625000000},
		{"the Dangun", "the Dangun", 660000000},
		{"the Dangun", "the Dangun", 695000000},
		{"the Dangun", "the Dangun", 730000000},
		{"the Dangun", "the Dangun", 775000000},
		{"the Dangun", "the Dangun", 810000000},	/* 60 */
		{"the Immortal Cardinal", "the Immortal Priestess", 1500000000},
		{"the Inquisitor", "the Inquisitress", 2000000000},
		{"the god of good and evil", "the goddess of good and evil", 3000000000},
		{"the Implementator", "the Implementress", 4000000000}},

	{
		{"the Man", "the Woman", 0},
		{"the Pilferer", "the Pilferess", 1},	/* 1 */
		{"the Footpad", "the Footpad", 1600},
		{"the Filcher", "the Filcheress", 3200},
		{"the Pick-Pocket", "the Pick-Pocket", 9600},
		{"the Sneak", "the Sneak", 16000},
		{"the Pincher", "the Pincheress", 25600},
		{"the Cut-Purse", "the Cut-Purse", 32000},
		{"the Snatcher", "the Snatcheress", 44800},
		{"the Sharper", "the Sharpress", 64000},
		{"the Rogue", "the Rogue", 96000},	/* 10 */
		{"the Robber", "the Robber", 144000},
		{"the Magsman", "the Magswoman", 224000},
		{"the Highwayman", "the Highwaywoman", 320000},
		{"the Burglar", "the Burglaress", 448000},
		{"the Thief", "the Thief", 640000},
		{"the Knifer", "the Knifer", 960000},
		{"the Quick-Blade", "the Quick-Blade", 1280000},
		{"the Killer", "the Murderess", 1760000},
		{"the Brigand", "the Brigand", 2240000},
		{"the Cut-Throat", "the Cut-Throat", 2880000},	/* 20 */
		{"the Urchin Father", "the Urchin Mother", 3300000},
		{"the Bully", "the Bully", 4200000},
		{"the Snifer", "the Snifer", 5250000},
		{"the famous Killer", "the Famous Killer", 6700000},
		{"the Best Snifer", "the Best Snifer", 9000000},
		{"the Quick Knife", "the Quick Knife", 12000000},
		{"the Stealer", "the Stealer", 16500000},
		{"the Stealer of breath", "the Stealer of Breath", 22500000},
		{"the Good Thief", "the Good Thief", 30000000},
		{"the Lord of Thieves", "the Lady of Thieves", 45000000},	/* 30 */
		{"the King of Thieves", "the Queen of Thieves", 67500000},
		{"the Urchin in world", "the Urchin in world", 80000000},
		{"the Shadow", "the Shadow", 105000000},
		{"the Thief of Fire", "the Thief of Fire", 130000000},
		{"the Thief of Wind", "the Thief of Wind", 155000000},
		{"the Thief of Earth", "the Thief of Earth", 180000000},
		{"the Thief of Universe", "the Thief of Universe", 205000000},
		{"the God-Father", "the God-Mother", 230000000},
		{"the Greatest Bully", "the Greatest Bully", 255000000},
		{"the Dangun", "the Dangun", 280000000},	/* 40 */
		{"the Dangun", "the Dangun", 305000000},
		{"the Dangun", "the Dangun", 330000000},
		{"the Dangun", "the Dangun", 355000000},
		{"the Dangun", "the Dangun", 380000000},
		{"the Dangun", "the Dangun", 405000000},
		{"the Dangun", "the Dangun", 435000000},
		{"the Dangun", "the Dangun", 460000000},
		{"the Dangun", "the Dangun", 495000000},
		{"the Dangun", "the Dangun", 525000000},
		{"the Dangun", "the Dangun", 555000000},	/* 50 */
		{"the Dangun", "the Dangun", 585000000},
		{"the Dangun", "the Dangun", 615000000},
		{"the Dangun", "the Dangun", 645000000},
		{"the Dangun", "the Dangun", 675000000},
		{"the Dangun", "the Dangun", 705000000},
		{"the Dangun", "the Dangun", 740000000},
		{"the Dangun", "the Dangun", 775000000},
		{"the Dangun", "the Dangun", 810000000},
		{"the Dangun", "the Dangun", 845000000},
		{"the Dangun", "the Dangun", 880000000},	/* 60 */
		{"the Immortal Assassin", "the Immortal Assassin", 1500000000},
		{"the Demi God of thieves", "the Demi Goddess of thieves", 2000000000},
		{"the God of Thieves", "the Goddess of Thieves", 3000000000},
		{"the Implementator", "the Implementress", 4000000000}},

	{
		{"the Man", "the Woman", 0},
		{"the Swordpupil", "the Swordpupil", 1},	/* 1 */
		{"the Recruit", "the Recruit", 2000},
		{"the Sentry", "the Sentress", 4000},
		{"the Fighter", "the Fighter", 12000},
		{"the Soldier", "the Soldier", 20000},
		{"the Warrior", "the Warrior", 32000},
		{"the Veteran", "the Veteran", 40000},
		{"the Swordsman", "the Swordswoman", 56000},
		{"the Fencer", "the Fenceress", 80000},
		{"the Combatant", "the Combatess", 120000},	/* 10 */
		{"the Hero", "the Heroine", 180000},
		{"the Myrmidon", "the Myrmidon", 280000},
		{"the Swashbuckler", "the Swashbuckleress", 400000},
		{"the Mercenary", "the Mercenaress", 560000},
		{"the Swordmaster", "the Swordmistress", 800000},
		{"the Lieutenant", "the Lieutenant", 1200000},
		{"the Champion", "the Lady Champion", 1600000},
		{"the Dragoon", "the Lady Dragoon", 2200000},
		{"the Cavalier", "the Cavalier", 2800000},
		{"the Knight", "the Lady Knight", 3600000},	/* 20 */
		{"the Knight of Fire", "the Knight of Fire", 4400000},
		{"the Knight of Wind", "the Knight of Wind", 5600000},
		{"the Knight of Ice", "the Knight of Ice", 7000000},
		{"the Knight of Water", "the Knight of Water", 9000000},
		{"the Powerful Knight", "the Powerful Knight", 12000000},
		{"the Lord of Wind", "the Lady of Wind", 16000000},
		{"the Lord of Fire", "the Lady of Fire", 22000000},
		{"the Lord of Water", "the Lady of Water", 30000000},
		{"the Lord of Earth", "the Lord of Earth", 40000000},
		{"the Bodyguard", "the Bodyguard", 60000000},	/* 30 */
		{"the Famous knight", "the Famous Knight", 85000000},
		{"the Best Knight", "the Best Knight", 110000000},
		{"the Lord of Knight", "The Lady of Knight", 135000000},
		{"The King of Knight", "the King of Knight", 160000000},
		{"the Adventurer", "the Adventurer", 185000000},
		{"the Best kid", "the Best lady", 210000000},
		{"the Paladin", "the Paladin", 235000000},
		{"the Best Swordman", "the Best Swordwoman", 260000000},
		{"the Adventurer for Dangun", "the Adventurer for Dangun", 290000000},
		{"the Dangun", "the Dangun", 325000000},	/* 40 */
		{"the Dangun", "the Dangun", 355000000},
		{"the Dangun", "the Dangun", 385000000},
		{"the Dangun", "the Dangun", 415000000},
		{"the Dangun", "the Dangun", 445000000},
		{"the Dangun", "the Dangun", 475000000},
		{"the Dangun", "the Dangun", 505000000},
		{"the Dangun", "the Dangun", 535000000},
		{"the Dangun", "the Dangun", 565000000},
		{"the Dangun", "the Dangun", 595000000},
		{"the Dangun", "the Dangun", 625000000},	/* 50 */
		{"the Dangun", "the Dangun", 660000000},
		{"the Dangun", "the Dangun", 695000000},
		{"the Dangun", "the Dangun", 730000000},
		{"the Dangun", "the Dangun", 765000000},
		{"the Dangun", "the Dangun", 800000000},
		{"the Dangun", "the Dangun", 840000000},
		{"the Dangun", "the Dangun", 880000000},
		{"the Dangun", "the Dangun", 920000000},
		{"the Dangun", "the Dangun", 960000000},
		{"the Dangun", "the Dangun", 1000000000},	/* 60 */
		{"the Immortal Warlord", "the Immortal Lady of War", 1500000000},
		{"the Extirpator", "the Queen of Destruction", 2000000000},
		{"the God of war", "the Goddess of war", 3000000000},
		{"the Implementator", "the Implementress", 4000000000}}
};

char *item_types[] =
{
	"UNDEFINED",
	"LIGHT",
	"SCROLL",
	"WAND",
	"STAFF",
	"WEAPON",
	"FIRE WEAPON",
	"MISSILE",
	"TREASURE",
	"ARMOR",
	"POTION",
	"WORN",
	"OTHER",
	"TRASH",
	"TRAP",
	"CONTAINER",
	"NOTE",
	"LIQUID CONTAINER",
	"KEY",
	"FOOD",
	"MONEY",
	"PEN",
	"BOAT",
	"CIGA",
	"DRUG",
	"\n"
};

char *wear_bits[] =
{
	"TAKE",
	"FINGER",
	"NECK",
	"BODY",
	"HEAD",
	"LEGS",
	"FEET",
	"HANDS",
	"ARMS",
	"SHIELD",
	"ABOUT",
	"WAISTE",
	"WRIST",
	"WIELD",
	"HOLD",
	"THROW",
	"KNEE",
	"ABOUTLEGS",
	"LIGHT-SOURCE",
	"BACKPACK",
	"\n"
};

char *extra_bits[] =
{
	"GLOW",
	"HUM",
	"DARK",
	"LOCK",
	"EVIL",
	"INVISIBLE",
	"MAGIC",
	"NODROP",
	"BLESS",
	"ANTI-GOOD",
	"ANTI-EVIL",
	"ANTI-NEUTRAL",
	"EQ_LVL_LIMIT",
	"ANTI-MAGE",
	"ANTI-CLERIC",
	"ANTI-THIEF",
	"ANTI-WARRIOR",
	"NORENT",
	"ANTI-POLICE",
	"ANTI-OUTLAW",
	"ANTI-ASSASSIN",
	"NOCOPY",
	"\n"
};

char *room_bits[] =
{
	"DARK",
	"NOSUMMON",
	"NO_MOB",
	"INDOORS",
	"LAWFUL",
	"NEUTRAL",
	"CHAOTIC",
	"NO_MAGIC",
	"TUNNEL",
	"PRIVATE",
	"OFF_LIMITS",
	"RENT",
	"NORELOCATE",
	"EVERYZONE",
	"JAIL",
	"RESTROOM",
	"TEST",
	"TEST2",
	"TEST3",
	"TEST4",
	"\n"
};

char *exit_bits[] =
{
	"IS-DOOR",
	"CLOSED",
	"LOCKED",
	"KK",
	"LL",
	"PICKPROOF",
	"NOPHASE",
	"\n"
};

char *sector_types[] =
{
	"Inside",
	"City",
	"Field",
	"Forest",
	"Hills",
	"Mountains",
	"Water Swim",
	"Water NoSwim",
	"Sky",
	"\n"
};

char *equipment_types[] =
{
	"Special",
	"Worn on right finger",
	"Worn on left finger",
	"First worn around Neck",
	"Second worn around Neck",
	"Worn on body",
	"Worn on head",
	"Worn on legs",
	"Worn about legs",
	"Worn on right knee",
	"Worn on left knee",
	"Worn on feet",
	"Worn on hands",
	"Worn on arms",
	"Worn as shield",
	"Worn about body",
	"Worn around waiste",
	"Worn around right wrist",
	"Worn around left wrist",
	"Wielded",
	"Held",
	"Worn on back",
	"\n"
};

char *affected_bits[] =
{"BLIND",
 "INVISIBLE",
 "DETECT-EVIL",
 "DETECT-INVISIBLE",
 "INFRAVISION",
 "SENSE-LIFE",
 "DEAF",
 "SANCTUARY",
 "GROUP",
 "DUMB",
 "CURSE",
 "MIRROR-IMAGE",
 "POISON",
 "PROTECT-EVIL",
 "REFLECT-DAMAGE",
 "HOLY-SHIELD",
 "SPELL-BLOCK",
 "SLEEP",
 "SHADOW-FIGURE",
 "SNEAK",
 "HIDE",
 "DEATH",
 "CHARM",
 "FOLLOW",
 "HASTE",
 "IMPROVED_HASTE",
 "LOVE",
 "RERAISE",
 "\n"
};

char *apply_types[] =
{
	"NONE",
	"STR",
	"DEX",
	"INT",
	"WIS",
	"CON",
	"SEX",
	"CLASS",
	"LEVEL",
	"AGE",
	"CHAR_WEIGHT",
	"CHAR_HEIGHT",
	"MANA",
	"HIT",
	"MOVE",
	"GOLD",
	"EXP",
	"ARMOR",
	"HITROLL",
	"DAMROLL",
	"SAVING_PARA",
	"SAVING_HIT_SKILL",
	"SAVING_PETRI",
	"SAVING_BREATH",
	"SAVING_SPELL",
	"REGENERATION",
	"INVISIBLE",
	"\n"
};

char *pc_class_types[] =
{
	"UNDEFINED",
	"Magic User",
	"Cleric",
	"Thief",
	"Warrior",
	"\n"
};

char *npc_class_types[] =
{
	"Normal",
	"Undead",
	"Humanoid",
	"Animal",
	"Dragon",
	"Giant",
	"Demon",
	"Insect",
	"\n"
};

char *action_bits[] =
{
	"SPEC",
	"SENTINEL",
	"SCAVENGER",
	"ISNPC",
	"NICE-THIEF",
	"AGGRESSIVE",
	"STAY-ZONE",
	"WIMPY",
	"FIGHTER",
	"MAGE",
	"CLERIC",
	"THIEF",
	"PALADIN",
	"DRAGON",
	"SPITTER",
	"SHOOTER",
	"GUARD",
	"SUPERGUARD",
	"GIANT",
	"HELPER",
	"RESCUER",
	"HELPER",
	"FINISHER",
	"\n"
};

char *player_bits[] =
{
	"BRIEF",
	"NOSHOUT",
	"COMPACT",
	"DONTSET",
	"NOTELL",
	"BANISHED",
	"CRIMINAL",
	"WIZINVIS",
	"EARMUFFS",
	"XYZZY",
	"AGGR",
	"KOREAN",
	"NOCHAT",
	"WIMPY",
	"DUMB-BY-WIZ",
	"AUTO-ASSIST",
	"SOLO",
	"\n"
};

char *position_types[] =
{
	"Dead",
	"Mortally wounded",
	"Incapacitated",
	"Stunned",
	"Sleeping",
	"Resting",
	"Sitting",
	"Fighting",
	"Standing",
	"\n"
};

char *connected_types[] =
{
	"Playing",
	"Get name",
	"Confirm name",
	"Read Password",
	"Get new password",
	"Confirm new password",
	"Get sex",
	"Read messages of today",
	"Read Menu",
	"Get extra description",
	"Get class",
	"\n"
};

/* [class], [level] (all) M, C, T, W*/
/* cyb const int thaco[4][IMO+4] = { */
/* probability of attack */
int thaco[4][IMO + 4] =
{
	{0,
	 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
	 0, 0, 0, 0},
	{0,
	 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
	 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
	 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 
	 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
	 0, 0, 0, 0},
	{0,
	 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
	 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
	 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
	 0, 0, 0, 0},
	{0,
	 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
	 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
	 0, 0, 0, 0}
};

/* [ch] strength apply (all) */
/* struct = { short tohit, todam, carry_w, wield_w } */
struct str_app_type str_app[31] =
{
	{-5, 0, 0, 0},		/* 0  */
	{-5, 1, 3, 1},		/* 1  */
	{-3, 1, 3, 2},
	{-3, 1, 10, 3},		/* 3  */
	{-2, 1, 25, 4},
	{-2, 1, 55, 5},		/* 5  */
	{-1, 2, 80, 6},
	{-1, 2, 90, 7},
	{0, 2, 100, 8},
	{0, 3, 100, 9},
	{0, 3, 115, 10},	/* 10  */
	{0, 4, 115, 11},
	{0, 5, 130, 12},
	{0, 6, 140, 13},
	{0, 7, 160, 14},
	{0, 8, 170, 15},	/* 15  */
	{0, 9, 195, 16},
	{1, 10, 220, 18},
	{1, 11, 255, 20},	/* 18  */
	{3, 20, 640, 40},
	{3, 21, 700, 40},	/* 20  */
	{4, 22, 810, 40},
	{4, 23, 970, 40},
	{5, 24, 1130, 40},
	{6, 25, 1440, 40},
	{7, 26, 1750, 40},	/* 25            */
	{1, 12, 280, 22},	/* 18/01-50      */
	{1, 13, 305, 24},	/* 18/51-75      */
	{2, 14, 330, 26},	/* 18/76-90      */
	{2, 15, 380, 28},	/* 18/91-99      */
	{3, 16, 480, 30},	/* 18/100   (30) */
};

/* [dex] skill apply (thieves only) */
/* struct = {short p_pocket, p_locks, traps, sneak, hide } */
struct dex_skill_type dex_app_skill[26] =
{
	{-99, -99, -90, -99, -60},	/* 0 */
	{-90, -90, -60, -90, -50},	/* 1 */
	{-80, -80, -40, -80, -45},
	{-70, -70, -30, -70, -40},
	{-60, -60, -30, -60, -35},
	{-50, -50, -20, -50, -30},	/* 5 */
	{-40, -40, -20, -40, -25},
	{-30, -30, -15, -30, -20},
	{-20, -20, -15, -20, -15},
	{-15, -10, -10, -20, -10},
	{-10, -5, -10, -15, -5},	/* 10 */
	{-5, 0, -5, -10, 0},
	{0, 0, 0, -5, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},	/* 15 */
	{0, 5, 0, 0, 0},
	{5, 10, 0, 5, 5},
	{10, 15, 5, 10, 10},
	{15, 20, 10, 15, 15},
	{15, 20, 10, 15, 15},	/* 20 */
	{20, 25, 10, 15, 20},
	{20, 25, 15, 20, 20},
	{25, 25, 15, 20, 20},
	{25, 30, 15, 25, 25},
	{25, 30, 15, 25, 25}
};

/* [level] backstab multiplyer (thieves only) */
byte backstab_mult[IMO + 4] =
{
	1, 
	2, 2, 3, 3, 3,			/* 5 */
	4, 4, 4, 4, 5,			/* 10 */
	5, 5, 5, 5, 6,			/* 15 */
	6, 6, 6, 6, 6,			/* 20 */
	7, 7, 7, 7,	7,			/* 25 */
	7, 7, 7, 8, 8,			/* 30 */
	8, 8, 8, 8, 8,			/* 35 */
	8, 9, 9, 9, 10,			/* 40 */
	12, 12, 12, 12, 12,		/* 45 */
	15, 15, 15, 15, 15,		/* 50 */
	18, 18, 18, 18, 18,		/* 55 */
	25, 25, 25, 25, 25,		/* 60 */
	50, 50, 50, 50
};

/* [dex] apply (all) */
/* struct = { short reaction, attack, defensive } */
/* attack : possibility of attack */
/* defensive : possibility of defensive */
struct dex_app_type dex_app[26] =
{
	{-7, 0, 0},		/* 0 */
	{-6, 1, 2},		/* 1 */
	{-4, 2, 4},
	{-3, 3, 6},
	{-2, 4, 8},
	{-1, 5, 10},		/* 5 */
	{0, 6, 12},
	{0, 7, 14},
	{0, 8, 16},
	{0, 9, 18},
	{0, 10, 20},		/* 10 */
	{0, 12, 22},
	{0, 14, 24},
	{0, 16, 26},
	{0, 18, 28},
	{0, 20, 30},		/* 15 */
	{1, 23, 33},
	{2, 26, 36},
	{2, 30, 40},		/* 18 */
	{3, 35, 45},
	{3, 40, 50},		/* 20 */
	{4, 45, 55},
	{4, 50, 60},
	{4, 55, 65},
	{5, 60, 70},
	{5, 65, 75}
};

/* [con] apply (all) */
/* struct = { short hitp, shock }  */
struct con_app_type con_app[26] =
{
	{-4, 20},		/* 0 */
	{-3, 25},		/* 1 */
	{-2, 30},
	{-2, 35},
	{-1, 40},
	{-1, 45},		/* 5 */
	{-1, 50},
	{0, 55},
	{1, 65},
	{2, 70},
	{3, 75},		/* 10 */
	{4, 78},
	{5, 80},
	{6, 85},
	{7, 88},
	{8, 90},		/* 15 */
	{9, 95},
	{10, 97},
	{10, 99},
	{10, 99},
	{10, 99},		/* 20 */
	{10, 99},
	{10, 99},
	{10, 99},
	{10, 99},
	{10, 100}		/* 25 */
};

/* [int] apply (all) */
struct int_app_type int_app[26] =
{
	{1},
	{2},
	{3},
	{4},
	{7},
	{8},
	{9},
	{10},
	{20},
	{28},
	{30},			/* 10 */
	{35},
	{38},
	{40},
	{41},
	{42},
	{46},
	{50},
	{54},
	{99},
	{99},			/* 20 */
	{99},
	{99},
	{99},
	{99},
	{99}
};

/* [wis] apply (all) struct = { byte bonus } */
struct wis_app_type wis_app[26] =
{
	{0},
	{1},
	{1},
	{1},
	{1},
	{1},
	{2},
	{2},
	{2},
	{3},
	{3},
	{3},
	{3},
	{3},
	{3},
	{4},
	{4},
	{4},
	{6},
	{8},
	{8},
	{8},
	{8},
	{8},
	{8},
	{8}
};
