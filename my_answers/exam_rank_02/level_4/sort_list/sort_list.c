#include <stdlib.h>
#include <stdio.h>

typedef struct s_list
{
    int data;
    struct s_list *next;
} t_list;

// Merges two sorted linked lists according to the comparison function
t_list *merge(t_list *left, t_list *right, int (*cmp)(int, int))
{
    // Create a dummy head to simplify edge cases
    t_list dummy;
    t_list *tail = &dummy;
    
    // Merge the two lists
    while (left && right)
    {
        // Compare the current nodes using the comparison function
        if (cmp(left->data, right->data))
        {
            tail->next = left;
            left = left->next;
        }
        else
        {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    
    // Attach the remaining nodes
    tail->next = left ? left : right;
    
    // Return the merged list (skip the dummy head)
    return dummy.next;
}

// Sorts a linked list using merge sort
t_list *sort_list(t_list *lst, int (*cmp)(int, int))
{
    // Base case: empty list or single element list is already sorted
    if (!lst || !lst->next)
        return lst;
    
    // Find the middle of the list using fast/slow pointer technique
    t_list *slow = lst;
    t_list *fast = lst->next;
    
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    // Split the list into two halves
    t_list *right = slow->next;
    slow->next = NULL;
    
    // Recursively sort both halves
    t_list *left_sorted = sort_list(lst, cmp);
    t_list *right_sorted = sort_list(right, cmp);
    
    // Merge the sorted halves and return the result
    return merge(left_sorted, right_sorted, cmp);
}

// Creates a new node with the given data
t_list *create_node(int data)
{
    t_list *node = malloc(sizeof(t_list));
    if (!node)
        return NULL;
    node->data = data;
    node->next = NULL;
    return node;
}

// Comparison function for ascending order
int ascending(int a, int b)
{
    return (a <= b);
}

// Comparison function for descending order
int descending(int a, int b)
{
    return (a >= b);
}

// Frees all nodes in a linked list
void free_list(t_list *head)
{
    t_list *temp;
    while (head)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Utility function to print a linked list
void print_list(t_list *head)
{
    while (head)
    {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");
}

int main(void)
{
    // Create a test list
    t_list *head = create_node(3);
    head->next = create_node(1);
    head->next->next = create_node(4);
    head->next->next->next = create_node(2);
    head->next->next->next->next = create_node(5);

    printf("Original list: ");
    print_list(head);

    // Sort in ascending order
    t_list *sorted_asc = sort_list(head, ascending);
    printf("Ascending order: ");
    print_list(sorted_asc);
    
    // Create another test list for descending sort
    t_list *head2 = create_node(3);
    head2->next = create_node(1);
    head2->next->next = create_node(4);
    head2->next->next->next = create_node(2);
    head2->next->next->next->next = create_node(5);
    
    // Sort in descending order
    t_list *sorted_desc = sort_list(head2, descending);
    printf("Descending order: ");
    print_list(sorted_desc);
    
    // Free memory
    free_list(sorted_asc);
    free_list(sorted_desc);
    
    return 0;
}