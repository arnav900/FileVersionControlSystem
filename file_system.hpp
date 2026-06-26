// Include neccessary libraries
#include <vector>
#include <string>
#include <ctime>
// Include custom libraries
#include "heap.hpp"
using namespace std;

// The worker in the program
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