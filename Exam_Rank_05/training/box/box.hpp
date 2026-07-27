#ifndef BOX_H
# define BOX_H

class Box {
    private:
        int* value;

    public:
        Box(int val);
        Box(const Box& other);
        Box& operator=(const Box& other);
        ~Box();
        void    setData(int data) {
            *value = data;
        }
        void show() const;
};

#endif