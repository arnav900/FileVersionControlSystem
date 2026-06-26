// Include necessary libraries
#include <vector>
// Include custom libraries
#include "file.hpp"
using namespace std;

class Heap
{
    vector<File *> arr; // Array to store heap elements
    int size = 0;       // Custom size rather than arr.size()
    char type;

    int parent(int i)
    {
        return (i - 1) / 2;
    }
    int left(int i)
    {
        return 2 * i + 1;
    }
    int right(int i)
    {
        return 2 * i + 2;
    }

    // Heapify up for insertion
    void heapifyUp(int i)
    {
        while (i > 0)
        {
            int p = parent(i);
            // 2 cases: the heap is for recently edited files or biggest files
            switch (type)
            {
            // Case for recently edited files. Heap based on last_modified timestamp
            case 'r':
                if (arr[i]->last_modified > arr[p]->last_modified)
                {
                    swap(arr[i], arr[p]);
                    i = p;
                }
                break;
            // Case for biggest files. Heap based on total_versions
            case 'b':
                if (arr[i]->total_versions > arr[p]->total_versions)
                {
                    swap(arr[i], arr[p]);
                    i = p;
                }
                break;
            }
        }
    }

    // Heapify down for deletion
    void heapifyDown(int i)
    {
        int l = left(i);
        int r = right(i);
        int largest = i;

        // 2 cases: the heap is for recently edited files or biggest files
        switch (type)
        {
        // Case for recently edited files. Heap based on last_modified timestamp
        case 'r':
            if (l < size && arr[l]->last_modified > arr[largest]->last_modified)
            {
                largest = l;
            }
            if (r < size && arr[r]->last_modified > arr[largest]->last_modified)
            {
                largest = r;
            }
            break;
        // Case for biggest files. Heap based on total_versions
        case 'b':
            if (l < size && arr[l]->total_versions > arr[largest]->total_versions)
            {
                largest = l;
            }
            if (r < size && arr[r]->total_versions > arr[largest]->total_versions)
            {
                largest = r;
            }
            break;
        }

        if (largest != i)
        {
            swap(arr[i], arr[largest]);
            heapifyDown(largest);
        }
    }

public:
    Heap(char type) : type(type) {}
    // DESTROY YET ANOTHER CREATION
    ~Heap()
    {
        arr.clear(); // Just empty the vector
        size = 0;    // and reset size
    }

    // Insert a new File
    void insert(File *val)
    {
        arr.push_back(val);
        size++;
        heapifyUp(size - 1);
    }

    // Remove and return the top File. Used in RECENT_FILES and BIGGEST_TREES
    File *pop()
    {
        if (size == 0)
        {
            return nullptr;
        }
        File *val = arr[0];
        arr[0] = arr[size - 1];
        arr.pop_back();
        size--;
        heapifyDown(0);
        return val;
    }

    // Get the top File without removing it
    // Unused function but kept for completeness and satisfaction
    File *top()
    {
        if (size == 0)
        {
            return nullptr;
        }
        return arr[0];
    }

    // Basically build_heap
    void setArray(vector<File *> newArr)
    {
        arr = newArr;
        size = arr.size();
        for (int i = parent(size - 1); i >= 0; i--)
        {
            heapifyDown(i);
        }
    }
};