/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */

/*
**navratove hodnoty ktore treba dat do tabulky :
*	 PLUS
     MINUS
     LESS_EQ
     LESS
     GREAT_EQ
     GREAT
     N_EQUAL
     ASSIGN
     EQUAL
     DIV
     MULTIPLY_EQ
     MULTIPLY
     scaner : Matej Havlas(xhavla06)
     		  Radoslav Bernath(x)
*/




#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

FILE *source;
int Token;

typedef struct Token_t
{
    int Token;
    char *attr;
    struct Token_t *next;
}

Token_t;
Token_t *tokenattr;

void setSourceFile(FILE *f)
{
  source = f;
}
void returnToken(int Token, char *attr)
 {
    Token_t *temp = malloc(sizeof(Token_t));

    if (temp == NULL)
        return;

    temp->Token = Token;
    temp->attr = attr;
    temp->next = tokenattr;
}
    int getNextToken(string  *attr) {
    int result;

    if (tokenattr == NULL) {
        string str;
        strInit(&str);
        result = lexer(&str);
        *attr = str.str;
        return result;
    }

    result = tokenattr->token;
    *attr = tokenattr->attr;

    token_t *temp = tokenattr;
    tokenattr = tokenattr->next;
    free(temp);

    return result;
tokenattr = temp;
int lexer(string *attr) {
    int state = 0; // stav automatu
    int c; // promenna pro znak
    char a [4]; // promenna pomocna pro zadani retezce pomoci hex cisla
    a[3] = '\0';

    int num_count = 0;
    int E_count = 0;
    int id_count = 0;

    int dbl_dot_count = 0; // promenna pro kontrolu tecky u doublu
    int dot_count = 0; // promenna pro kontrolu spravnosti plne kvalifikovaneho identifikatoru
    int quote_count = 0; // promenna pro signalizaci pouziti znaku " uvnitr stringu
    int sign_count = 0; // promenna pro signalizaci pouziti nepovinneho znamenka u desetinneho exponentu
    int star_count = 0; // promenna pro signalizaci spravneho ukonceni blokoveho komentare

    int plus_count = 0; // kontrola jestli je na vstupu +
    int minus_count = 0; // kontrola jestli je na vstupu -
    int less_count = 0; // kontrolla jestli je na vstupu <
    int great_count = 0; // kontrola jestli je na vstupu >
    int excl_count = 0;  // kontrola jestli je na vstupu !
    int eq_count   = 0; // kontrola jestli je na vstupu =
    int quote_count = 0;// kontrola ci je na vstupe /
    int mul_count = 0; //kontrola ci je na vstupe *

    strClear(attr); // vymazat soucasny obsah stringu

    while(1){ // cyklus nacitani znaku

   if( (c = getc(source)) == EOF ) {return END_OF_FILE; break;} // nacteni znaku ze souboru

    switch (state){ // automat pro zpracovani znaku ze vstupu

    case 0: // POCATECNI STAV

      if (isspace(c)) state = 0; // ignoruj bila mista
      else if (c == '\'') {state = 3;}
      else if (c == '/') { quote_count = 1; state = 8; }// bude bud komentar a nebo se jedna o operator deleni, nejdriv zkontrolovat jestli je to deleni ve stavu 8
      else if (c == '-') { minus_count = 1; state = 3; }//bud je to minus alebo udentifikator

      else if (isalpha(c) || c == '_' || c == '$' || c== '&' || c=='%'){ // jedna se o indentifikator nebo klicove slovo

       // printf("jetoznak jdu do 3\n"); // TEST

	    strAddChar(attr, c); // nahraj znak do struktury
	    state = 3; // preskoc do casu kde se budou resit identifikatory atd.
	 }

	 else if (c == '"'){ // jedna se o zacatek retezce

        state = 4; // preskoc do stavu pro zpracovani retezce
	 }

	 else if (isdigit(c)){ // jedna se o int nebo double

        strAddChar(attr, c);
        state = 5;
	 }                    // OSTATNI ZNAKY

	 else if (c == '{') {  return LEFT_VINCULUM; }
	 else if (c == '}') {  return RIGHT_VINCULUM; }
	 else if (c == '(') {  return LEFT_BRACKET; }
	 else if (c == ')') {  return RIGHT_BRACKET; }
	 else if (c == ';') {  return SEMICOLON; }
	 else if (c == ',') {  return COMMA; }

     else if (c == '*') { mul_count = 1; state = 8; }
	 else if (c == '+') { plus_count = 1; state = 8;}
	 else if (c == '-') { minus_count = 1; state = 8;}
	 else if (c == '<') { less_count = 1; state = 8;}
	 else if (c == '>') { great_count = 1; state = 8;}
	 else if (c == '=') { eq_count = 1; state = 8;}
	 else if (c == '!') { excl_count = 1; state = 8;}
	 else return ER_LEX;
     break;

    case 1: // RADKOVY KOMENTAR

        if (c != '\'') state = 1;  // jednoradkovy komentar, zustan tu a ignoruj ho

        if (c == '\n') state = 0; // konec radku a teda i komentare ???

        break;

    case 2: // BLOKOVY KOMENTAR

        if (c == '/' && quote_count = 1 ) { state = 1;} // netusim ako zapisat' ascii je 39

        else if (c == '/' && quote_count = 3 ) { quote_count = 0; state = 0;} // konec blokoveho komentare, vrat se na zacatek do nuly

        else if (c == EOF) {return ER_LEX;} // Testuj neukonceny komentar???!!

        break;

    case 3: // IDENTIFIKATORY, KLICOVA SLOVA

       if (c == '-' && !isspace(c)){ minus_count = 1; state = 8;} // ak nieje medzera je to identifikator
       if (c == '*' && !isspace(c)){ mul_count = 1; state = 8; }//to iste ale pre hviezdu
	strAddChar(attr, c); // dokud se jedna o identifikator nebo klicove slovo, naplnuj strukturu

        state = 3; // zustan tady a res identifikatory a klicova slova

        }

	    else {// struktura naplnena, nasleduje prazdne misto nebo nepovoleny znak nebo zacatek zavorky

        if (!isspace(c) && !isalnum(c) && c != '_' && c != '$' && c != '(' && c!= ')' && c!= '{' && c!= '}' && c!= '=' && c!= '+' && c!= '-' && c!= '*' && c!= '/' && c!= '<' && c!= '>' && c!='!' && c!= ';' && c != ',') { return ER_LEX; break; } // pokud se neobjevi prazdne misto nebo zavorky nebo operatory ale nejaky nepovoleny znak je to error

        ungetc(c, source); // POZOR! Je potreba vratit posledni nacteny znak

        //IDENTIFIKATORY
	    if (strCmpConstStr(attr, "As") == 0) { return AS;}
   else if (strCmpConstStr(attr, "Asc") == 0) { return ASC; }
   else if (strCmpConstStr(attr, "Declare") == 0) { return DECLARE;}
   else if (strCmpConstStr(attr, "Dim") == 0) { return DIM;}
   else if (strCmpConstStr(attr, "Do") == 0) { return DO; }
   else if (strCmpConstStr(attr, "Double") == 0) { return DOUBLE;}
   else if (strCmpConstStr(attr, "Else") == 0) { return ELSE;}
   else if (strCmpConstStr(attr, "End") == 0) {return END;}
   else if (strCmpConstStr(attr, "Chr") == 0) {return CHR;}
   else if (strCmpConstStr(attr, "Function") == 0) {return FUNCTION;}
   else if (strCmpConstStr(attr, "If") == 0) {return IF;}
   else if (strCmpConstStr(attr, "Input") == 0) {return INPUT;}
   else if (strCmpConstStr(attr, "Integer") == 0) {return INTEGER;}
   else if (strCmpConstStr(attr, "Length") == 0) {return LENGTH;}
   else if (strCmpConstStr(attr, "Loop") == 0) {return LOOP;}
   else if (strCmpConstStr(attr, "Print") == 0) {return PRINT;}
   else if (strCmpConstStr(attr, "Return") == 0) {return RETURN;}
   else if (strCmpConstStr(attr, "Scope") == 0) { return SCOPE;}
   else if (strCmpConstStr(attr, "String") == 0) {return STRING; }
   else if (strCmpConstStr(attr, "SubStr") == 0) {return SUBSTR;}
   else if (strCmpConstStr(attr, "Then") == 0) {return THEN;}
   else if (strCmpConstStr(attr, "While") == 0) {return WHILE;}
   //klicova slova
   else if (strCmpConstStr(attr, "And") == 0) {return AND;}
   else if (strCmpConstStr(attr, "Boolean") == 0) {return BOOLEAN;}
   else if (strCmpConstStr(attr, "Continue") == 0) {return CONTINUE;}
   else if (strCmpConstStr(attr, "Elseif") == 0) {return ELSEIF ;}
   else if (strCmpConstStr(attr, "Exit") == 0) {return EXIT;}
   else if (strCmpConstStr(attr, "False") == 0) {return FALSE;}
   else if (strCmpConstStr(attr, "For") == 0) {return FOR;}
   else if (strCmpConstStr(attr, "Next") == 0) { return NEXT;}
   else if (strCmpConstStr(attr, "Not") == 0) {return NOT; }
   else if (strCmpConstStr(attr, "And") == 0) {return AND;}
   else if (strCmpConstStr(attr, "Or") == 0) {return OR;}
   else if (strCmpConstStr(attr, "Shared") == 0) {return SHARED;}
   else if (strCmpConstStr(attr, "Static") == 0) {return STATIC;}
   else if (strCmpConstStr(attr, "True") == 0) {return TRUE;}





	    else {return ID;}
        } break;

    case 4: // RETEZCOVY LITERAL

         if (c != '"' && c!= '\x5C' && quote_count == 0 && c != '\n'){ // dokud sme v retezci a nejsou pouzity specialni znaky jako \n \" a nema nasledovat neco za spec znakem

         strAddChar(attr, c); // tak normalne naplnuj strukturu

         //quote_count = 0; // radsi furt nuluj quote_count jelikoz se nejedna o specialni znak

         state = 4; // a zustan tady
        }

        else if (c == '\x5C' && quote_count == 0){ // bude nasledovat specialni znak

         quote_count = 1; // signalizuj ze se bude jedna o nejaky spec. znak

         state = 4; // a zustan tady

        }

        else if (c == 'n' && quote_count == 1) { // \n preved na new line

        c = '\x0A';

        strAddChar(attr, c);

        quote_count = 0;

		state = 4;
	}

        else if (c == '"' && quote_count == 1){ // jedna se o uvozovky uvnitr stringu NE signalizujici jeho konec

         strAddChar(attr, c); // hod je do struktury

         quote_count = 0;

         state = 4; // a zustan tady

        }

        else if ( c == 't' && quote_count == 1){ // jedna se o tab

        c = '\x09';

        strAddChar(attr, c);

        quote_count = 0;

		state = 4;

        }

        else if ( c == '\x5C' && quote_count == 1){ // jedna se o  \ uvnitr stringu

        strAddChar(attr, c);

        quote_count = 0;

		state = 4;

        }

	else if (c == '\n' && quote_count == 0){

            return ER_LEX; break;
        }

       else if (!isdigit(c) && c != '\x5C' && c != 't' && c != '"' && c != 'n' && quote_count == 1){ // cokoliv jineho  \a, \b atd bude zamitnuto

        return ER_LEX; break;

        }

        else if (isdigit(c) && quote_count == 1) { // znak zadany pomoci \xxx

	a[0] = c;

	quote_count = 0;

	num_count = 1; // prislo prvni cislo

	state = 9;

	}

        else if (c == '"' && quote_count == 0){ // sme na konci retezce

         return STRING_LITERAL; // vrat retezcovy literal
        } break;

    case 5: // CELOCISELNY LITERAL

        if (isdigit(c)){ // pokud prichazi cislo

         strAddChar(attr, c); // pln strukturu

         state = 5; // a zustan tady
        }

        else if (!isdigit(c) && c != ';' && c != '.' && c != 'e' && c != 'E' && !isspace(c) && c != ')' && c != '+' && c != '-' && c != '/' && c != '*' && c != '!' && c != ',' && c != '=' && c != '<' && c != '>'){ // pokud nasleduje znak jiny nez ; ktery signalizuje konec zadavani literalu nebo tecka signalizujici des. cislo nebo exponent, je to error

          return ER_LEX; break;
        }

        else if (isspace(c) || c == ')' || c == ',' || c == ';' || c == '+' || c == '-' || c == '/' || c == '*' || c == '!' || c == '=' || c == '<' || c == '>'){

          ungetc(c, source); // konec celeho cisla, vracime ; nebo volny zpatky, zpracujem pak

	  long int int_control = atoi(attr->str);

          if (int_control > INT_MAX) {return ER_LEX; break;}

          return INT_LITERAL; // a vrati se celociselny literal

        }



        else if(c == '.') { // bude se jednat o desetinny literal ve kterem se nachazi desetinna cast

         dbl_dot_count = 1;
         strAddChar(attr, c);
         state = 6;

        }

        else if(c == 'e' || c == 'E') { // bude se jednat o desetinny literal ve kterem NENI desetinna cast ale pouze exponent

         strAddChar(attr, c);

         E_count = 1;

         state = 7;

        }
        break;

    case 6: // DESETINNY LITERAL

        if (isdigit(c) && dbl_dot_count == 1){ // pokud prichazi cislo

         dbl_dot_count = 0;

         strAddChar(attr, c); // pln strukturu

         state = 6; // a zustan tady
        }

        else if (isdigit(c) && dbl_dot_count == 0){

         strAddChar(attr, c); // pln strukturu

         state = 6; // a zustan tady
        }

        else if (dbl_dot_count == 1 && !isdigit(c)){ // po tecce musi nasledovat cislo

            return ER_LEX;
        }

        else if (c == 'e' || c == 'E'){ // nasleduje exponent

         strAddChar(attr, c);

         E_count = 1;

         state = 7;
        }

        else if (!isdigit(c) && c != ';' && c != 'e' && c != 'E' && !isspace(c) && c != ')' && c != '+' && c != '-' && c != '/' && c != '*' && c != '!' && c != ',' && c != '=' && c != '<' && c != '>'){ // pokud nasleduje znak jiny nez ; ktery signalizuje konec zadavani literalu nebo tecka signalizujici des. cislo nebo exponent, je to error

          return ER_LEX; break;
        }

        else if (isspace(c) || c == ')' || c == ',' || c == ';' || c == '+' || c == '-' || c == '/' || c == '*' || c == '!' || c == '=' || c == '<' || c == '>'){

          ungetc(c, source); // konec celeho cisla, vracime ; nebo volny zpatky, zpracujem pak

          return DOUBLE_LITERAL; // a vrati se celociselny literal

        }

        break;

    case 7: // DESETINNY LITERAL S EXPONENTEM

        if ((c == '+' || c == '-') && E_count == 1 ){ // nasleduje nepovinne znamenko

         E_count = 0;

         sign_count = 1; // bylo pouzito nepovinne znamenko

         strAddChar(attr, c); // uloz ho do struktury

         state = 7; // a zustan tady
        }

        else if (!isdigit(c) && sign_count == 1){

           return ER_LEX; break; // za nepovinnym znamenkem nenasledovala neprazdna posloupnost cislic
        }

        else if (isdigit(c) && sign_count == 1){

         sign_count = 0; // bylo pouzito nepovinne znamenko

         strAddChar(attr, c); // uloz ho do struktury

         state = 7; // a zustan tady
        }

        else if ((c != '+' || c != '-') && !isdigit(c) && E_count == 1){ // hned po exponentu nenasleduje cislo nebo nepovinne znamenko

        return ER_LEX; break;

        }

        else if (isdigit(c) && E_count == 1 ) { // nebylo vyuzito nepovinne znamenko

         strAddChar(attr, c); // uloz ho do struktury

         E_count = 0;

         state = 7; // a zustan tady
        }

        else if (isdigit(c) && E_count == 0 ) { // neprazdna posloupnost cislic

         strAddChar(attr, c); // uloz ho do struktury

         state = 7; // a zustan tady
        }

        else if (!isdigit(c) && c != ';' && !isspace(c) && c != ')' && c != '+' && c != '-' && c != '/' && c != '*' && c != '!' && c != ',' && c != '=' && c != '<' && c != '>'){ // pokud nasleduje znak jiny nez ; ktery signalizuje konec zadavani literalu nebo tecka signalizujici des. cislo nebo exponent, je to error

          return ER_LEX; break;
        }

        else if (isspace(c) || c == ')' || c == ',' || c == ';' || c == '+' || c == '-' || c == '/' || c == '*' || c == '!' || c == '=' || c == '<' || c == '>'){

          ungetc(c, source); // konec celeho cisla, vracime ; nebo volny zpatky, zpracujem pak

          return DOUBLE_LITERAL; // a vrati se celociselny literal

        } break;

    case 8: // OPERATORY, SLOZENE OPERATORY, ROZPOZNAVANI ZACATKU KOMENTARU

     if (plus_count == 1 ) {ungetc(c, source); return PLUS;} // vrat neplatny znak, je to plus
     else if (minus_count == 1) {ungetc(c, source);  return MINUS;} // vrat neplatny znak, je to minus
     else if (less_count == 1 && c == '=')  { return LESS_EQ;}
     else if (less_count == 1 && c != '=')  {ungetc(c, source); return LESS; } // vrat neplatny znak, je to mensi nez
     else if (great_count == 1 && c == '=') {return GREAT_EQ;}
     else if (great_count == 1 && c != '=') {ungetc(c, source); return GREAT; } // vrat neplatny znak, je to vetsi nez
     else if (excl_count == 1 && c == '=')  {return N_EQUAL;}
     else if (eq_count == 1 && c != '=')    {ungetc(c, source); return ASSIGN; } // vrat neplatny znak, je to rovnitko
     else if (eq_count == 1 && c == '=')    {return EQUAL;} // vrat operator ==
     else if (quote_count == 1 && c != '/') {ungetc(c, source); return DIV; } // nejedna se o komentar ale o operator deleni
     else if (mul_count ==1 && c == '=')	{ return MULTIPLY_EQ} //vrat nasobenie ako vysledok
     else if (mul_count ==1 && c != '=')	{ungetc(c, source);return MULTIPLY;}	//vrat nasobenie do pola
     else if (quote_count == 1 && c == '/') {state = 2;} // jedna se o blokovy komentar
     else return ER_LEX;
     break;
  } // konec switche
 } // konec while
} // konec funkce
