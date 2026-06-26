// Include necessary libraries
#include <iostream>
#include <vector>
#include <string>
// Include custom libraries
#include "file_system.hpp"
using namespace std;

// Function to split input string by spaces. Like the python string.split() function
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