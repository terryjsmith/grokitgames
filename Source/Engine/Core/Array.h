
#ifndef array_h
#define array_h

#include <Core/Giga.h>

/**
 * Drop in replacement for Array
 */
template<class T>
class GIGA_API Array {
public:
    Array() {
        m_size = 0;
        m_values = 0;
    }
    
    Array(const Array &b) {
        m_size = b.m_size;
        m_values = 0;
        
        if(m_size == 0) return;
        m_values = new T[m_size];
        for(int i = 0; i < m_size; i++) {
            m_values[i] = b.m_values[i];
        }
    }
    
    ~Array() {
        if(m_size) {
            delete[] m_values;
            m_values = 0;
            m_size = 0;
        }
    }
    
    /**
     * Built-in iterator between array elements
     */
    class Iterator {
    public:
        Iterator() {
            m_position = 0;
            m_size = 0;
            m_start = 0;
        }
        
        /**
         * Operators
         */
        Iterator operator++(int) {
            m_position++;
            return(*this);
        }
        
        Iterator operator--(int) {
            m_position--;
            return(*this);
        }
        
        Iterator operator+(const int &b) const {
            assert(m_position + b < m_size);
            
            Iterator it;
            it.m_position = m_position + b;
            it.m_start = m_start;
            it.m_size = m_size;
            return(it);
        }
        
        Iterator operator-(const int &b) const {
            assert(m_position - b > 0);
            
            Iterator it;
            it.m_position = m_position - b;
            it.m_start = m_start;
            it.m_size = m_size;
            return(it);
        }
        
        int operator+(const Iterator &b) const {
            assert(m_position + b.m_position < m_size);
            return(m_position + b.m_position);
        }
        
        int operator-(const Iterator &b) const {
            assert(m_position - b.m_position >= 0);
            return(m_position - b.m_position);
        }
        
        bool operator ==(const Iterator &b) const {
            if(m_position == b.m_position) {
                return(true);
            }
            
            return(false);
        }
        
        bool operator !=(const Iterator &b) const {
            if(m_position != b.m_position) {
                return(true);
            }
            
            return(false);
        }
        
        T& operator*() {
            assert(m_position < m_size);
            assert(m_position >= 0);
            
            return(m_start[m_position]);
        }
        
        friend class Array<T>;
        
    protected:
        // Current position / offset into array
        int m_position;
        
        // Size of array
        int m_size;
        
        // Start of array
        T* m_start;
    };
    
    /**
     * Create an iterator
     */
    Iterator begin() {
        Iterator it;
        it.m_position = 0;
        it.m_start = m_values;
        it.m_size = m_size;
        return(it);
    }
    
    /**
     * Create an end iterator (points to size)
     */
    Iterator end() {
        Iterator it;
        it.m_position = m_size;
        it.m_start = m_values;
        it.m_size = m_size;
        return(it);
    }
    
    /**
     * Remove an element
     */
    void erase(Iterator pos) {
        this->erase(pos, pos);
    }
    
    /**
     * Remove range of elements
     */
    void erase(Iterator start, Iterator end) {
        // Calculate removed item count (inclusive of start and end)
        int count = end - start + 1;
        
        // Determine the new size
        int newsize = m_size - count;
        
        // Create new data and copy
        T* newarr = new T[newsize];
        for(int i = 0; i < start.m_position; i++) {
            newarr[i] = m_values[i];
        }
        
        for(int i = end.m_position + 1; i < m_size; i++) {
            int index = i - end.m_position + start.m_position - 1;
            newarr[index] = m_values[i];
        }
        
        if(m_size) {
            delete[] m_values;
        }
        
        m_size = newsize;
        if(m_size > 0) {
            m_values = newarr;
        }
    }
    
    /**
     * Get size of array
     */
    int size() { return m_size; }
    
    /**
     * Insert items
     */
    void push_back(T item) {
        T* newarr = new T[m_size + 1];
        for(int i = 0; i < m_size; i++) {
            newarr[i] = m_values[i];
        }
        newarr[m_size] = item;
        
        if(m_size)
            delete[] m_values;
        
        m_values = newarr;
        m_size++;
    }
    
    /**
     * Insert a value in front of another value
     */
    void insert(T item, Iterator pos) {
        T* newarr = new T[m_size + 1];
        for(int i = 0; i < pos.m_position; i++) {
            newarr[i] = m_values[i];
        }
        
        newarr[pos.m_position] = item;
        
        for(int i = pos.m_position + 1; i <= m_size; i++) {
            newarr[i] = m_values[i - 1];
        }
        
        if(m_size)
            delete[] m_values;
        
        m_values = newarr;
        m_size++;
    }
    
    /**
     * Whether the array is empty
     */
    bool empty() { return(m_size == 0); }
    
    /**
     * Clear contents
     */
    void clear() {
        if(m_size) {
            delete[] m_values;
            m_values = 0;
        }
        
        m_size = 0;
    }
    
    /**
     * Find an object
     */
    Iterator find(T value) {
        Iterator it = this->begin();
        for(; it != this->end(); it++) {
            if((*it) == value) break;
        }
        
        return(it);
    }
    
    /**
     * Get data pointer
     */
    T* data() { return m_values; }
    
    /**
     * Resize buffer
     */
    void resize(int size) {
        T* newarr = new T[size];
        
        int copysize = size < m_size ? size : m_size;
        for(int i = 0; i < copysize; i++) {
            newarr[i] = m_values[i];
        }

        if(m_size) {
            delete[] m_values;
        }
        
        m_size = size;
        m_values = newarr;
    }
    
    /**
     * Operators
     */
    T& operator[](std::size_t idx) {
        return m_values[idx];
    }
    
    const T& operator[](std::size_t idx) const {
        return m_values[idx];
    }
    
    Array& operator =(const Array& b) {
        m_size = b.m_size;
        m_values = 0;
        
        if(m_size == 0) return(*this);
        m_values = new T[m_size];
        for(int i = 0; i < m_size; i++) {
            m_values[i] = b.m_values[i];
        }
        return(*this);
    }
    
protected:
    // Values
    T* m_values;
    
    // Size of array
    int m_size;
};

#endif
