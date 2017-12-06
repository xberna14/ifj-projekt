/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */

#define ER_SYN 2
#define ER_SEM_P 3
#define ER_SEM_T 4
#define ER_SEM_O 6
#define ER_INT 99
#define MAXSTACK 30
#define SYNTAX_OK 0


int parse(tBSTNodePtr *tb, tListOfInstr *instrList);
int program();
int mainList();
int inParam();
int exprlist();
int assignment(tData **var1,tData **var2);
int statement();
int statements();
int funcParam();
int declrList();
