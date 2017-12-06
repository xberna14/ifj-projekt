/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
 */

// VARIANTA BINARNI VYHLEDAVACI STROM
#include "symtable.h"
#include "scaner.h"

void symTableInit(symtab_elem_t *Root){
    *Root = NULL;
}

int symTableSearch(symtab_elem_t Root, elem_type typ, string nazov,char k){
    if(Root != NULL){
		if(Root->type == typ){
			nazov = Root->name;
			return TRUE;
		}else{
			if(Root->id > k){
				return symTableSearch(Root->lptr,typ,nazov,k);
			}else{
				return symTableSearch(Root->rptr,typ,nazov,k);
			}
		}
	}else{
		return FALSE;
	}
}

void symTableInsert(symtab_elem_t* Root, char k, string nazov, elem_type typ){
    if(*Root != NULL){
		if(*Root->id == k){
			*Root->type = typ;
			*Root->name = nazov;
		}else if(*Root->id < k){
			symTableInsert(&(*Root)->rptr,k,nazov,typ);
		}else{
			symTableInsert(&(*Root)->lptr,k,nazov,typ);
		}
	}else{
		symtab_elem_t tmp = malloc(sizeof(struct symtab_elem_t));
		if(tmp != NULL){
			tmp->id = k;
			tmp->lptr = NULL;
			tmp->rptr = NULL;
			tmp->name = nazov;
			tmp->type = typ;
			*Root = tmp;
		}
	}
}

void symDeleteTree(symtab_elem_t *Root){
    if(*Root != NULL){
		symDeleteTree(&(*Root)->rptr);
		symDeleteTree(&(*Root)->lptr);
		free(*Root);
		*Root = NULL;
	}
}
