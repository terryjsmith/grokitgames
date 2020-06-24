
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
        m_values = (T*)malloc(sizeof(T) * m_size);
        memcpy(m_values, b.m_values, sizeof(T) * m_size);
    }
    
    ~Array() {
        free(m_values);
        m_size = 0;
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
            assert(m_position - b.m_position > 0);
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
        T* newarr = (T*)malloc(sizeof(T) * (m_size - 1));
        memcpy(newarr, m_values, sizeof(T) * pos.m_position);
        if(pos.m_position < (m_size - 1)) {
            memcpy(newarr + pos.m_position, m_values + (pos.m_position + 1), sizeof(T) * (m_size - pos.m_position - 1));
        }
        
        m_size--;
        
        if(m_values) {
            free(m_values);
        }
        
        if(m_size > 0) {
            m_values = newarr;
        }
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
        T* newarr = (T*)malloc(sizeof(T) * newsize);
        memcpy(newarr, m_values, sizeof(T) * start.m_position);
        if(end.m_position < (m_size - 1)) {
            memcpy(newarr + start.m_position, m_values + (end.m_position + 1), sizeof(T) * (newsize - start.m_position));
        }
        
        m_size = newsize;
        
        if(m_values) {
            free(m_values);
        }
        
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
        T* newarr = (T*)malloc(sizeof(T) * (m_size + 1));
        memcpy(newarr, m_values, sizeof(T) * m_size);
        newarr[m_size] = item;
        
        if(m_values)
            free(m_values);
        
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
        if(m_values) {
            free(m_values);
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
        T* newarr = (T*)malloc(size * sizeof(T));
        memcpy(newarr, m_values, sizeof(T) * (size < m_size ? size : m_size)); // Use the smaller of existing or new size
        m_size = size;
        
        if(m_values) {
            free(m_values);
        }
        
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
        m_values = (T*)malloc(sizeof(T) * m_size);
        memcpy(m_values, b.m_values, sizeof(T) * m_size);
        return(*this);
    }
    
protected:
    // Values
    T* m_values;
    
    // Size of array
    int m_size;
};

#endif
