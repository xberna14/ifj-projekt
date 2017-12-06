/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "listinstr.h"
#include "parser.h"
#include "string.h"
#include "symtable.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Chyba: Špatné argumenty!");
		return EXIT_FAILURE;
	}

	if ((source = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "Chyba: Soubor se nepodařilo otevřít!");
		return EXIT_FAILURE;
	}

	tListOfInstr IL;
	listInit(&IL);

	int error = 0;
	// error = parse(tabsymbolu ,IL);

}
