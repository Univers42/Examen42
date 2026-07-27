#include <iostream>

class Bad {
    private:
        int *value;
    public:
        Bad(int data) {
            value = new int(data);
        }
        Bad(const Bad& other) {
            value = new int(*other.value);
        }
        Bad &operator=(const Bad& other){
            if (this != &other)
                *value = *other.value;
            return (*this);
        }
        ~Bad() {
            delete value;
        }
        void setData(int data) {
            *value = data;
        }
        void print() {
            std::cout << "value:" << *value << std::endl;
        }
};

void    corruptMemory(Bad b, int data) {
    b.setData(data);
}

int main(void) {
    Bad a(50);
    corruptMemory(a, 999);
    a.print();
    return (0);
}
