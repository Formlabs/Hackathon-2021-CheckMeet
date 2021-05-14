#pragma once

template<class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
class Span {
    T* m_Data = nullptr;
    size_t m_Size = 0;
public:
    using element_type = T;
    using value_type = typename std::remove_cv<T>::type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = const T&;
    using const_reference = const T&;
    using iterator = T*;
    using reverse_iterator = std::reverse_iterator<iterator>;

    Span() = default;
    Span(T* first, size_type count) : m_Data(first), m_Size(count) {}
    Span(T* first) : m_Data(first), m_Size(std::char_traits<value_type>::length(first)) {}
    template<class ContiguousRange>
    Span(ContiguousRange&& rng) : m_Data(rng.data()), m_Size(rng.size()) {}

    iterator begin() const { return m_Data; }
    iterator end() const { return m_Data + m_Size; }
    pointer data() const { return m_Data; }
    size_type size() const { return m_Size; }
};

using StringView = Span<const char>;

inline StringView operator "" _sv(const char* str, std::size_t len) { return StringView(str, len); }

inline std::string fmt(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    size_t len = static_cast<size_t>(vsnprintf(nullptr, 0, format, arg));
    std::vector<char> buffer(len+1);
    vsnprintf(buffer.data(), len + 1, format, arg);
    va_end(arg);
    return buffer.data();
}