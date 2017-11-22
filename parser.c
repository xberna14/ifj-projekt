/*
** 		_
**     |
** |___|___
**     |   | 
**	  _|
**
*/


/* TODO
	Precedenci analyza: cca 85% Dodelat deleni, vyladit mouchy, spravne navazat na tokeny z lexeru, spravne navazat generovani insturkci, overit jestli je precedenci tabulka OK
						spravne navazat na tabulku symbolu implementovanou jako bin. strom
	LL Gramatika: 0%
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "lexer.c"
#include "parser.h"
#include "string.h"
#include "listinstr.h"



int counterVar = 1;
tListOfInstr *list; // glob. prom. pro seznam instrukci


int generateVariable(string *var) {
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace

	strClear(var);
	if (strAddChar(var, '$'))
	{
		return ER_INT;
	}
	int i;
	i = counterVar;
	while (i != 0)
	{
		if (strAddChar(var, (char)(i % 10 + '0')))
		{
			return ER_INT;
		}
		i = i / 10;
	}
	counterVar++;
	return 0;
}

int generateInstruction(int instType, void *addr1, void *addr2, void *addr3) {
//vlozi novou instrukci do seznamu instrukci

	tInstr I;
	I.instType = instType;
	I.addr1 = addr1;
	I.addr2 = addr2;
	I.addr3 = addr3;
	if (listInsertLast(list, I))
	{
		return ER_INT;
	}
	return 0;
}


//  PRECEDENCNI SYNTAKTICKA ANALYZA

typedef enum
{
	O_Plus,				// + 0
	O_Minus,			// - 1
	O_Krat,				// * 2
	O_DelDouble,		// / 3
	O_DelInt,			// \ 4
	O_Mensi,			// < 5
	O_Vetsi,			// > 6
	O_MensiRovno,		// <= 7
	O_VetsiRovno,		// >= 8
	O_Rovno,			// = 9
	O_NeRovno,			// <> 10
	O_LevaZ,			// ( 11
	O_PravaZ,			// ) 12
	O_ID,				// id 13
  
	O_Lt,				// $ 14

	TAB_Rovno,			// = 15
	TAB_Mensi,			// < 16
	TAB_Vetsi,			// > 17
	TAB_Chyba,			// X 18

	PTE,
} prvkyPT;


// TODO
// radek je znak na zasobniku, sloupec vstupni token
int PTAB [15][15] =
{  //		+			-			*			/			\			<			>		<=			>=			=		<>			(			)			id       $
/* + */		{TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* - */		{TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* * */		{TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* / */		{TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},	
/* \ */		{TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* < */		{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* > */		{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* <= */	{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* >= */	{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* = */		{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* <> */	{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi, TAB_Mensi, TAB_Vetsi},
/* ( */		{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Rovno, TAB_Mensi, TAB_Chyba},
/* ) */		{TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Chyba, TAB_Vetsi, TAB_Chyba, TAB_Vetsi},
/* id */	{TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Vetsi, TAB_Chyba, TAB_Vetsi, TAB_Chyba, TAB_Vetsi},
/* $ */		{TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Mensi, TAB_Chyba, TAB_Mensi, TAB_Chyba},
 
};

unsigned int indexing (int i) {
	
	int id = 13;
	if (i < O_ID)
	{
		return i;
	}

	if (i < O_Lt)
	{
		return id;
	}

	if (O_Lt <= i)
	{
		return i;
	}

	return i;
}

prvkyPT getPrvek(int i, int j)
{
	return PTAB [indexing(i)][indexing(j)];
}

char cnst;

// TODO
prvkyPT changeToken (Token_t token) { // upravim si token z lexeru

	switch (token.token)
	{
		case PLUS: return O_Plus;
		case MINUS: return O_Minus;
		case MUL: return O_Krat;
		case DIV: return O_DelDouble;
		case CELDIV: return O_DelInt;
		case GREAT: return O_Vetsi;
		case LESS: return O_Mensi;
		case GREAT_EQ: return O_VetsiRovno;
		case LESS_EQ: return O_MensiRovno;
		case EQUAL: return O_Rovno;
		case N_EQUAL: return O_NeRovno;
		case LEFT_BRACKET: return O_LevaZ;
		case RIGHT_BRACKET: return O_PravaZ;
		case ID: return O_ID;
		
		case tInt :
		constant = 'i';
		return O_ID;

		case tString:
		constant = 's';
		return O_ID;

		case tFloat:
		constant = 'f';
		return O_ID;

		default: return O_Lt;
	}
}




typedef struct mstack {
	tData * data;
	prvkyPT type;
} 	mstack_item;
 

typedef struct auxStack {
	mstack_item * a;
	int top;
	int size;
}	*tStack;
 
int SInit(tStack * S) {
	if (((*S) =  malloc (sizeof (struct auxStack))) == NULL)
	{
		return ER_INT;
	}
	if (((*S)->a =  malloc (MAXSTACK * sizeof (struct mstack))) == NULL)
	{
		return ER_INT;
	}

	(*S)->top = 0;
	(*S)->size = MAXSTACK;

	for(int i = 0; i < MAXSTACK; i++)
	{
		(*S)->a[i].type = 0;
		(*S)->a[i].data= NULL;
	}
	return 0;
}
 
void SDispose(tStack * S)
{
	free((*S)->a);
	free(*S);
}
 
void SPush(tStack * S, int ptr, tData *addr)
{
	if ((*S)->top >= (*S)->size)
	{
		(*S)->size += MAXSTACK;
		(*S)->a = realloc ((*S)->a, (*S)->size * sizeof (struct auxStack));
	}
	(*S)->top++;
	(*S)->a[(*S)->top].type= ptr;
	(*S)->a[(*S)->top].data= addr;
}
 
void SPush_term(tStack * S,int ptr, int adress)
{
	prvkyPT c;
	tData *data;
	adress++;
	(*S)->top=adress;

	if ((*S)->top >= (*S)->size)
	{
		(*S)->size += MAXSTACK;
		(*S)->a = realloc ((*S)->a, (*S)->size * sizeof (struct auxStack));
	}
	data = (*S)->a[(*S)->top].data;
	c = (*S)->a[(*S)->top].type;
 
	(*S)->a[(*S)->top].data = NULL;
	(*S)->a[(*S)->top].type=ptr;

	if (c == PTE)
	{
		adress++;
		(*S)->top=adress;
		if ((*S)->top >= (*S)->size)
		{
			(*S)->size += MAXSTACK;
			(*S)->a = realloc ((*S)->a, (*S)->size * sizeof (struct auxStack));
		}
		(*S)->a[(*S)->top].data = data;
		(*S)->a[(*S)->top].type = c;
	}
}


void SPop(tStack * S)
{
	if ((*S)->top > 0 )
	{
		(*S)->a[(*S)->top].type=0;
		(*S)->top--;
	}
}

mstack_item STop(tStack * S)
{
	if ((*S)->top == 0)
	{
		fprintf(stderr,"chyba zasobniku");
	}
	return (*S)->a[(*S)->top];
}

int SFind_Term(tStack * S)
{
	if ((*S)->a[(*S)->top].type <= O_Lt)
	{
		return (*S)->top;
	}
	return (*S)->top-1;
}
 
mstack_item STop_Term(tStack * S)
{
	int c;
	int position;
	mstack_item term;
	c = (*S)->top;
	position = SFind_Term(S);
	(*S)->top = position;
	term = STop(S);
	(*S)->top = c;
	return term;
}

int insertInt(int var, tData ** varinfo)
{
	string new;
	if (strInit(&new))
	{
		return ER_INT;
	}
	if (generateVariable(&new))
	{
		return ER_INT;
	}
	if (BSTInsert (TbSmBST, &new, TYPE_INT))
	{
		return ER_INT;
	}
	*varinfo = BSTSearch(TbSmBST, &new);
	strFree(&new);
	(*varinfo)->varValue.valueInt = var;
	return 0;
}
 
int insertDbl(double var, tData ** varinfo)
{
	string new;
	if (strInit(&new))
	{
		return ER_INT;
	}
	if (generateVariable(&new))
	{
		return ER_INT;
	}
	if(BSTInsert (TbSmBST, &new, TYPE_DBL))
	{
		return ER_INT;
	}
	*varinfo = BSTSearch(TbSmBST, &new);
	strFree(&new);
	(*varinfo)->varValue.valueDBL = var;
	return 0;
}
 
int insertStr(tData ** varinfo)
{
	string new;
	if (strInit(&new))
	{
		return ER_INT;
	}
	if (generateVariable(&new))
	{
		return ER_INT;
	}
	if (BSTInsert (TbSmBST, &new, TYPE_STR))
	{
		return ER_INT;
	}
	*varinfo = BSTSearch(TbSmBST, &new);
	strFree(&new);
	if (strCopyString( &((*varinfo)->varValue.valueStr), &attr))
	{
		return ER_INT;
	}
	return 0;
}

int getExpr(tData **output)
{
	mstack_item a;
	mstack_item b;
	mstack_item d;
	mstack_item k;
	mstack_item m;
	mstack_item n;
	mstack_item f;
	mstack_item c;
	mstack s;

	SInit(&s);     
	SPush(&s, O_Lt, NULL); // $ - zarazka
	tData *varinfo;
	prvkyPT term;
	int result = SYNTAX_OK;
	a.type= changeToken(token); //konverze tokenu

	if(token.token == ID) // token je id, zjistit addresu
	{
		if ((varinfo = BSTSearch(TbSmBST, &attr)) == NULL)
		{
			SDispose(&s);
			return ER_SEM_P;
		}
	}
	else if(token.token == INT) // token je int, prevedeme ze stringu, vygenerujeme prom a na jeji adresu dame nacteny int
	{
		if ((result = insertInt(atoi(attr.str), &varinfo)))
		{
			SDispose(&s);
			return result;
		}
	}
	else if (token.token == DOUBLE)
	{
		if ((result = insertDbl(atof(attr.str), &varinfo)))
		{
			SDispose(&s);
			return result;
		}
	}
	else if (token.token == STRING)
	{
		if ((result = insertStr(&varinfo)))
		{
			SDispose(&s);
			return result;
		}
	}
	else
	{
		varinfo = NULL;
	}

	do
	{
		b = STop_Term(&s); //znak ze zasobniku
    	switch (ClenTab(b.type,a.type)) //vyber clena tabulky
    	{
			case TAB_Rovno: // push(a), precist symbol a ze vstupu
			SPush(&s, a.type, varinfo);
			b = STop_Term(&s);    
			if ((token = getNextToken(&attr)) == ER_LEX)
			{
				SDispose(&s);
				return ER_LEX;
			}
			else if (token.token == ER_INT)
			{
				SDispose(&s);
				return ER_INT;
			}
			a.type= changeToken(token);
			if (token == ID)
			{
				if ((varinfo = BSTSearch(TbSmBST, &attr)) == NULL)
				{
					SDispose(&s);
					return ER_SEM_P;
				}
			}
			else if (token.token == INT)
			{
				if ((result = insertInt(atoi(attr.str), &varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else if (token.token == DOUBLE)
			{
				if ((result = insertDbl(atof(attr.str), &varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else if (token.token == STRING)
			{
				if ((result = insertStr(&varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else
			{
				varinfo = NULL;
			}
			break;
			
			case TAB_Mensi: // zamenit vrchol b za b<, push(token), precist symbol a ze vstupu
			term = SFind_Term(&s);
			SPush_term(&s,PTLaver,term);
			SPush(&s, a.type, varinfo);
			if ((token = getNextToken(&attr)) == ER_LEX)
			{
				SDispose(&s);
				return ER_LEX;
			}
			else if (token.token  == ER_INT)
			{
				SDispose(&s);
				return ER_INT;
			}
			a.type= changeToken(token);
			if (token.token == tIdentif)
			{
				if ((varinfo = BSTSearch(TbSmBST, &attr)) == NULL)
				{
					SDispose(&s);
					return ER_SEM_P;
				}
			}
			else if(token.token == INT)
			{
				if ((result = insertInt(atoi(attr.str), &varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else if(token.token == DOUBLE)
			{
				if ((result = insertDbl(atof(attr.str),&varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else if(token.token == STRING)
			{
				if ((result = insertStr(&varinfo)))
				{
					SDispose(&s);
					return result;
				}
			}
			else
			{
				varinfo = NULL;
			}
			b = STop_Term(&s); 
			break;


			case TAB_Vetsi: // <y je na zasobniku, r: A → y ∈ P, pak zamenit <y za A, vypsat r 
			d = STop(&s);      
			switch (d.type)
			{
				case O_ID:
				SPop(&s);
				f = STop(&s);
				if (f.type == TAB_Mensi)
					{
						SPop(&s);
						if (d.data == NULL)
						{
							SDispose(&s);
							return ER_SEM_P;
						}
						SPush(&s,PTE, d.data);
						*output = d.data;
					}
				break;

				case O_PravaZ:
				SPop(&s);
				c = STop(&s);
				SPop(&s);
				SPop(&s);
				SPop(&s);
				SPush(&s,PTE, c.data);
				*output = c.data;
				break;

				case PTE:
				c = STop(&s);
				SPop(&s);
				k = STop(&s);
				if ((k.type >= O_Plus) && (k.type <= O_NeRovno))
				{
					SPop(&s);
					m = STop(&s);
					SPop(&s);
					n = STop(&s);
					if ((m.type == PTE) && (n.type == TAB_Mensi))
					{
						SPop(&s);
						string new;
						if (strInit(&new))
						{
							SDispose(&s);
							strFree(&new);
							return ER_INT;
						}
						if(generateVariable(&new))
						{
							SDispose(&s);
							strFree(&new);
							return ER_INT;
						}
						if (BSTInsert (TbSmBST, &new, TYPE_INT))
						{
							SDispose(&s);
							strFree(&new);
							return ER_INT;
						}
						tData *vysledek;
						vysledek = BSTSearch(TbSmBST, &new);
						strFree(&new);
						switch(k.type)  // a podle typu operace provedeme vhodnou instrukci
						{
							case O_Plus:

							if ((m.data->varType == TYPE_INT) && (c.data->varType== TYPE_INT))
							{
								vysledek->varType = TYPE_INT;
							}
							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_INT))
							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_INT) && (c.data->varType == TYPE_DBL))
							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_DBL))
							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_STR) && (c.data->varType == TYPE_STR))
							{
								vysledek->varType = TYPE_STR;
							}
							else
							{
								return ER_SEM_P;
							}

							if(generateInstruction(I_ADD, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_Minus:
							if ((m.data->varType == TYPE_INT) && (c.data->varType== TYPE_INT))
							{
								vysledek->varType = TYPE_INT;
							}
							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_INT))
							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_INT) && (c.data->varType == TYPE_DBL))
							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_DBL))
							{
								vysledek->varType = TYPE_DBL;
							}
							else
							{
								return ER_SEM_P;
							}

							if (generateInstruction(I_SUB, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;
							
							case O_Krat:

							if ((m.data->varType == TYPE_INT) && (c.data->varType== TYPE_INT))
							{
								vysledek->varType = TYPE_INT;
							}
 							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_INT))
 							{
								vysledek->varType = TYPE_DBL;
 							}
 							else if ((m.data->varType == TYPE_INT) && (c.data->varType == TYPE_DBL))
 							{
								vysledek->varType = TYPE_DBL;
							}
							else if ((m.data->varType == TYPE_DBL) && (c.data->varType == TYPE_DBL))
							{
								vysledek->varType = TYPE_DBL;
							}
							else
							{
								return ER_SEM_P;
							}

							if(generateInstruction(I_MUL, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_DelInt:
							if ((m.data->varType == TYPE_INT) && (c.data->varType== TYPE_INT))
							{
								vysledek->varType = TYPE_INT;
							}

							else
							{
								return ER_SEM_P;
							}

							if(generateInstruction(I_DIV,m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_DelDouble:

							// TODO

							break;
							
							case O_Vetsi:
							if (generateInstruction(I_GREATER,m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_Mensi:
							if (generateInstruction(I_LESS,m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_VetsiRovno:
							if (generateInstruction(I_GREATEQ, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							

							case O_MensiRovno:
							if (generateInstruction(I_LESSEQ, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_Rovno:
							if (generateInstruction(I_EQ, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							case O_NeRovno:
							if (generateInstruction(I_NOTEQ, m.data, c.data, vysledek))
							{
								SDispose(&s);
								return ER_INT;
							}	
							SPush(&s,PTE, vysledek);
							*output = vysledek;
							b = STop_Term(&s);
							break;

							default:
							SDispose(&s);
							return ER_SYN;
							break;
						}
					}
				}
				break;

				default:
				SDispose(&s);
				return ER_SYN;
				break;
			}
			break;

			default:
			SDispose(&s);
			return ER_SYN;
			break;
 
			case TAB_Chyba:
			if ((a.type == O_Lt) && (b.type == O_Lt))
			{
				result = ER_SYN;
			}
			else if (token.token == RIGHT_BRACKET)
			{
				SDispose(&s);
				return SYNTAX_OK;
			}
			else
			{
				SDispose(&s);
				return ER_SYN;
			}
			break;
		}
 
	} while ((a.type != O_Lt) || (b.type != O_Lt));
	
	if ((a.type == O_Lt) && (b.type == O_Lt))
	{
		result = SYNTAX_OK;
	}
	SDispose(&s);
	return result;
}

// KONEC PSA (konecne)


