#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

struct TreeNode
{
    int version_id; // Will be used as hash key in version_map of File
    string content;
    string message; // Empty string if not snapshotted
    time_t created_timestamp;
    time_t modified_timestamp;
    time_t snapshot_timestamp; // -1 if not a snapshot and not root. time(0) if root
    TreeNode *parent;
    vector<TreeNode *> children;
    bool is_snapshotted; // Keep track if this node is snapshotted

    // Constructor if root version is being made
    TreeNode() : version_id(0), content(""), message("DEFAULT MESSAGE"), created_timestamp(time(0)), modified_timestamp(time(0)), snapshot_timestamp(time(0)), parent(nullptr), is_snapshotted(true) {}
    // Constructor for new version
    TreeNode(int vid, string cont, string msg) : version_id(vid), content(cont), message(msg), created_timestamp(time(0)), modified_timestamp(time(0)), snapshot_timestamp(-1), parent(nullptr), is_snapshotted(false) {}
};

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

class File
{
    // Keep the remove function private
    void remove(TreeNode *node)
    {
        for (auto child : node->children)
        {
            remove(child);
        }
        delete node;
    }

public:
    string filename;
    TreeNode *root; // Root version
    TreeNode *active_version;
    HashMapF<int, TreeNode *> version_map;
    int total_versions;
    time_t last_modified;

    // Constructor for new file. It is always a root
    File(string filename) : filename(filename), total_versions(0), last_modified(time(0))
    {
        root = new TreeNode();
        active_version = root;
        version_map.push(0, root);
    }

    // DESTROY YOUR CREATION, RECURSIVELY
    ~File()
    {
        remove(root);
        version_map.~HashMapF();
    }
};

class FileSystem
{
    HashMapF<int, File *> files; // Easy and quick access to files using hash map

    Heap heap_RF; // Recent Files heap
    Heap heap_BF; // Biggest Files heap

    // Polynomial rolling hash function for strings
    int hash(string filename)
    {
        int result = 0;
        for (char c : filename)
        {
            result = result * 31 + c;
        }
        result = result < 0 ? -result : result;
        return result % 307;
    }

public:
    FileSystem() : heap_RF('r'), heap_BF('b') {} // Give 'r' and 'b' types to heaps

    // DESTROY YET ANOTHER ONE
    ~FileSystem()
    {
        for (auto file : files.getValues())
        {
            delete file;
        }
    }

    // Create a new file
    // Assumption : No 2 files have same filename
    bool create(string filename)
    {
        int index = hash(filename);
        if (files.find(index) != nullptr)
        {
            return false; // File already exists
        }
        File *file = new File(filename);
        files.push(index, file);
        heap_RF.insert(file);
        heap_BF.insert(file);
        return true; // File Created
    }

    // Read the active version of a file
    string read(string filename)
    {
        int index = hash(filename);
        File *file = files.find(index); // Hash map's find function
        if (file != nullptr)
        {
            return file->active_version->content; // File found and content returned
        }
        return "File does not exist"; // File not found
    }

    // Insert content to the active version of a file
    // Make new version if active version is snapshotted
    bool insert(string filename, string content)
    {
        int index = hash(filename);
        File *file = files.find(index);
        if (file != nullptr) // File exists
        {
            if (file->active_version->is_snapshotted) // Make new version as snapshotted
            {
                file->total_versions++;
                TreeNode *newNode = new TreeNode(file->total_versions, content, "");
                file->version_map.push(file->total_versions, newNode);
                newNode->parent = file->active_version;
                file->active_version->children.push_back(newNode);
                file->active_version = newNode; // Assumption: The active version changes automatically as a new version gets created
            }
            else // Just insert content in the existing active version
            {
                file->active_version->content += content;
                file->active_version->modified_timestamp = time(0);
            }
            file->last_modified = time(0);
            return true; // Content/Version inserted successfully
        }

        return false; // File does not exist
    }

    // Update the content of the active version of a file
    // Make new version if active version is snapshotted
    bool update(string filename, string content)
    {
        int index = hash(filename);
        File *file = files.find(index);
        if (file != nullptr) // File exists
        {
            if (file->active_version->is_snapshotted) // Make new version as snapshotted
            {
                file->total_versions++;
                TreeNode *newNode = new TreeNode(file->active_version->version_id, content, "");
                file->version_map.push(file->total_versions, newNode);
                newNode->parent = file->active_version;
                file->active_version->children.push_back(newNode);
                file->active_version = newNode; // Assumption: The active version changes automatically as a new version gets created
            }
            else // Just update content in the existing active version
            {
                file->active_version->content = content;
                file->active_version->modified_timestamp = time(0);
            }
            file->last_modified = time(0);
            return true; // File updated or Version created successfully
        }

        return false; // File does not exist
    }

    // Snapshot the active version of a file
    bool snapshot(string filename, string message)
    {
        int index = hash(filename);
        File *file = files.find(index);
        if (file != nullptr)
        {
            if (file->active_version->is_snapshotted)
            {
                return true; // Already snapshotted
            }
            file->active_version->is_snapshotted = true;
            if (!message.empty()) // If input message is not empty, update it in active version
            {
                file->active_version->message = message;
            }
            file->active_version->snapshot_timestamp = time(0);
            file->active_version->modified_timestamp = time(0);
            file->last_modified = time(0);
            return true;
        }
        return false;
    }

    // Rollback to previous version or a specific version
    bool rollback(string filename, string version_id = "")
    {
        int index = hash(filename);
        File *file = files.find(index);
        if (file != nullptr)
        {
            if (version_id == "") // No version ID provided, roll back to parent version if exists
            {
                file->active_version = file->active_version->parent;
                return true;
            }
            TreeNode *version = file->version_map.find(stoi(version_id)); // Find the version in version_map
            if (version != nullptr)                                       // Version exists so change the active version to this new version
            {
                file->active_version = version;
                return true;
            }
        }
        return false; // File or version does not exist
    }

    // Return vector of snapshotted versions from active version to root
    vector<TreeNode *> history(string filename)
    {
        vector<TreeNode *> empty_vector; // Return this if file does not exist
        vector<TreeNode *> result;
        int index = hash(filename);
        File *file = files.find(index);
        if (file != nullptr) // File exists
        {
            TreeNode *curr = file->active_version;
            while (curr != nullptr)
            {
                if (curr->is_snapshotted) // Only push snapshotted versions
                {
                    result.push_back(curr);
                }
                curr = curr->parent;
            }

            return result;
        }
        return empty_vector;
    }

    // Return vector of most recently edited files
    vector<File *> recent_files(int num)
    {
        heap_RF.setArray(files.getValues()); // Rebuild the heap everytime this is called
        vector<File *> result;
        for (int i = 0; i < num; i++)
        {
            File *f = heap_RF.pop(); // Keep emptying heap to get most recent files
            if (f == nullptr)
                break;
            result.push_back(f);
        }
        return result;
    }

    // Return vector of files with biggest version trees or most number of versions
    vector<File *> biggest_trees(int num)
    {
        heap_BF.setArray(files.getValues()); // Rebuild the heap everytime this is called
        vector<File *> result;
        for (int i = 0; i < num; i++)
        {
            File *f = heap_BF.pop(); // Keep emptying heap to get biggest tree files
            if (f == nullptr)
                break;
            result.push_back(f);
        }
        return result;
    }
};

vector<string> split(string s)
{
    vector<string> res;
    string temp = "";
    for (char c : s)
    {
        if (c == ' ')
        {
            if (!temp.empty())
            {
                res.push_back(temp);
                temp = "";
            }
        }
        else
        {
            temp += c;
        }
    }
    if (!temp.empty())
    {
        res.push_back(temp);
    }
    return res;
}

// The main driver function
int main()
{
    FileSystem fs;     // Instantiate our worker
    string line_input; // Take entire line as input

    cout << "Enter operation:\n";

    while (getline(cin, line_input))
    {
        if (line_input.empty()) // If empty line, ask for input again
        {
            cout << "Command missing\n";
            continue;
        }
        if (line_input == "EXIT") // Exit condition
        {
            break;
        }
        vector<string> inputs = split(line_input); // Now that some input is there, split it by spaces
        // Checked whether line is empty of no need for inputs.empty() now

        // If size is 1, File name is missing
        if (inputs.size() == 1)
        {
            cout << "Filename missing\n";
            continue;
        }

        // Rest is just many many if conditions for different commands
        if (inputs[0] == "CREATE")
        {
            if (inputs.size() >= 3)
            {
                cout << "Too many arguments for CREATE\n";
                continue;
            }

            bool created = fs.create(inputs[1]); // returns false if file exists already.
            if (created)
            {
                cout << "File created successfully\n";
            }
            else
            {
                cout << "File already exists\n";
            }
        }
        else if (inputs[0] == "READ")
        {
            if (inputs.size() >= 3)
            {
                cout << "Too many arguments for READ\n";
                continue;
            }
            cout << fs.read(inputs[1]) << endl;
        }
        else if (inputs[0] == "HISTORY")
        {
            if (inputs.size() >= 3)
            {
                cout << "Too many arguments for HISTORY\n";
                continue;
            }
            for (auto node : fs.history(inputs[1])) // returned vector filled with File*
            {
                cout << "Version ID: " << node->version_id << ", Message: " << node->message << ", Created: " << ctime(&node->created_timestamp) << ", Modified: " << ctime(&node->modified_timestamp);
                if (node->is_snapshotted)
                {
                    cout << ", Snapshotted at: " << ctime(&node->snapshot_timestamp);
                }
                cout << endl;
            }
            cout << endl;
        }
        else if (inputs[0] == "RECENT_FILES")
        {
            if (inputs.size() >= 3)
            {
                cout << "Too many arguments for RECENT_FILES\n";
                continue;
            }
            for (auto file : fs.recent_files(stoi(inputs[1]))) // returned vector filled with inputs[1] number of File*
            {
                cout << "File: " << file->filename << ", Last Modified: " << ctime(&file->last_modified) << ", Total Versions: " << file->total_versions << endl;
            }
        }
        else if (inputs[0] == "BIGGEST_TREES")
        {
            if (inputs.size() >= 3)
            {
                cout << "Too many arguments for BIGGEST_TREES\n";
                continue;
            }
            for (auto file : fs.biggest_trees(stoi(inputs[1]))) // returned vector filled with inputs[1] number of File*
            {
                cout << "File: " << file->filename << ", Total Versions: " << file->total_versions << ", Last Modified: " << ctime(&file->last_modified) << endl;
            }
        }
        else if (inputs[0] == "INSERT")
        {
            if (inputs.size() == 2)
            {
                cout << "Content missing for INSERT\n";
                continue;
            }

            // Join all remaining inputs as content
            string content = "";
            for (int i = 2; i < inputs.size(); i++)
            {
                content += inputs[i] + " ";
            }
            bool inserted = fs.insert(inputs[1], content); // returns false if file does not exist
            if (inserted)
            {
                cout << "Content inserted successfully\n";
            }
            else
            {
                cout << "File does not exist\n";
            }
        }
        else if (inputs[0] == "UPDATE")
        {
            if (inputs.size() < 3)
            {
                cout << "Content missing for UPDATE\n";
                continue;
            }
            // Join all remaining inputs as content
            string content = "";
            for (int i = 2; i < inputs.size(); i++)
            {
                content += inputs[i] + " ";
            }
            bool updated = fs.update(inputs[1], content); // returns false if file does not exist
            if (updated)
            {
                cout << "Content updated successfully\n";
            }
            else
            {
                cout << "File does not exist\n";
            }
        }
        else if (inputs[0] == "SNAPSHOT")
        {
            // Join all remaining inputs as message
            string message = "";
            for (int i = 2; i < inputs.size(); i++)
            {
                message += inputs[i] + " ";
            }
            message = message.empty() ? "DEFAULT MESSAGE" : message; // If no message provided then give "DEFAULT MESSAGE"
            bool snapshotted = fs.snapshot(inputs[1], message);      // returns false if file does not exist or already snapshotted
            if (snapshotted)
            {
                cout << "Snapshot created successfully\n";
            }
            else
            {
                cout << "File does not exist or already snapshotted\n";
            }
        }
        else if (inputs[0] == "ROLLBACK")
        {
            if (inputs.size() < 3)
            {
                cout << "No version ID provided, rolling back to parent version if exists\n";
            }
            if (inputs.size() >= 4)
            {
                cout << "Too many arguments for ROLLBACK\n";
                continue;
            }
            string version_id = inputs.size() == 3 ? inputs[2] : ""; // If version ID not provided, it will be empty string
            bool rolled_back = fs.rollback(inputs[1], version_id);   // returns false if file or version does not exist
            if (rolled_back)
            {
                cout << "Rollback successful\n";
            }
            else
            {
                cout << "File or version does not exist\n";
            }
        }
        // If none of the commands match
        else
        {
            cout << "Unknown command\n";
        }
    }
    return 0;
}