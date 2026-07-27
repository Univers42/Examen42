#include <iostream>

// 1) Might legitimately point at "nothing", and caller must be able to test that
//    -> POINTER (a reference can never be null, so it can't express "not found")
int *findFirstNegative(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] < 0)
            return &arr[i];
    }
    return nullptr;
}

// 2) Target is guaranteed valid (caller's contract: index is in range)
//    -> REFERENCE (no null-check noise, used exactly like a plain int)
int &getElement(int *arr, int index) {
    return arr[index];
}

// 3) Modify the caller's variables: reference is the idiomatic C++ way,
//    pointer is the older C way. Same job, compare the call sites in main().
void swapByReference(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
}

void swapByPointer(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// 4) Must be reseated (walk to the next node) and must be nullable (detect
//    the end of the chain) -> only a POINTER can do both of those.
struct Node {
    int value;
    Node *next;
};

void printList(Node *head) {
    Node *current = head;
    while (current != nullptr) {
        std::cout << current->value << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

int main() {
    int values[] = {4, 7, -3, 9, -1};
    int size = 5;

    int *neg = findFirstNegative(values, size);
    if (neg)
        std::cout << "first negative: " << *neg << std::endl;
    else
        std::cout << "no negative found" << std::endl;

    int &third = getElement(values, 2);
    std::cout << "element at index 2: " << third << std::endl;
    third = 100;
    std::cout << "values[2] after edit through reference: " << values[2] << std::endl;

    int x = 1, y = 2;
    swapByReference(x, y);
    std::cout << "after swapByReference: x=" << x << " y=" << y << std::endl;
    swapByPointer(&x, &y);
    std::cout << "after swapByPointer:   x=" << x << " y=" << y << std::endl;

    Node c = {3, nullptr};
    Node b = {2, &c};
    Node a = {1, &b};
    printList(&a);

    return (0);
}
