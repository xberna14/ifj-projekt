/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */


//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);
