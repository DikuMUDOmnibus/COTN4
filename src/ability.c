#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define CUR  5

/*
Toughness    	// resist, damage
Power 		// damage, DR
Skill    	// attacks, HR 
Prowess  	// AC, parry
Agility  	// attacks, dodge
*/

void gain_ability(CHAR_DATA * ch, int gain)
{
char buf[MAX_STRING_LENGTH];

if (IS_NPC(ch)) return;
if (gain < 0) return;

// Toughness
if (ch->ability_cur[CUR] == 0 && ch->ability[0] < 100)
{
ch->ability_exp[0] += gain;

if (ch->ability_exp[0] > (21000000 * ch->ability[0]))
{
ch->ability_exp[0] -= 21000000 * ch->ability[0];
ch->ability[0]++;
}
return;
}

// Power
if (ch->ability_cur[CUR] == 1 && ch->ability[1] < 100)
{
ch->ability_exp[1] += gain;

if (ch->ability_exp[1] > (21000000 * ch->ability[1]))
{
ch->ability_exp[1] -= 21000000 * ch->ability[1];
ch->ability[1]++;
}
return;
}

// Skill
if (ch->ability_cur[CUR] == 2 && ch->ability[2] < 100)
{
ch->ability_exp[2] += gain;

if (ch->ability_exp[2] > (21000000 * ch->ability[2]))
{
ch->ability_exp[2] -= 21000000 * ch->ability[2];
ch->ability[2]++;
}
return;
}

// Prowess
if (ch->ability_cur[CUR] == 3 && ch->ability[3] < 100)
{
ch->ability_exp[3] += gain;

if (ch->ability_exp[3] > (21000000 * ch->ability[3]))
{
ch->ability_exp[3] -= 21000000 * ch->ability[3];
ch->ability[3]++;
}
return;
}

// Ability
if (ch->ability_cur[CUR] == 4 && ch->ability[4] < 100)
{
ch->ability_exp[4] += gain;

if (ch->ability_exp[4] > (21000000 * ch->ability[4]))
{
ch->ability_exp[4] -= 21000000 * ch->ability[4];
ch->ability[4]++;
}
return;
}
return;
}







void do_ability(CHAR_DATA * ch, char *argument)
{
char buf[MAX_STRING_LENGTH];
char arg[MAX_INPUT_LENGTH];

int damcap=0;
int ac=0, hr=0, dr=0;
int parry=0, dodge=0;
int resist=0, hp=0;
int attack=0, damage=0;

char * ability;

if (IS_NPC(ch))
return;

argument = one_argument(argument, arg);


if (ch->ability[0] < 1) ch->ability[0] = 1;
if (ch->ability[1] < 1) ch->ability[1] = 1;
if (ch->ability[2] < 1) ch->ability[2] = 1;
if (ch->ability[3] < 1) ch->ability[3] = 1;
if (ch->ability[4] < 1) ch->ability[4] = 1;

if (ch->ability[0] > 1)
{
resist += ch->ability[0] / 40;
hp += ch->ability[0] * 100;
}
if (ch->ability[1] > 1)
{
damage += ch->ability[1] * 4;
dr += ch->ability[1] * 10;
}
if (ch->ability[2] > 1)
{
attack += ch->ability[2] / 20;
hr += ch->ability[2] * 10;
}
if (ch->ability[3] > 1)
{
ac -= ch->ability[3] - (ch->ability[3] * 10);
parry += ch->ability[3] / 10;
}
if (ch->ability[4] > 1)
{
attack += ch->ability[4] / 20;
dodge += ch->ability[4] / 10;
}




        if (arg[0] == '\0')
	{
stc("#C[#c--------------------------#C[#7Ability#C]#c--------------------------#C]#n\n\r",ch);
         stc("[ Ability   Level   Exp	  Exp needed	  Mods]\n\r",ch);
xprintf(buf, "  Toughness   %-3d    %-10ld   %-10ld   Resist: %d/HP: %d\n\r", 
	ch->ability[0], ch->ability_exp[0],
	((21000000 * ch->ability[0]) - ch->ability_exp[0]),
	resist, hp );
stc(buf,ch);
xprintf(buf, "  Power       %-3d    %-10ld   %-10ld   Damage: %d/DR: %d\n\r",
	ch->ability[1], ch->ability_exp[1],
        ((21000000 * ch->ability[1]) - ch->ability_exp[1]),
	damage, dr );
stc(buf,ch);
xprintf(buf, "  Skill       %-3d    %-10ld   %-10ld   Attacks: %d/HR: %d\n\r", 
	ch->ability[2], ch->ability_exp[2],
        ((21000000 * ch->ability[2]) - ch->ability_exp[2]),
	attack, hr );
stc(buf,ch);
xprintf(buf, "  Prowess     %-3d    %-10ld   %-10ld   AC: %d/Parry: %d\n\r",
	ch->ability[3], ch->ability_exp[3],
        ((21000000 * ch->ability[3]) - ch->ability_exp[3]),
	 ac, parry );
stc(buf,ch);
xprintf(buf, "  Agility     %-3d    %-10ld   %-10ld   Attack: %d/Dodge %d\n\r", 
	ch->ability[4], ch->ability_exp[4],
        ((21000000 * ch->ability[4]) - ch->ability_exp[4]),
	attack, dodge);
stc(buf,ch);

	if (ch->ability_cur[CUR] == 0) ability = "Combat Toughness";
        else if (ch->ability_cur[CUR] == 1) ability = "Combat Power";
        else if (ch->ability_cur[CUR] == 2) ability = "Combat Skill";
        else if (ch->ability_cur[CUR] == 3) ability = "Combat Prowess";
        else if (ch->ability_cur[CUR] == 4) ability = "Combat Ability";
	else ability = "None";
	

	xprintf(buf,"\n\r  Current Focus: %s\n\r", ability);
	stc(buf,ch);

stc("#C[#c--------------------------#C[#7Ability#C]#c--------------------------#C]#n\n\r",ch);
	return;
	}

        if (!str_cmp(arg, "toughness"))
	{
	ch->ability_cur[CUR] = 0;

        send_to_char("You Focus on Toughness.\n\r", ch);
	save_char_obj(ch);
        return;
	}

        if (!str_cmp(arg, "power"))
        {
        ch->ability_cur[CUR] = 1;

        send_to_char("You Focus on Combat Power.\n\r", ch);
        save_char_obj(ch);
        return;
        }
        if (!str_cmp(arg, "skill"))
        {
        ch->ability_cur[CUR] = 2;

        send_to_char("You Focus on Combat Skill.\n\r", ch);
        save_char_obj(ch);
        return;
        }
        if (!str_cmp(arg, "prowess"))
        {
        ch->ability_cur[CUR] = 3;

        send_to_char("You Focus on Combat Prowess.\n\r", ch);
        save_char_obj(ch);
        return;
        }
        if (!str_cmp(arg, "agility"))
        {
        ch->ability_cur[CUR] = 4;

        send_to_char("You Focus on Combat Agility.\n\r", ch);
        save_char_obj(ch);

        return;
        }

return;
}


