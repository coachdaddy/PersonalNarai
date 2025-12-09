
#include "structs.h"
#include "limit.h"
#include "utils.h"	/* K(), M(), G() 사용을 위해, 251209 */

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
        {"the Apprentice of Magic", "the Apprentice of Magic", 1},  /* 1 */
        {"the Spell Student", "the Spell Student", K(1) + 300},
        {"the Scholar of Magic", "the Scholar of Magic", K(2) + 600},
        {"the Delver in Spells", "the Delveress in Spells", K(7) + 800},
        {"the Medium of Magic", "the Medium of Magic", K(13)},
        {"the Scribe of Magic", "the Scribess of Magic", K(20) + 800},
        {"the Seer", "the Seeress", K(26)},
        {"the Sage", "the Sage", K(36) + 400},
        {"the Illusionist", "the Illusionist", K(52)},
        {"the Abjurer", "the Abjuress", K(78)},     /* 10 */
        {"the Invoker", "the Invoker", K(117)},
        {"the Enchanter", "the Enchantress", K(182)},
        {"the Conjurer", "the Conjuress", K(260)},
        {"the Magician", "the Witch", K(364)},
        {"the Creator", "the Creator", K(520)},
        {"the Savant", "the Savant", K(780)},
        {"the Magus", "the Craftess", K(1040)},
        {"the Wizard", "the Wizard", M(1) + K(430)},
        {"the Warlock", "the War Witch", M(1) + K(820)},
        {"the Sorcerer", "the Sorceress", M(2) + K(340)}, /* 20 */
        {"the Man of Hecate", "the Lady of Hecate", M(2) + K(860)},
        {"the Apparantice Archmage", "the Apparantice Archmage", M(3) + K(640)},
        {"the Archmage of Fire", "the Archmage of Fire", M(4) + K(550)},
        {"the Archmage of Water", "the Archmage of Water", M(5) + K(850)},
        {"the Archmage of Earth", "the Archmage of Earth", M(7) + K(800)},
        {"the Archmage of Wind", "the Archmage of Wind", M(10) + K(400)},
        {"the Mage Guru", "the Mage Guru", M(14) + K(300)},
        {"the Apparantice MageLord", "the Apparantice MageLord", M(19) + K(500)},
        {"the MageLord of Fire", "the MageLord of Fire", M(26)},
        {"the MageLord of Water", "the MageLord of Water", M(39)},  /* 30 */
        {"the MageLord of Earth", "the MageLord of Earth", M(58) + K(500)},
        {"the MageLord of Wind", "the MageLord of Wind", M(78)},
        {"the MageLord of Moon", "the MageLord of Moon", M(104)},
        {"the MageLord of Sun", "the MageLord of Sun", M(130)},
        {"the MageLord of Galaxy", "the MageLord of Galaxy", M(156)},
        {"the MageLord of Universe", "the MageLord of Universe", M(182)},
        {"the MageLord of Metaphysics", "the MageLord of Metaphysics", M(208)},
        {"the Metaphysician", "the Metaphysicians", M(234)},
        {"the Master of MageLord", "the Master of MageLord", M(260)},
        {"the Dangun", "the Dangun", M(280)},    /* 40 */
        {"the Dangun", "the Dangun", M(305)},
        {"the Dangun", "the Dangun", M(330)},
        {"the Dangun", "the Dangun", M(355)},
        {"the Dangun", "the Dangun", M(380)},
        {"the Dangun", "the Dangun", M(405)},
        {"the Dangun", "the Dangun", M(430)},
        {"the Dangun", "the Dangun", M(455)},
        {"the Dangun", "the Dangun", M(480)},
        {"the Dangun", "the Dangun", M(505)},
        {"the Dangun", "the Dangun", M(530)},    /* 50 */
        {"the Dangun", "the Dangun", M(560)},
        {"the Dangun", "the Dangun", M(590)},
        {"the Dangun", "the Dangun", M(620)},
        {"the Dangun", "the Dangun", M(650)},
        {"the Dangun", "the Dangun", M(680)},
        {"the Dangun", "the Dangun", M(715)},
        {"the Dangun", "the Dangun", M(750)},
        {"the Dangun", "the Dangun", M(785)},
        {"the Dangun", "the Dangun", M(820)},
        {"the Dangun", "the Dangun", M(855)},    /* 60 */
        {"the Immortal Warlock", "the Immortal Enchantress", G(1) + M(500)},
        {"the Avatar of Magic", "the Empress of Magic", G(2)},
        {"the God of magic", "the Goddess of magic", G(3)},
        {"the Implementator", "the Implementress", G(4)}},
    {
        {"the Man", "the Woman", 0},
        {"the Believer", "the Believer", 1},    /* 1 */
        {"the Attendant", "the Attendant", K(1)},
        {"the Acolyte", "the Acolyte", K(2)},
        {"the Novice", "the Novice", K(6)},
        {"the Missionary", "the Missionary", K(10)},
        {"the Adept", "the Adept", K(16)},
        {"the Deacon", "the Deaconess", K(20)},
        {"the Vicar", "the Vicaress", K(28)},
        {"the Priest", "the Priestess", K(40)},
        {"the Minister", "the Lady Minister", K(60)},   /* 10 */
        {"the Canon", "the Canon", K(90)},
        {"the Levite", "the Levitess", K(140)},
        {"the Curate", "the Curess", K(200)},
        {"the Monk", "the Nunne", K(280)},
        {"the Healer", "the Healess", K(400)},
        {"the Chaplain", "the Chaplain", K(600)},
        {"the Expositor", "the Expositress", K(800)},
        {"the Bishop", "the Bishop", M(1) + K(100)},
        {"the Arch Bishop", "the Arch Lady of the Church", M(1) + K(400)},
        {"the Patriarch", "the Matriarch", M(1) + K(800)},    /* 20 */
        {"the Lesser Doctor", "the Lesser Doctor", M(2) + K(200)},
        {"the Doctor", "the Doctor", M(2) + K(800)},
        {"the Master Doctor", "the Master Doctor", M(3) + K(500)},
        {"the Healer of GOD", "the Healer of GOD", M(4) + K(500)},
        {"the Cleric", "the Cleric", M(6)},
        {"the Cleric for people", "the Cleric for people", M(8)},
        {"the Chief Doctor", "the Chief Doctor", M(11)},
        {"the Great Doctor", "the Great Doctor", M(15)},
        {"the Doctor in Wind", "the Doctor in Wind", M(20)},
        {"the Doctor in Fire", "the Doctor in Fire", M(30)},     /* 30 */
        {"the Doctor of earth", "the Doctor of earth", M(45)},
        {"the Doctor of Universe", "the Doctor of Universe", M(60)},
        {"the Famous Doctor", "the Famous Doctor", M(80)},
        {"the Prayer", "the Prayer", M(100)},
        {"the Healer", "the Healer", M(120)},
        {"the Healer of Lords", "the Healer of Lords", M(140)},
        {"the Healer of God", "the Healer of God", M(160)},
        {"the Great Prayer", "the Great Prayer", M(180)},
        {"the Prayer for Dangun", "the Prayer for Dangun", M(200)},
        {"the Dangun", "the Dangun", M(220)},    /* 40 */
        {"the Dangun", "the Dangun", M(245)},
        {"the Dangun", "the Dangun", M(270)},
        {"the Dangun", "the Dangun", M(295)},
        {"the Dangun", "the Dangun", M(320)},
        {"the Dangun", "the Dangun", M(345)},
        {"the Dangun", "the Dangun", M(370)},
        {"the Dangun", "the Dangun", M(395)},
        {"the Dangun", "the Dangun", M(420)},
        {"the Dangun", "the Dangun", M(445)},
        {"the Dangun", "the Dangun", M(470)},    /* 50 */
        {"the Dangun", "the Dangun", M(500)},
        {"the Dangun", "the Dangun", M(530)},
        {"the Dangun", "the Dangun", M(560)},
        {"the Dangun", "the Dangun", M(590)},
        {"the Dangun", "the Dangun", M(625)},
        {"the Dangun", "the Dangun", M(660)},
        {"the Dangun", "the Dangun", M(695)},
        {"the Dangun", "the Dangun", M(730)},
        {"the Dangun", "the Dangun", M(775)},
        {"the Dangun", "the Dangun", M(810)},    /* 60 */
        {"the Immortal Cardinal", "the Immortal Priestess", G(1) + M(500)},
        {"the Inquisitor", "the Inquisitress", G(2)},
        {"the god of good and evil", "the goddess of good and evil", G(3)},
        {"the Implementator", "the Implementress", G(4)}},

    {
        {"the Man", "the Woman", 0},
        {"the Pilferer", "the Pilferess", 1},   /* 1 */
        {"the Footpad", "the Footpad", K(1) + 600},
        {"the Filcher", "the Filcheress", K(3) + 200},
        {"the Pick-Pocket", "the Pick-Pocket", K(9) + 600},
        {"the Sneak", "the Sneak", K(16)},
        {"the Pincher", "the Pincheress", K(25) + 600},
        {"the Cut-Purse", "the Cut-Purse", K(32)},
        {"the Snatcher", "the Snatcheress", K(44) + 800},
        {"the Sharper", "the Sharpress", K(64)},
        {"the Rogue", "the Rogue", K(96)},  /* 10 */
        {"the Robber", "the Robber", K(144)},
        {"the Magsman", "the Magswoman", K(224)},
        {"the Highwayman", "the Highwaywoman", K(320)},
        {"the Burglar", "the Burglaress", K(448)},
        {"the Thief", "the Thief", K(640)},
        {"the Knifer", "the Knifer", K(960)},
        {"the Quick-Blade", "the Quick-Blade", M(1) + K(280)},
        {"the Killer", "the Murderess", M(1) + K(760)},
        {"the Brigand", "the Brigand", M(2) + K(240)},
        {"the Cut-Throat", "the Cut-Throat", M(2) + K(880)},  /* 20 */
        {"the Urchin Father", "the Urchin Mother", M(3) + K(300)},
        {"the Bully", "the Bully", M(4) + K(200)},
        {"the Snifer", "the Snifer", M(5) + K(250)},
        {"the famous Killer", "the Famous Killer", M(6) + K(700)},
        {"the Best Snifer", "the Best Snifer", M(9)},
        {"the Quick Knife", "the Quick Knife", M(12)},
        {"the Stealer", "the Stealer", M(16) + K(500)},
        {"the Stealer of breath", "the Stealer of Breath", M(22) + K(500)},
        {"the Good Thief", "the Good Thief", M(30)},
        {"the Lord of Thieves", "the Lady of Thieves", M(45)},   /* 30 */
        {"the King of Thieves", "the Queen of Thieves", M(67) + K(500)},
        {"the Urchin in world", "the Urchin in world", M(80)},
        {"the Shadow", "the Shadow", M(105)},
        {"the Thief of Fire", "the Thief of Fire", M(130)},
        {"the Thief of Wind", "the Thief of Wind", M(155)},
        {"the Thief of Earth", "the Thief of Earth", M(180)},
        {"the Thief of Universe", "the Thief of Universe", M(205)},
        {"the God-Father", "the God-Mother", M(230)},
        {"the Greatest Bully", "the Greatest Bully", M(255)},
        {"the Dangun", "the Dangun", M(280)},    /* 40 */
        {"the Dangun", "the Dangun", M(305)},
        {"the Dangun", "the Dangun", M(330)},
        {"the Dangun", "the Dangun", M(355)},
        {"the Dangun", "the Dangun", M(380)},
        {"the Dangun", "the Dangun", M(405)},
        {"the Dangun", "the Dangun", M(435)},
        {"the Dangun", "the Dangun", M(460)},
        {"the Dangun", "the Dangun", M(495)},
        {"the Dangun", "the Dangun", M(525)},
        {"the Dangun", "the Dangun", M(555)},    /* 50 */
        {"the Dangun", "the Dangun", M(585)},
        {"the Dangun", "the Dangun", M(615)},
        {"the Dangun", "the Dangun", M(645)},
        {"the Dangun", "the Dangun", M(675)},
        {"the Dangun", "the Dangun", M(705)},
        {"the Dangun", "the Dangun", M(740)},
        {"the Dangun", "the Dangun", M(775)},
        {"the Dangun", "the Dangun", M(810)},
        {"the Dangun", "the Dangun", M(845)},
        {"the Dangun", "the Dangun", M(880)},    /* 60 */
        {"the Immortal Assassin", "the Immortal Assassin", G(1) + M(500)},
        {"the Demi God of thieves", "the Demi Goddess of thieves", G(2)},
        {"the God of Thieves", "the Goddess of Thieves", G(3)},
        {"the Implementator", "the Implementress", G(4)}},

    {
        {"the Man", "the Woman", 0},
        {"the Swordpupil", "the Swordpupil", 1},    /* 1 */
        {"the Recruit", "the Recruit", K(2)},
        {"the Sentry", "the Sentress", K(4)},
        {"the Fighter", "the Fighter", K(12)},
        {"the Soldier", "the Soldier", K(20)},
        {"the Warrior", "the Warrior", K(32)},
        {"the Veteran", "the Veteran", K(40)},
        {"the Swordsman", "the Swordswoman", K(56)},
        {"the Fencer", "the Fenceress", K(80)},
        {"the Combatant", "the Combatess", K(120)}, /* 10 */
        {"the Hero", "the Heroine", K(180)},
        {"the Myrmidon", "the Myrmidon", K(280)},
        {"the Swashbuckler", "the Swashbuckleress", K(400)},
        {"the Mercenary", "the Mercenaress", K(560)},
        {"the Swordmaster", "the Swordmistress", K(800)},
        {"the Lieutenant", "the Lieutenant", M(1) + K(200)},
        {"the Champion", "the Lady Champion", M(1) + K(600)},
        {"the Dragoon", "the Lady Dragoon", M(2) + K(200)},
        {"the Cavalier", "the Cavalier", M(2) + K(800)},
        {"the Knight", "the Lady Knight", M(3) + K(600)}, /* 20 */
        {"the Knight of Fire", "the Knight of Fire", M(4) + K(400)},
        {"the Knight of Wind", "the Knight of Wind", M(5) + K(600)},
        {"the Knight of Ice", "the Knight of Ice", M(7)},
        {"the Knight of Water", "the Knight of Water", M(9)},
        {"the Powerful Knight", "the Powerful Knight", M(12)},
        {"the Lord of Wind", "the Lady of Wind", M(16)},
        {"the Lord of Fire", "the Lady of Fire", M(22)},
        {"the Lord of Water", "the Lady of Water", M(30)},
        {"the Lord of Earth", "the Lord of Earth", M(40)},
        {"the Bodyguard", "the Bodyguard", M(60)},   /* 30 */
        {"the Famous knight", "the Famous Knight", M(85)},
        {"the Best Knight", "the Best Knight", M(110)},
        {"the Lord of Knight", "The Lady of Knight", M(135)},
        {"the King of Knight", "the King of Knight", M(160)},
        {"the Adventurer", "the Adventurer", M(185)},
        {"the Best kid", "the Best lady", M(210)},
        {"the Paladin", "the Paladin", M(235)},
        {"the Best Swordman", "the Best Swordwoman", M(260)},
        {"the Adventurer for Dangun", "the Adventurer for Dangun", M(290)},
        {"the Dangun", "the Dangun", M(325)},    /* 40 */
        {"the Dangun", "the Dangun", M(355)},
        {"the Dangun", "the Dangun", M(385)},
        {"the Dangun", "the Dangun", M(415)},
        {"the Dangun", "the Dangun", M(445)},
        {"the Dangun", "the Dangun", M(475)},
        {"the Dangun", "the Dangun", M(505)},
        {"the Dangun", "the Dangun", M(535)},
        {"the Dangun", "the Dangun", M(565)},
        {"the Dangun", "the Dangun", M(595)},
        {"the Dangun", "the Dangun", M(625)},    /* 50 */
        {"the Dangun", "the Dangun", M(660)},
        {"the Dangun", "the Dangun", M(695)},
        {"the Dangun", "the Dangun", M(730)},
        {"the Dangun", "the Dangun", M(765)},
        {"the Dangun", "the Dangun", M(800)},
        {"the Dangun", "the Dangun", M(840)},
        {"the Dangun", "the Dangun", M(880)},
        {"the Dangun", "the Dangun", M(920)},
        {"the Dangun", "the Dangun", M(960)},
        {"the Dangun", "the Dangun", M(1000)},   /* 60 */
        {"the Immortal Warlord", "the Immortal Lady of War", G(1) + M(500)},
        {"the Extirpator", "the Queen of Destruction", G(2)},
        {"the God of war", "the Goddess of war", G(3)},
        {"the Implementator", "the Implementress", G(4)}}
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
