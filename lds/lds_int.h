#ifndef LDS_INT_H
#define LDS_INT_H

class lds_int {
public:
    lds_int(int value) : b(value) {}
    inline lds_int operator=(int other) {b = other; return *this;}
    inline operator int() const {return b;}

protected:
    int b;
};

class lds_int_zero : public lds_int{
public:
    lds_int_zero() : lds_int(0) {}
    inline lds_int_zero operator=(int other) {b = other; return *this;}
};

#endif // LDS_INT_H
