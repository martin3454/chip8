#include "common.h"


template <typename T>
class Stack{
private:  
        T* stack_array;
        uint8_t top;
        uint8_t capacity;
        uint8_t size;
public:
     explicit Stack(int capacity = 16);
     ~Stack();
     Stack(const Stack &st);
     Stack& operator=(const Stack &st);
     void push(T item);
     Option<T> pop();
};

template<typename T>
Stack<T>::Stack(int capacity) : top(0), capacity(capacity), size(0){
    stack_array = new T[capacity];
}

template<typename T>
Stack<T>::~Stack(){ 
  delete [] stack_array; 
}

template<typename T>
Stack<T>::Stack(const Stack &st){
  *this = st;
}

template<typename T>
void Stack<T>::push(T item){
  if(size < capacity){
    stack_array[top++] = item;
    size++;
  }else Serial.println("stack full");
}

template<typename T>
Option<T> Stack<T>::pop(){
  Option<T> op;
  if(size > 0){
    size--;
    op.val = stack_array[--top];
    op.je = true; 
  }
  return op; 
}

template<typename T>
Stack<T>& Stack<T>::operator=(const Stack &st){
  if(this == &st) return *this;

  this->~Stack();
  top = st.top;
  capacity = st.capacity;
  size = st.size;
  stack_array = new T[capacity];
  for(uint8_t i = 0; i < size; i++){
    stack_array[i] = st.stack_array[i];
  } 
  return *this;
}
