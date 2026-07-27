# include "box.hpp"
int main(void) {
    Box a(100);
    Box b = a;
    Box c(999);

    a.show();
    b.show();
    c.show();
    c.setData(2);
    return (0);
}

