#ifndef CYCLE_H_INCLUDED
#define CYCLE_H_INCLUDED

#include <vector>

template<typename T>
class Cycle {
private:
    std::vector<T> data;
    std::size_t ptr;
public:
    Cycle();
    ~Cycle();
    T& curr() const;
    T& prev() const;
    T& next() const;
    std::size_t size() const;
    void push_back(const T& val);
    void pop_back();
    void pop_curr();
    // Does not guarantee elements will remain in same order
    void remove(std::size_t index);
    // slower than remove but guarantees remaining elements in same order
    void stable_remove(std::size_t index);
    void advance();
    void retreat();
};

template<typename T>
Cycle<T>::Cycle() {
}

template<typename T>
Cycle<T>::~Cycle() {
    data.clear();
    ptr = 0;
}

template<typename T>
T& Cycle<T>::curr() const {
    return data[ptr];
}

template<typename T>
T& Cycle<T>::prev() const {
    return data[(ptr-1)%data.size()];
}

template<typename T>
T& Cycle<T>::next() const {
    return data[(ptr+1)%data.size()];
}

template<typename T>
std::size_t Cycle<T>::size() const {
    return data.size();
}

template<typename T>
void Cycle<T>::push_back(const T& val) {
    data.push_back(val);
}

template<typename T>
void Cycle<T>::pop_back() {
    data.pop_back();
    ptr = ptr%data.size();
}

template<typename T>
void Cycle<T>::pop_curr() {
    remove(ptr);
}

template<typename T>
void Cycle<T>::remove(std::size_t index) {
    T temp = data[index];
    data[index] = data.back();
    data[data.size()-1] = temp;
    pop_back();
}

template<typename T>
void Cycle<T>::stable_remove(std::size_t index) {
    data.remove(data.begin() + index);
    ptr = ptr%data.size();
}

template<typename T>
void Cycle<T>::advance() {
    ptr = (ptr+1)%data.size();
}

template<typename T>
void Cycle<T>::retreat() {
    ptr = (ptr-1)%data.size();
}

#endif // CYCLE_H_INCLUDED