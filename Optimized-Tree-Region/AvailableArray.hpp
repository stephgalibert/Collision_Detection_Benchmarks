#pragma once

template <typename T>
class AvailableArray
{
public:
    AvailableArray() = default;
    ~AvailableArray() = default;

    int32_t push(const T& element)
    {
        if (!available_.empty()) {
            size_t index = available_.back();
            available_.pop_back();
            array_[index] = element;
            return index;
        }
        array_.push_back(element);
        return array_.size() - 1;
    }

    size_t erase(size_t index)
    {
        // Warning: child must be adjacent?
        available_.push_back(index);
    }

    void swap(AvailableArray<T>& other)
    {
        array_.swap(other.array_);
        available_.swap(other.available_);
    }

    size_t size() const
    {
        return array_.size();
    }

    const T& operator[](size_t index) const
    {
        return array_[index];
    }

    T& operator[](size_t index)
    {
        return array_[index];
    }

    typename std::vector<T>::iterator begin() { return std::begin(array_); }
    typename std::vector<T>::const_iterator begin() const { return std::begin(array_); }
    typename std::vector<T>::iterator end() { return std::end(array_); }
    typename std::vector<T>::const_iterator end() const { return std::end(array_); }

private:
    std::vector<T> array_;
    std::vector<size_t> available_;
};