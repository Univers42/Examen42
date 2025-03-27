/*
Assignment name  : ft_list_remove_if
Expected files   : ft_list_remove_if.c
Allowed functions: free
--------------------------------------------------------------------------------

Write a function called ft_list_remove_if that removes from the
passed list any element the data of which is "equal" to the reference data.

It will be declared as follows :

void ft_list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)());

cmp takes two void* and returns 0 when both parameters are equal.

You have to use the ft_list.h file, which will contain:

$>cat ft_list.h
typedef struct      s_list
{
    struct s_list   *next;
    void            *data;
}                   t_list;
$>
*/
#include <stdlib.h>
#include "ft_list.h"

void ft_list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)())
{
	if (begin_list == NULL || *begin_list == NULL) //null check 
		return;

	t_list *aux = *begin_list; //aux var

	if (cmp(aux->data, data_ref) == 0) //compara si es igual 
	{
		*begin_list = aux->next;//pasa begin list al siguiente 
		free(aux);//elimina el actual 
		ft_list_remove_if(begin_list, data_ref, cmp);//reinicia
	}
	else // if there is a no else, you cant pass the Moulinette, tryed 2023.09.08
		ft_list_remove_if(&aux->next, data_ref, cmp); //next node
}

/*
Original:
+----+----+----+----+
| 10 | 20 | 30 | 40 |
+----+----+----+----+
  |    |    |    |
  v    v    v    v
 NULL NULL NULL NULL

Después de eliminar el nodo con 20:
+----+----+----+
| 10 | 30 | 40 |
+----+----+----+
  |    |    |
  v    v    v
 NULL NULL NULL
*/