#include <iostream>

class Bad {
    private:
        int *data;
    public:
    Bad(int val){
        data = new int(val);
    }
    
    ~Bad(){
        delete data;
    }
    void print(){
        std::cout << "data:" << *data << std::endl;
    }
};

int main(void){
    Bad a(42);

    a.print();
    return (0);
}