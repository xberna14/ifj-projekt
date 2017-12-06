/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */

typedef enum {
    t_function;
    t_ID;
} elem_type;

typedef struct symtab_elem_t {
    string name;
    char id;
    type_t type;
    struct symtab_elem_t *lptr;
    struct symtab_elem_t *rptr;
} symtab_elem_t;

void symTableInit(symtab_elem_t *Root);
int symTableSearch(symtab_elem_t Root, elem_type typ, string nazov,char k);
void symTableInsert(symtab_elem_t* Root, char k, string nazov, elem_type typ);
void symDeleteTree(symtab_elem_t *Root);
