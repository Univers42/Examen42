#include <iostream>


/**
in echo to the exercise of dangling_poiinter if we want corruptMemory pass by value work, to still reach 
back and mutate obj, we need shared ownership, not independant ownership, and that's a different tool tahtn rulle of three
the idiomatic way: replace the raw pointer with 
std::shared_ptr<int>.

*/

#include <memory>

class Bad {
    private:
        std::shared_ptr<int> data;
    public:
        Bad(int val) : data(std::make_shared<int>(val)) {

        }
        void setData(int val){ *data = val ;}
        void print() { std::cout << "Value: " << *data <<std::endl; }
};

void corruptMemory(Bad b){
    b.setData(999);
}

int main() {
    Bad obj(10);
    corruptMemory(obj);
    obj.print();
}