template <typename T, std::size_t MaxSize>
class StaticStorage {
private:
    std::array<std::optional<T>, MaxSize> storage;
    std::array<std::string, MaxSize> keys;
    std::size_t count = 0;

    int findKeyIndex(const std::string& key) const {
        for (std::size_t i = 0; i < count; ++i) {
            if (keys[i] == key) {
                return i;
            }
        }
        return -1; // Key not found
    }

public:
    bool set(const std::string& key, const T& value) {
        if (count >= MaxSize) {
            throw std::runtime_error("Storage is full");
        }

        int index = findKeyIndex(key);
        if (index != -1) {
            storage[index] = value;
            return true;
        }

        keys[count] = key;
        storage[count] = value;
        ++count;
        return true;
    }

    std::optional<T> get(const std::string& key) const {
        int index = findKeyIndex(key);
        if (index != -1) {
            return storage[index];
        }
        return std::nullopt; // Key not found
    }

    bool exists(const std::string& key) const {
        return findKeyIndex(key) != -1;
    }

    bool remove(const std::string& key) {
        int index = findKeyIndex(key);
        if (index == -1) {
            return false; // Key not found
        }

        // Shift elements to remove the key and its value
        for (std::size_t i = index; i < count - 1; ++i) {
            keys[i] = keys[i + 1];
            storage[i] = storage[i + 1];
        }

        keys[count - 1].clear();
        storage[count - 1].reset();
        --count;
        return true;
    }

    std::size_t size() const {
        return count;
    }
};
