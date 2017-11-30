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
	LL Gramatika: 50%
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "lexer.h"
#include "parser.h"
#include "string.h"
#include "listinstr.h"



int counterVar = 1;
tListOfInstr *list; // glob. prom. pro seznam instrukci
tBSTNodePtr *TbSmBST;   // glob. prom. uchovavajici tabulku symbolu
string attr; // glob. prom. pro aktualni atribut tokenu
Token_t token; // glob. prom. pro aktualni token

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
	return PTAB [indexing(j)][indexing(i)];
}

char constant;

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
    	switch (getPrvek(b.type,a.type)) //vyber clena tabulky
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
/////////////////////////
// LL GRAMATIKA

//2.	<dcllist> ->  <declarefunc> <dclist>
//3.	<dcllist> ->  ε					
//4.	<declarefunc> -> ε 
//5.	<declarefunc> -> declare function id (<funcparam>) as <type> EOL
//6.	<declarefunc> -> function id (<funcparam>) as <type> EOL <statements> end function EOL

int declrList()
{
	int result = 0;
	if (strCmpConstStr(&attr, "declare") == 0) // declare
	{
		if ((token = getNextToken(&attr)) == ER_LEX) // declare function
		{
			return ER_LEX;
		}
		else if (token == ER_INT) 
		{
			return ER_INT;
		}

		if (strCmpConstStr(&attr, "function") == 0) 
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // declare function id
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			if (token != ID)
			{
				return ER_SYN;
			}

			string id;
			if (strInit(&id))
			{
 				return ER_INT;
			}

			if (strCopyString(&id, &attr))
			{
				return ER_INT;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // declare function id (
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (token != LEFT_BRACKET)
			{
				strFree(&id);
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // declare function id ( param...
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (token == ID)
			{
				result = funcParam(); // po ukonceni funkce bude v tokenu )
				if (result != SYNTAX_OK)
				{
					return result;
				}
			}
			else if (token != RIGHT_BRACKET) // declare function id ()
			{
				strFree(&id);
				return ER_INT;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // declare function id (param/bez) as
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "as") == 0)
			{
				if ((token = getNextToken(&attr)) == ER_LEX) // declare function id (param/bez) as type
				{
					strFree(&id);
					return ER_LEX;
				}
				else if(token == ER_INT)
				{
					strFree(&id);
					return ER_INT;
				}				
				if ((strCmpConstStr(&attr, "integer") == 0)) // integer
				{
					if (( result = BSTInsert(TbSmBST, &id, TYPE_INT)) == -1 ) // vlozime do tab symbolu
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "double") == 0)) // double
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_DBL)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "string") == 0)) // string
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_STR)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else
				return ER_SYN;
			}

			else
			{
				strFree(&id);
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // declare function id (param/bez) as type EOL
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			if (token != EOL)
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // dalsi token urci co bude dal
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			return declrList();
		}

	}

	else if (strCmpConstStr(&attr, "function") == 0) 
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // function id
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			if (token != ID)
			{
				return ER_SYN;
			}

			string id;
			if (strInit(&id))
			{
 				return ER_INT;
			}

			if (strCopyString(&id, &attr))
			{
				return ER_INT;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // function id (
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (token != LEFT_BRACKET)
			{
				strFree(&id);
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // function id ( param...
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (token == ID)
			{
				result = funcParam(); // po ukonceni funkce bude v tokenu )
				if (result != SYNTAX_OK)
				{
					return result;
				}
			}
			else if (token != RIGHT_BRACKET) //  function id ()
			{
				strFree(&id);
				return ER_INT;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) //  function id (param/bez) as
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "as") == 0)
			{
				if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type
				{
					strFree(&id);
					return ER_LEX;
				}
				else if(token == ER_INT)
				{
					strFree(&id);
					return ER_INT;
				}				
				if ((strCmpConstStr(&attr, "integer") == 0)) // integer
				{
					if ((result = BSTInsert(TbSmBST, &id, TYPE_INT)) == -1 ) // vlozime do tab symbolu
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "double") == 0)) // double
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_DBL)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "string") == 0)) // string
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_STR)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else
				return ER_SYN;
			}

			else
			{
				strFree(&id);
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type EOL
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			if (token != EOL)
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type EOL statements
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			result = statements();

			if (result != SYNTAX_OK)
			{
				return result;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type EOL statements end
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "end") == 0)
			{
				if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type EOL statements end function
				{
					return ER_LEX;
				}
				else if (token == ER_INT) 
				{
					return ER_INT;
				}

				if (strCmpConstStr(&attr, "function") == 0)
				{
					if ((token = getNextToken(&attr)) == ER_LEX) // function id (param/bez) as type EOL statements end function EOL
					{
						return ER_LEX;
					}
					else if (token == ER_INT) 
					{
						return ER_INT;
					}

					if (token != EOL)
					{
						return ER_SYN;
					}

					if ((token = getNextToken(&attr)) == ER_LEX) // dalsi token urci co bude dal
					{
						return ER_LEX;
					}
					else if (token == ER_INT) 
					{
						return ER_INT;
					}

					return declrList();
				}
				else
				{
					return ER_SYN;
				}
			}
			else
			{
				return ER_SYN;
			}

		}
	else if (strCmpConstStr(&attr, "scope") == 0)
		{
			/*
			if ((token = getNextToken(&attr)) == ER_LEX) // dalsi token urci co bude dal
			{
				return ER_LEX;
			}
				else if (token == ER_INT) 
			{
				return ER_INT;
			} */

			return SYNTAX_OK;
		}
	else
	{
		return ER_SYN;
	}
}

//7.	<funcparam> -> id as <type> <funcparam_n> 
//8.	<funcparam> -> ε
//9.	<funcparam_n> -> ε
//10.	<funcparam_n> -> , id as <type> <funcparam_n>
int funcParam()
{
	if (token == ID)
	{
	string id;
	if (strInit(&id))
	{
 		return ER_INT;
	}

	if (strCopyString(&id, &attr))
	{
		return ER_INT;
	}

	if ((token = getNextToken(&attr)) == ER_LEX) // id as
	{
		strFree(&id);
		return ER_LEX;
	}
	else if (token == ER_INT) 
	{
		strFree(&id);
		return ER_INT;
	}

	if (strCmpConstStr(&attr, "as") == 0)
	{
		if ((token = getNextToken(&attr)) == ER_LEX) // id as type
		{
			strFree(&id);
			return ER_LEX;
		}
		else if(token == ER_INT)
		{
			strFree(&id);
			return ER_INT;
		}

				if ((strCmpConstStr(&attr, "integer") == 0)) // integer
				{
					if ((result = BSTInsert(TbSmBST, &id, TYPE_INT)) == -1 ) // vlozime do tab symbolu
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "double") == 0)) // double
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_DBL)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else if ((strCmpConstStr(&attr, "string") == 0)) // string
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_STR)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
					strFree(&id);
				}
				else
				return ER_SYN;

		if ((token = getNextToken(&attr)) == ER_LEX) // urcime co bude dal
		{
			return ER_LEX;
		}
		else if(token == ER_INT)
		{
			return ER_INT;
		}

		if (token == COMMA) // nasleduje dalsi param
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // , id
			{
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				return ER_INT;
			}
			return = funcParam();
		}
		else if (token == RIGHT_BRACKET) // zadny dalsi param
		{
			return SYNTAX_OK;
		}
		else
		{
			return ER_SYN;
		}
	}
	else
	{
		return ER_SYN;
	}
	} // end if (token == id)
	else
	{
		return ER_SYN;
	}
}


//12.	<statements> -> ε
//13.	<statements> -> <statement> <stlist>
//14.	<stlist> -> <statement> <stlist>
//15.	<stlist> -> ε
int statements()
{
	int result = 0;
	result = statement(); // zpracujeme prvni statement
	if (result != SYNTAX_OK)
	{
		return result;
	}

/*	if ((token = getNextToken(&attr)) == ER_LEX) // musi nasledovat EOL, pokud nasleduje = jde o definici promenne
	{
		return ER_LEX;
	}
	else if (token == ER_INT) 
	{
		return ER_INT;
	}

	if (token == EQUAL)
	{

		
	}

	if (token != EOL)
	{
		return ER_SYN;
	} */

	if ((token = getNextToken(&attr)) == ER_LEX) // zjistim zda nasleduje dalsi statement
	{
		return ER_LEX;
	}
	else if (token == ER_INT) 
	{
		return ER_INT;
	}

	if ((strCmpConstStr(&attr, "end") == 0) || ((strCmpConstStr(&attr, "loop") == 0)) || ((strCmpConstStr(&attr, "else") == 0))) // v tokenu je end, else nebo loop, statements konci
	{
		return SYNTAX_OK;
	}

	else if ((token == KEYWORD) || (token == ID)) // dalsi token je statement
	{
		return statements();
	}
	else
	{
		return ER_SYN;
	}

}


/*
16.	<statement> -> dim id as <type> EOL
17.	<statement> -> dim id as <type> = expr EOL
18.	<statement> -> id = expr EOL
19.	<statement> -> input id EOL
20.	<statement> -> print <exlist_n> EOL
21.	<statement> -> if expr then EOL <statements> else EOL <statements> end if EOL
22.	<statement> -> do while expr EOL <statements> loop EOL
23.	<statement> -> id = id_funkce (<in_param>) EOL

*/
int statement()
{
	int result = 0;
	if (token == ID) // prirazeni do prom
	{

		tData *data;
		tData *data1;
      	if ((data = BSTSearch(TbSmBST, &attr)) == NULL) // podivame se zda promenna existuje
      	{
      		return ER_SEM_P;
      	}


		if ((token = getNextToken(&attr)) == ER_LEX) // id =
		{
			return ER_LEX;
		}
		else if (token == ER_INT)
		{
			return ER_INT;
		}

		if (token != EQUAL)
		{
			return ER_SYN;
		}

		if ((token = getNextToken(&attr)) == ER_LEX) // ziskame dalsi token
		{
			return ER_LEX;
		}
		else if (token == ER_INT)
		{
			return ER_INT;
		}
// TODO
		// if expr
		return assignment(&data,&data1);
		// else je to id funkce
	}

	else if (token == KEYWORD)
	{
		
		if ((strCmpConstStr(&attr, "dim") == 0)) // dim
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // dim id
			{
				return ER_LEX;
			}
			else if (token == ER_INT)
			{
				return ER_INT;
			}
			if (token != ID)
			{
				return ER_SYN;
			}
			
			string id;
			if (strInit(&id))
			{
 				return ER_INT;
			}

			if (strCopyString(&id, &attr))
			{
				return ER_INT;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // dim id as
			{
				strFree(&id);
				return ER_LEX;
			}
			else if (token == ER_INT) 
			{
				strFree(&id);
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "as") == 0)
			{
				if ((token = getNextToken(&attr)) == ER_LEX) // dim id as type
				{
					strFree(&id);
					return ER_LEX;
				}
				else if(token == ER_INT)
				{
					strFree(&id);
					return ER_INT;
				}

				if ((strCmpConstStr(&attr, "integer") == 0)) // integer
				{
					if ((result = BSTInsert(TbSmBST, &id, TYPE_INT)) == -1 ) // vlozime do tab symbolu
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
				}
				else if ((strCmpConstStr(&attr, "double") == 0)) // double
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_DBL)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
				}
				else if ((strCmpConstStr(&attr, "string") == 0)) // string
				{
					if ((BSTInsert (TbSmBST, &id, TYPE_STR)) == -1 )
					{
						strFree(&id);
						return ER_SEM_O;
					}
					else if ( result != 0 )
					{
						strFree(&id);
						return ER_INT;
					}
				}
				else
				{
					strFree(&id);
					return ER_SYN;
				}

				if ((token = getNextToken(&attr)) == ER_LEX) // dalsi token, bud EOL nebo =
				{
					strFree(&id);
					return ER_LEX;
				}
				else if (token == ER_INT)
				{
					strFree(&id);
					return ER_INT;
				}

				if (token == EOL)
				{
					strFree(&id);
					return SYNTAX_OK;
				}
				else if (token == EQUAL)
				{
					tData *data;
					tData *data1;
      				if ((data = BSTSearch(TbSmBST, &id)) == NULL) // podivame se zda promenna existuje, mela by, prave sme ji vytvorili
      				{
      					return ER_SEM_P;
      				}

      				if ((token = getNextToken(&attr)) == ER_LEX) // ziskame dalsi token - musi byt vyraz
					{
						return ER_LEX;
					}
					else if (token == ER_INT)
					{
						return ER_INT;
					}
					return assignment(&data,&data1);

				}
				else
				{
					return ER_SYN;
				}
			}
			else
			{
				strFree(&id);
				return ER_SYN;
			}
		}
		

		else if ((strCmpConstStr(&attr, "input") == 0))
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // input id
			{
				return ER_LEX;
			}
			else if (token == ER_INT)
			{
				return ER_INT;
			}
			if (token != ID)
			{
				return ER_SYN;
			}

			if (token != ID)
			{
				return ER_SYN;
			}

			tData *data;
			tData *data1;
      		if ((data = BSTSearch(TbSmBST, &attr)) == NULL) // podivame se zda promenna existuje
      		{
      			return ER_SEM_P;
      		}
// TODO vygenerovat instrukci

      		if ((token = getNextToken(&attr)) == ER_LEX) // dalsi token musi byt EOL
			{
				return ER_LEX;
			}
			else if (token == ER_INT)
			{
				return ER_INT;
			}

			if (token == EOL)
			{
				return SYNTAX_OK;
			}
			else
			{
				return ER_SYN;
			}
		}

		else if ((strCmpConstStr(&attr, "print") == 0))
		{
			int result = 0;
			if ((token = getNextToken(&attr)) == ER_LEX) // nactu token pro PA
			{
				return = ER_LEX;
			}
			else if(token  == ER_INT)
			{
			return ER_INT;
			}

			if (token == EOL) // pokud EOL, tak je print zavolan bez expr => SYNTAX ERROR
			{
				return ER_SYN;
			}

			result = exprlist(); // v pripade spravnych vyrazu zpracuje i EOL
			if (result != SYNTAX_OK)
			{
				return result;
			}

			return SYNTAX_OK;
		}

		else if ((strCmpConstStr(&attr, "if") == 0))
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr
			{
				return = ER_LEX;
			}
			else if(token  == ER_INT)
			{
			return ER_INT;
			}

			string newVar; // vygenerujeme prom pro vysledek expr
			if (strInit(&newVar))
        	{
        		return ER_INT;
        	}
      		if (generateVariable(&newVar))
        	{
        		return ER_INT;
        	}

        	if (BSTInsert (TbSmBST, &newVar, TYPE_INT) != 0) // ulozime do tabulky
			{
				return ER_INT;
			}

			tData *newVariableInfo;
			strFree(&newVar); // uvolnime nazev promenne

			result = readExpr(&newVariableInfo); // volame PA (po konci PA bude token posunut na then)
      		if (result != SYNTAX_OK)
      		{
      			return result;
      		}

			void *addrOfIfNotGoto; 

			// pokud je expr false, skaceme na konec
			// nagenerujeme instrukci podmineneho skoku a ulozime jeji adresu,
      		// protoze pozdeji bude potreba doplnit adresu skoku, kam skakat

			if (generateInstruction(I_IFNOTGOTO, (void*) newVariableInfo, NULL, NULL))
			{	
				return ER_INT;
			}
			addrOfIfNotGoto = listGetPointerLast(list); // vrati ukazatel na posledni instrukci

			if (strCmpConstStr(&attr, "then") != 0) // dalsi token musi byt then
			{
				return SYNTAX_ERROR;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then EOL
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (token != EOL)
			{		
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then EOL statements
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if ( (result = statements()) != SYNTAX_OK )
			{
				return result;
			}

			void *addrGoto;
			// nagenerujeme instrukci skoku, ktera preskoci else statement
			if (generateInstruction(I_GOTO, NULL, NULL, NULL))
			{
				return ER_INT;
			}
			addrGoto = listGetPointerLast(list);
			// nagenerujeme instrukci noveho navesti
			if (generateInstruction(I_LAB, NULL, NULL, NULL))
			{
				return ER_INT;
			}
			void *addrOfLab1;
			addrOfLab1 = listGetPointerLast(list); // vrati ukazatel na posledni instrukci
			// do drive generovane instrukce podmineneho skoku
			// nastavime aktualni instrukci podle adresy instrukce ifnotgoto
			listGoto(list, addrOfIfNotGoto); // nastavi aktivni instrukci podle zadaneho ukazatele
			tInstr *data;
			data = listGetData(list); // vrati aktiv. instrukci
			data->addr3 = addrOfLab1;  // a jako 3. adresu dame adresu navesti
			//kam se bude skakat, pokud podminka nebude platit


			if (strCmpConstStr(&attr, "else") != 0) // dalsi token je else
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then EOL else EOL
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (token != EOL)
			{		
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then statements EOL else EOL statements
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if ( (result = statements()) != SYNTAX_OK )
			{
				return result;
			}

			// nagenerujeme instrukci dalsiho navesti
			if (generateInstruction(I_LAB, NULL, NULL, NULL))
			{
				return ER_INT;
			}

			// navesti konce if, ulozime adresu navesti do addrOfLab1
			void *addrOfLab2;
			addrOfLab2 = listGetPointerLast(list); // vrati ukazatel na posledni instrukci
			listGoto(list, addrGoto); // nastavime aktiv. intrukci podle zadaneho ukazatele
			tInstr *dataInstr;
			dataInstr = listGetData(list);
			dataInstr->addr3 = addrOfLab2;  // a jako 3. adresu dame adresu navesti kam se bude skakat, po provedeni then

			if (strCmpConstStr(&attr, "end") != 0) // dalsi token je end
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then EOL else EOL statements end if
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "if") != 0) // musi byt if
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - if expr then EOL else EOL statements end if EOL
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (token != EOL)
			{		
				return ER_SYN;
			}

			return SYNTAX_OK;
		}

		else if ((strCmpConstStr(&attr, "do") == 0))
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // do while
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (strCmpConstStr(&attr, "while") != 0) 
			{
				return ER_SYN;
			}


			if ((token = getNextToken(&attr)) == ER_LEX) // do while expr
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}


			if (generateInstruction(I_LAB, NULL, NULL, NULL))
			{
				return ER_INT;
			}
			// navesti za while, ulozime adresu navesti do addrOfLab1
			void *addrOfLab1;
			addrOfLab1 = listGetPointerLast(list); // vrati ukazatel na posledni instrukci
 
			// nagenerujeme inikatni nazev promenne pro vysledek vyrazu a vlozime
			// do tabulky symbolu
			string newVar;
			if (strInit(&newVar))
			{
				return ER_INT;
			}
			if (generateVariable(&newVar))
			{
				return ER_INT;
			}
			// vlozime promennou, pravdepodobne typu int, moznnost pripadne pretypovat
			if (BSTInsert (TbSmBST, &newVar, TYPE_INT))
			{
				return ER_INT;
			}

			// ulozime si adresu s daty promenne
			tData *newVariableInfo;
			newVariableInfo = BSTSearch(TbSmBST, &newVar);
			// uvolnime nazev promenne
			strFree(&newVar);



			result = readExpr(&newVariableInfo); // volame PA (po konci PA bude token posunut na EOL)
      		if (result != SYNTAX_OK)
      		{
      			return result;
      		}

      		// pokud je podminka FALSE skacu na konec, jinak pokracuju
      		// nagenerujeme instrukci podmineneho skoku a ulozime jeji adresu,
      		// protoze pozdeji bude potreba doplnit adresu skoku, kam skakat
      		void *addrOfIfGoto;
      		if(generateInstruction(I_IFNOTGOTO, (void*) newVariableInfo, NULL, NULL))
        	{
        		return ER_INT;
        	}

      		addrOfIfGoto = listGetPointerLast(list);
 
      		if (token != EOL)
      		{
      			return ER_SYN;
      		}

      		if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - do while expr EOL statements
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if ( (result = statements()) != SYNTAX_OK ) // zpracujeme statements
			{
				return result;
			}

			// nagenerujeme instrukci skoku, ktera skoci na misto za while
      		// takze budeme cyklit
      		if(generateInstruction(I_GOTO, NULL, NULL, (void*) addrOfLab1))
      		{
        		return ER_INT;
      		}
 
      		// nagenerujeme instrukci druheho navesti
      		if(generateInstruction(I_LAB, NULL, NULL, NULL))
        	{
        		return ER_INT;
        	}
      		void *addrOfLab2;
      		addrOfLab2 = listGetPointerLast(list);// ulozime si ukazatel na posledni instr.
 
      		// jiz zname adresu druheho navesti, muzeme tedy nastavit adresu
      		// do drive generovane instrukce podmineneho skoku
      		// nastavime aktualni instrukci podle adresy instrukce ifnotgoto
    		listGoto(list, addrOfIfGoto);
   			tInstr *data;
      		data = listGetData(list);
      		data->addr3 = addrOfLab2;  // a jako 3. adresu dame adresu 2. navesti
      		//kam se bude skakat, pokud podminka nebude platit

      		if (strCmpConstStr(&attr, "loop") != 0) // token se nacetl pri zpracovani statements  - do while expr EOL statements loop
			{
				return ER_SYN;
			}

			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - do while expr EOL statements loop EOL
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if (token != EOL)
			{		
				return ER_SYN;
			}

			return SYNTAX_OK;
		}
		
		else if ((strCmpConstStr(&attr, "return") == 0))
		{

		}
		
	
		else
		{	
			return ER_SYN;
		}		
		

	}


}


// prirazeni do prom
int assignment(tData **var1,tData **var2)
{
	int ret;
	if ((ret = getExpr(var2)) != 0)
	{
		return ret;
	}
	if (generateInstruction(I_ASSIGN, *var1, *var2, NULL))
	{
		return ER_INT;
	}

	if ((token = getNextToken(&attr)) == ER_LEX) // EOL
	{
		return ER_LEX;
	}
	else if (token == ER_INT)
	{
		return ER_INT;
	}

	if (token != EOL)
	{
		return ER_SYN;
	}

	return 0;
}


//25.	<exlist> -> expr ; <exlist_n>
//26.	<exlist_n> -> expr ; <exlist_n>
//27.	<exlist_n> -> ε
int exprlist()
{
	if (token != EOL) // jeste neni konec
	{	
		tData *data;
		int ret;
		if ((ret = getExpr(data)) != 0) // zavolam PA
		{
			return ret;
		}

		if ((token = getNextToken(&attr)) == ER_LEX) // musi nasledovat ;
		{
			return ER_LEX;
		}
			else if (token == ER_INT)
		{
			return ER_INT;
		}

		if (token != SEMICOLON)
		{
			return ER_SYN;
		}

// TODO gen instrukci vypsani dat		

		if ((token = getNextToken(&attr)) == ER_LEX) // bud EOF nebo dalsi expr
		{
			return ER_LEX;
		}
			else if (token == ER_INT)
		{
			return ER_INT;
		}

		return exprlist();
	}
	else // token je EOL, koncime
	{
		return SYNTAX_OK;
		
	}
}


// 11.	<body> -> Scope <statements> end scope EOL 
int mainList()
{
	if (token == KEYWORD)
	{
		if ((strCmpConstStr(&attr, "scope") == 0))
		{
			if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - scope statements
			{
				return = ER_LEX;
			}
			else if (token  == ER_INT)
			{
				return ER_INT;
			}

			if ( (result = statements()) != SYNTAX_OK ) // zpracujeme statements
			{
				return result;
			}

			if ((strCmpConstStr(&attr, "end") == 0)) // statements posune token, ten musi byt end
			{
				if ((token = getNextToken(&attr)) == ER_LEX) // nacteme dalsi token  - scope statements end scope
				{
					return = ER_LEX;
				}
				else if (token  == ER_INT)
				{
					return ER_INT;
				}
				if ((strCmpConstStr(&attr, "scope") == 0))
				{
					// TODO 
					return SYNTAX_OK;

				}
				else
				{
					return ER_SYN;
				}


			}
			else
			{
				return ER_SYN;
			}



		}
	}

}


// 1.	<prog> -> <dcllist> <body>
int program()
{
	int result = 0;
	switch (token)
	{
		case KEYWORD:  
		if ((strCmpConstStr(&attr, "declare") == 0) || (strCmpConstStr(&attr, "function") == 0) || (strCmpConstStr(&attr, "scope") == 0))
		{
			result = declrList();
			if (result != SYNTAX_OK)
			{
				return result;
			}
			result = mainList();
			if (result != SYNTAX_OK)
			{
				return result;
			}
			/*
			if (token != tEOF) return ER_SYN;

			if(generateInstruction(I_STOP, NULL, NULL, NULL))
			{
				return ER_INT;
			}
*/
			return SYNTAX_OK;
		}
		break;

		default:
		return ER_SYN;
		break;
	}
	return ER_SYN;
}


// parser, bere na vstup tabulku symbolu a seznam instrukci
int parse(tBSTNodePtr *tb, tListOfInstr *instrList) 
{
	int result = 0;
	TbSmBST = tb;
	list = instrList;
	if (strInit(&attr))
	{
		return ER_INT;
	}
	if ((token = getNextToken(&attr)) == ER_LEX)
	{
		result = ER_LEX;
	}
	else if(token  == ER_INT)
	{
		return ER_INT;
	}
	else
	{
		result = program();
	}
	strFree(&attr);
	return result;
}

