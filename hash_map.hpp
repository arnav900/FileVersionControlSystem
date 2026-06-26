// Include the necessary libraries
#include <vector>
using namespace std;

// Use template to use custom data types
template <typename K, typename V>
class HashMapF
{
    const int SIZE = 1009; // Take prime number for minimal collisions
    struct Node
    {
        K key;
        V value;
        Node *next;

        Node(K k, V v) : key(k), value(v), next(nullptr) {}
    };
    // Array of linked lists to handle collisions using chaining
    vector<Node *> arrays;

public:
    HashMapF() : arrays(SIZE, nullptr) {}
    // DESTROY EVERYTHING
    ~HashMapF()
    {
        for (auto node : arrays)
        {
            while (node != nullptr)
            {
                Node *curr = node;
                node = node->next;
                delete curr;
            }
        }
    }

    // Simplest hasher
    int hash(int key)
    {
        return key % SIZE;
    }

    // Put value in hash map
    void push(int key, V value)
    {
        int index = hash(key);
        Node *newNode = new Node(key, value);
        newNode->next = arrays[index];
        arrays[index] = newNode;
    }

    // Find value in hash map
    V find(int key)
    {
        int index = hash(key);
        Node *node = arrays[index];
        while (node != nullptr)
        {
            if (node->key == key)
            {
                return node->value;
            }
            node = node->next;
        }
        return nullptr; // Not found
    }

    // Return vector of all values in the hash map
    vector<V> getValues()
    {
        vector<V> values;
        for (auto node : arrays)
        {
            auto temp = node;
            while (temp != nullptr)
            {
                values.push_back(temp->value);
                temp = temp->next;
            }
        }
        return values;
    }
};