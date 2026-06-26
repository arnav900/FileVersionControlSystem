// Import the necessary libraries
#include <vector>
#include <string>
#include <ctime>
using namespace std;

// TreeNode struct
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