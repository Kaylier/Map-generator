/*
 * like std::vector, but every elements are uniques, and you can
 * reverse lookup easily, and pop any element just by its value.
 * TODO: l'implémentation de lookup_vector
 */

#ifndef LOOKUP_VECTOR_HPP
#define LOOKUP_VECTOR_HPP

#include <cstddef> /* size_t */
#include <functionnal> /* equal_to */
#include <memory> /* std::allocator_traits */
#include <iterator> /* std::iterator */
#include <initializer_list> /* std::initializer_list */

#include <vector>

template <class T, 
          class Pred = equal_to<T>, 
          class Alloc = std::allocator<T> >
class lookup_vector
{
public:
    using value_type = T;
    using value_equal = Pred;
    using allocator_type = Alloc;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
    using iterator = class std::iterator<std::random_access_iterator_tag, value_type>;
    using const_iterator = class std::iterator<std::random_access_iterator_tag, const value_type>;
    using reverse_iterator = typename std::reverse_iterator<iterator>;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
    using difference_type = typename std::iterator_traits<iterator>::difference_type;
    using size_type = size_t;

public:
    /* (constructor) default */
    lookup_vector();
    explicit lookup_vector(const allocator_type& alloc);
    /* (constructor) fill */
    explicit lookup_vector(size_type n, const allocator_type& alloc = allocator_type());
    lookup_vector(size_type n, const value_type& val, const allocator_type& alloc = allocator_type());
    /* (constructor) range */
    template <class InputIterator>
        lookup_vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type());
    /* (constructor) copy */
    lookup_vector(const lookup_vector& x);
    lookup_vector(const lookup_vector& x, const allocator_type& alloc);
    /* (constructor) move */
    lookup_vector(lookup_vector&& x);
    lookup_vector(lookup_vector&& x, const allocator_type& alloc);
    /* (constructor) initializer list */
    lookup_vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type());
    /* (destructor) */
    ~lookup_vector();
    /* (affect) copy */
    lookup_vector& operator=(const lookup_vector& x);
    /* (affect) move */
    lookup_vector& operator=(lookup_vector&& x);
    /* (affect) initializer list */
    lookup_vector& operator=(std::initializer_list<value_type> il);

    /* Iterators */
    iterator                begin() noexcept;
    const_iterator          begin() const noexcept;
    iterator                end() noexcept;
    const_iterator          end() const noexcept;
    reverse_iterator        rbegin() noexcept;
    const_reverse_iterator  rbegin() const noexcept;
    reverse_iterator        rend() noexcept;
    const_reverse_iterator  rend() const noexcept;
    const_iterator          cbegin() const noexcept;
    const_iterator          cend() const noexcept;
    const_reverse_iterator  crbegin() const noexcept;
    const_reverse_iterator  crend() const noexcept;

    /* Capacity */
    size_type   size() const noexcept;
    size_type   max_size() const noexcept;
    void        resize(size_type n);
    void        resize(size_type n, const value_type& val);
    size_t      capacity() const noexcept;
    bool        empty() const noexcept;
    void        reserve(size_type n);
    void        shrink_to_fit();

    /* Element access */
    reference           operator[](size_type n);
    const_reference     operator[](size_type n) const;
    reference           at(size_type n);
    const_reference     at(size_type n) const;
    reference           front();
    const_reference     front() const ;
    reference           back();
    const_reference     back() const;
    size_type           find(const value_type& val) const; /* NEW */
    size_type           lookup(const value_type& val) const; /* NEW */
    value_type*         data() noexcept;
    const value_type*   data() const noexcept;

    /* Modifiers */
    template <class InputIterator>
        void        assign(InputIterator first, InputIterator last);
    void            assign(size_type n, const value_type& val);
    void            assign(std::initializer_list<value_type> il);
    void            push_back(const value_type& val);
    void            push_back(value_type&& val);
    size_type       push(const value_type& val); /* NEW */
    size_type       push(value_type& val); /* NEW */
    void            pop_back();
    size_type       pop(const value_type& val); /* NEW */
    size_type       pop(value_type& val); /* NEW */
    iterator        insert(const_iterator position, const value_type& val);
    iterator        insert(const_iterator position, size_type n, const value_type& val);
    template <class InputIterator>
        iterator    insert(const_iterator position, InputIterator first, InputIterator last);
    iterator        insert(const_iterator position, value_type&& val);
    iterator        insert(const_iterator position, std::initializer_list<value_type> il);
    iterator        erase(const_iterator position);
    iterator        erase(const_iterator first, const_iterator last);
    void            swap(lookup_vector& x);
    void            clear() noexcept;
    template <class... Args>
        iterator    emplace(const_iterator position, Args&&... args);
    template <class... Args>
        void        emplace_back(Args&&... args);
    
    /* Allocator */
    allocator_type get_allocator() const noexcept;

protected:
    std::vector<T, Alloc> _vector;

private:
    
};

template <class T, class Alloc>
    bool operator==(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);
template <class T, class Alloc>
    bool operator!=(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);
template <class T, class Alloc>
    bool operator<(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);
template <class T, class Alloc>
    bool operator<=(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);
template <class T, class Alloc>
    bool operator>(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);
template <class T, class Alloc>
    bool operator>=(const lookup_vector<T,Alloc>& lhs, const lookup_vector<T,Alloc>& rhs);

template <class T, class Alloc>
    void swap(lookup_vector<T,Alloc>& x, lookup_vector<T,Alloc>& y);

#endif // LOOKUP_VECTOR_HPP
