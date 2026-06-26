// Include the necessary libraries
#include <ctime>

// Include custom libraries
#include "tree_node.hpp"
#include "hash_map.hpp"

// File class. It's a subpart of the bigger File System
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