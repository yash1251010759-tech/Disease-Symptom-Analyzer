#include <bits/stdc++.h>
using namespace std;

// ==================== STRUCTURES ====================

struct Disease {
    string name;
    vector<string> symptoms;
};

struct TreeNode {
    string diseaseName;
    TreeNode* left;
    TreeNode* right;

    TreeNode(string name) {
        diseaseName = name;
        left = NULL;
        right = NULL;
    }
};

struct HistoryNode {
    string diseaseName;
    HistoryNode* next;

    HistoryNode(string name) {
        diseaseName = name;
        next = NULL;
    }
};

// ==================== GLOBAL VARIABLES ====================

HistoryNode* historyHead = NULL;

unordered_map<string, vector<string>> graph;

// ==================== FILE HANDLING ====================

bool saveDiseases(const vector<Disease>& diseases) {
    ofstream file("diseases.txt");

    if (!file) {
        cout << "Unable to save diseases.txt\n";
        return false;
    }

    for (const auto& d : diseases) {
        file << d.name << '\n';
        file << d.symptoms.size() << '\n';

        for (const auto& s : d.symptoms)
            file << s << '\n';
    }

    return true;
}

bool loadDiseases(vector<Disease>& diseases) {
    ifstream file("diseases.txt");

    if (!file)
        return false;

    vector<Disease> loaded;
    string name, countLine;

    while (getline(file, name)) {
        if (name.empty())
            continue;

        if (!getline(file, countLine))
            return false;

        int count;

        try {
            count = stoi(countLine);
        }
        catch (...) {
            return false;
        }

        if (count < 0)
            return false;

        Disease d;
        d.name = name;

        for (int i = 0; i < count; i++) {
            string symptom;

            if (!getline(file, symptom))
                return false;

            d.symptoms.push_back(symptom);
        }

        loaded.push_back(d);
    }

    if (loaded.empty())
        return false;

    diseases = loaded;
    return true;
}

void saveHistoryEntry(string entry) {
    ofstream file("history.txt", ios::app);

    if (file)
        file << entry << '\n';
}

void showSavedHistory() {
    ifstream file("history.txt");
    string line;

    cout << "\n===== SAVED HISTORY =====\n";

    if (!file) {
        cout << "No saved history file found.\n";
        return;
    }

    bool found = false;

    while (getline(file, line)) {
        cout << line << '\n';
        found = true;
    }

    if (!found)
        cout << "No saved history available.\n";
}

// ==================== LINKED LIST HISTORY ====================

void addToHistory(string name) {
    HistoryNode* newNode = new HistoryNode(name);

    newNode->next = historyHead;
    historyHead = newNode;

    saveHistoryEntry(name);
}

void displayHistory() {
    if (historyHead == NULL) {
        cout << "\nNo search history is available!\n";
        return;
    }

    HistoryNode* temp = historyHead;

    cout << "\n===== RECENT SEARCH HISTORY =====\n";

    while (temp != NULL) {
        cout << temp->diseaseName << "\n";
        temp = temp->next;
    }
}

// ==================== GRAPH ====================

// Bipartite graph: Disease nodes <-> Symptom nodes.

string diseaseNode(string name) {
    return "Disease: " + name;
}

string symptomNode(string name) {
    return "Symptom: " + name;
}

void buildGraph(const vector<Disease>& diseases) {
    graph.clear();

    for (const auto& d : diseases) {
        string dn = diseaseNode(d.name);
        graph[dn];

        unordered_set<string> used;

        for (const auto& s : d.symptoms) {
            if (!used.insert(s).second)
                continue;

            string sn = symptomNode(s);

            graph[dn].push_back(sn);
            graph[sn].push_back(dn);
        }
    }
}

void displayGraph() {
    cout << "\n===== DISEASE-SYMPTOM GRAPH =====\n";

    if (graph.empty()) {
        cout << "Graph is empty.\n";
        return;
    }

    vector<string> nodes;

    for (const auto& p : graph)
        nodes.push_back(p.first);

    sort(nodes.begin(), nodes.end());

    for (const auto& node : nodes) {
        cout << node << " -> ";

        vector<string> neighbours = graph[node];
        sort(neighbours.begin(), neighbours.end());

        for (size_t i = 0; i < neighbours.size(); i++) {
            if (i)
                cout << ", ";

            cout << neighbours[i];
        }

        cout << '\n';
    }
}

// BFS Traversal

void graphBFS() {
    string name;

    cout << "Enter disease name: ";
    getline(cin >> ws, name);

    string start = diseaseNode(name);

    if (!graph.count(start)) {
        cout << "Disease not found in graph.\n";
        return;
    }

    queue<string> q;
    unordered_set<string> visited;

    q.push(start);
    visited.insert(start);

    cout << "\nBFS Traversal:\n";

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        cout << current << '\n';

        vector<string> neighbours = graph[current];
        sort(neighbours.begin(), neighbours.end());

        for (const auto& next : neighbours) {
            if (!visited.count(next)) {
                visited.insert(next);
                q.push(next);
            }
        }
    }

    addToHistory("BFS: " + name);
}

// DFS Traversal

void dfsVisit(string node, unordered_set<string>& visited) {
    visited.insert(node);
    cout << node << '\n';

    vector<string> neighbours = graph[node];
    sort(neighbours.begin(), neighbours.end());

    for (const auto& next : neighbours) {
        if (!visited.count(next))
            dfsVisit(next, visited);
    }
}

void graphDFS() {
    string name;

    cout << "Enter disease name: ";
    getline(cin >> ws, name);

    string start = diseaseNode(name);

    if (!graph.count(start)) {
        cout << "Disease not found in graph.\n";
        return;
    }

    unordered_set<string> visited;

    cout << "\nDFS Traversal:\n";
    dfsVisit(start, visited);

    addToHistory("DFS: " + name);
}

// Search Diseases Using Graph

void graphSymptomSearch() {
    string symptom;

    cout << "Enter symptom: ";
    getline(cin >> ws, symptom);

    string node = symptomNode(symptom);

    if (!graph.count(node)) {
        cout << "Symptom not found in graph.\n";
        return;
    }

    cout << "\nDiseases connected to " << symptom << ":\n";

    for (const auto& neighbour : graph[node]) {
        if (neighbour.find("Disease: ") == 0)
            cout << neighbour.substr(9) << '\n';
    }

    addToHistory("Graph symptom search: " + symptom);
}

// ==================== BINARY SEARCH TREE ====================

TreeNode* insertDisease(TreeNode* root, string name) {
    if (root == NULL) {
        return new TreeNode(name);
    }
    else if (name < root->diseaseName) {
        root->left = insertDisease(root->left, name);
    }
    else if (name > root->diseaseName) {
        root->right = insertDisease(root->right, name);
    }

    return root;
}

TreeNode* buildBST(const vector<Disease>& diseases) {
    TreeNode* root = NULL;

    for (const auto& d : diseases)
        root = insertDisease(root, d.name);

    return root;
}

void deleteBST(TreeNode* root) {
    if (root == NULL)
        return;

    deleteBST(root->left);
    deleteBST(root->right);
    delete root;
}

void inorderTraversal(TreeNode* root) {
    if (root == NULL)
        return;

    inorderTraversal(root->left);
    cout << root->diseaseName << endl;
    inorderTraversal(root->right);
}

void preorderTraversal(TreeNode* root) {
    if (root == NULL)
        return;

    cout << root->diseaseName << endl;
    preorderTraversal(root->left);
    preorderTraversal(root->right);
}

void postorderTraversal(TreeNode* root) {
    if (root == NULL)
        return;

    postorderTraversal(root->left);
    postorderTraversal(root->right);
    cout << root->diseaseName << endl;
}

TreeNode* searchDiseaseBST(TreeNode* root, string name) {
    if (root == NULL)
        return NULL;

    else if (name == root->diseaseName)
        return root;

    else if (name < root->diseaseName)
        return searchDiseaseBST(root->left, name);

    else
        return searchDiseaseBST(root->right, name);
}

// ==================== DISEASE FUNCTIONS ====================

// Display All Diseases

void displayDiseases(const vector<Disease>& diseases) {
    cout << "\n========== ALL DISEASES ==========\n";

    if (diseases.empty()) {
        cout << "No diseases available.\n";
        return;
    }

    for (int i = 0; i < diseases.size(); i++) {
        cout << "\nDisease: " << diseases[i].name << "\n";
        cout << "Symptoms: ";

        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            cout << diseases[i].symptoms[j];

            if (j + 1 < diseases[i].symptoms.size())
                cout << ", ";
        }

        cout << "\n";
    }
}

// Search Disease

void searchDiseases(const vector<Disease>& diseases) {
    string userDisease;
    bool found = false;

    cout << "\nEnter disease name: ";
    getline(cin >> ws, userDisease);

    for (int i = 0; i < diseases.size(); i++) {
        if (diseases[i].name == userDisease) {
            addToHistory(diseases[i].name);
            found = true;

            cout << "Disease found: " << diseases[i].name << endl;
            cout << "Symptoms: ";

            for (int j = 0; j < diseases[i].symptoms.size(); j++) {
                cout << diseases[i].symptoms[j];

                if (j + 1 < diseases[i].symptoms.size())
                    cout << ", ";
            }

            cout << '\n';
            break;
        }
    }

    if (!found)
        cout << "Disease not found in the dataset.\n";
}

// Find Diseases By Symptom

void findDiseasesBySymptom(const vector<Disease>& diseases) {
    string userSymptom;
    bool found = false;

    cout << "\nEnter symptom: ";
    getline(cin >> ws, userSymptom);

    for (int i = 0; i < diseases.size(); i++) {
        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            if (diseases[i].symptoms[j] == userSymptom) {
                found = true;

                cout << "Disease containing " << userSymptom
                     << ": " << diseases[i].name << endl;

                break;
            }
        }
    }

    if (!found)
        cout << "Symptom not found.\n";
}

// Add New Disease

void addDisease(vector<Disease>& diseases) {
    Disease d;
    string newDisease;

    cout << "Enter the new disease name: ";
    getline(cin >> ws, newDisease);

    for (const auto& existing : diseases) {
        if (existing.name == newDisease) {
            cout << "Disease already exists.\n";
            return;
        }
    }

    int n;

    cout << "Enter the number of symptoms: ";

    if (!(cin >> n)) {
        cout << "Invalid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    if (n <= 0) {
        cout << "Number of symptoms must be positive.\n";
        return;
    }

    vector<string> newSymptoms(n);

    cout << "Enter the symptoms: ";

    for (int i = 0; i < n; i++)
        cin >> newSymptoms[i];

    d.name = newDisease;
    d.symptoms = newSymptoms;

    diseases.push_back(d);

    cout << "New disease added successfully!\n";
}

// Common Symptoms

void commonSymptoms(const vector<Disease>& diseases) {
    unordered_map<string, int> frequency;

    for (int i = 0; i < diseases.size(); i++) {
        unordered_set<string> counted;

        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            string symptom = diseases[i].symptoms[j];

            if (counted.insert(symptom).second)
                frequency[symptom]++;
        }
    }

    cout << "\n===== COMMON SYMPTOMS =====\n";

    bool found = false;

    for (const auto& entry : frequency) {
        if (entry.second >= 2) {
            cout << entry.first << " appears in "
                 << entry.second << " diseases.\n";

            found = true;
        }
    }

    if (!found)
        cout << "No common symptoms found.\n";
}

// Symptom Frequency

void symptomFrequency(const vector<Disease>& diseases) {
    unordered_map<string, int> frequency;

    for (int i = 0; i < diseases.size(); i++) {
        unordered_set<string> counted;

        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            string symptom = diseases[i].symptoms[j];

            if (counted.insert(symptom).second)
                frequency[symptom]++;
        }
    }

    cout << "\n====== SYMPTOM FREQUENCY =====\n";

    if (frequency.empty()) {
        cout << "No symptoms available in the dataset.\n";
        return;
    }

    for (const auto& entry : frequency)
        cout << entry.first << " : " << entry.second << " diseases\n";
}

// Delete Disease

void deleteDisease(vector<Disease>& diseases) {
    string diseaseName;

    cout << "Enter the name of the disease to be deleted: ";
    getline(cin >> ws, diseaseName);

    bool found = false;

    for (int i = 0; i < diseases.size(); i++) {
        if (diseases[i].name == diseaseName) {
            found = true;

            diseases.erase(diseases.begin() + i);

            cout << "Disease " << diseaseName
                 << " deleted successfully!\n";

            break;
        }
    }

    if (!found)
        cout << "Disease " << diseaseName << " was not found.\n";
}

// Update Disease

bool updateDisease(vector<Disease>& diseases, string diseaseName) {
    for (int i = 0; i < diseases.size(); i++) {
        if (diseases[i].name == diseaseName) {

            int count;

            cout << "Enter the number of new symptoms of "
                 << diseaseName << ": ";

            if (!(cin >> count)) {
                cout << "Invalid number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return false;
            }

            if (count <= 0) {
                cout << "Number of symptoms must be positive.\n";
                return false;
            }

            vector<string> s(count);

            cout << "\nEnter the new symptoms: ";

            for (int j = 0; j < count; j++)
                cin >> s[j];

            diseases[i].symptoms = s;

            cout << "\nThe updated Disease: "
                 << diseases[i].name << " : ";

            for (int k = 0; k < diseases[i].symptoms.size(); k++)
                cout << diseases[i].symptoms[k] << " ";

            cout << "\nDisease " << diseaseName
                 << " updated successfully!\n";

            return true;
        }
    }

    cout << "Disease " << diseaseName << " is not found.\n";
    return false;
}

// Sort Diseases

void sortDiseases(vector<Disease>& diseases) {
    sort(diseases.begin(), diseases.end(),
         [](const Disease& a, const Disease& b) {
             return a.name < b.name;
         });

    cout << "The diseases in sorted order are:\n";

    for (int i = 0; i < diseases.size(); i++)
        cout << diseases[i].name << "\n";
}

// Sort Symptoms

void sortSymptoms(vector<Disease>& diseases) {
    unordered_map<string, int> frequency;

    for (int i = 0; i < diseases.size(); i++) {
        unordered_set<string> counted;

        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            string symptom = diseases[i].symptoms[j];

            if (counted.insert(symptom).second)
                frequency[symptom]++;
        }
    }

    vector<pair<string, int>> ans;

    for (auto& entry : frequency)
        ans.push_back({entry.first, entry.second});

    sort(ans.begin(), ans.end(),
         [](const pair<string, int>& a, const pair<string, int>& b) {
             if (a.second != b.second)
                 return a.second > b.second;

             return a.first < b.first;
         });

    cout << "\n===== SORTED SYMPTOMS =====\n";

    for (const auto& entry : ans)
        cout << entry.first << " : " << entry.second << " diseases\n";
}

// Top K Symptoms

void topKsymptoms(vector<Disease>& diseases) {
    unordered_map<string, int> frequency;

    for (int i = 0; i < diseases.size(); i++) {
        unordered_set<string> counted;

        for (int j = 0; j < diseases[i].symptoms.size(); j++) {
            string symptom = diseases[i].symptoms[j];

            if (counted.insert(symptom).second)
                frequency[symptom]++;
        }
    }

    vector<pair<string, int>> ans;

    for (auto& entry : frequency)
        ans.push_back({entry.first, entry.second});

    sort(ans.begin(), ans.end(),
         [](const pair<string, int>& a, const pair<string, int>& b) {
             if (a.second != b.second)
                 return a.second > b.second;

             return a.first < b.first;
         });

    if (ans.empty()) {
        cout << "No symptoms available.\n";
        return;
    }

    int k;

    cout << "Enter the value of k: ";

    if (!(cin >> k)) {
        cout << "Invalid value of k.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << endl;

    if (k <= 0) {
        cout << "K must be positive.\n";
        return;
    }

    k = min(k, (int)ans.size());

    for (int i = 0; i < k; i++)
        cout << ans[i].first << " : " << ans[i].second << " diseases\n";
}

// ==================== REPORT ====================

void exportReport(const vector<Disease>& diseases) {
    ofstream file("report.txt");

    if (!file) {
        cout << "Unable to create report.txt\n";
        return;
    }

    file << "===== DISEASE-SYMPTOM REPORT =====\n\n";

    for (const auto& d : diseases) {
        file << "Disease: " << d.name << '\n';
        file << "Symptoms: ";

        for (size_t i = 0; i < d.symptoms.size(); i++) {
            file << d.symptoms[i];

            if (i + 1 < d.symptoms.size())
                file << ", ";
        }

        file << "\n\n";
    }

    file.close();

    cout << "Report exported successfully to report.txt\n";
}

// ==================== MAIN FUNCTION ====================

int main() {

    // Sample educational dataset

    vector<Disease> diseases = {
        {"Disease A", {"Fever", "Cough", "Fatigue"}},
        {"Disease B", {"Fever", "Headache", "Fatigue"}},
        {"Disease C", {"Cough", "Nausea", "Fatigue"}},
        {"Disease D", {"Fever", "Headache", "Nausea"}},
        {"Disease E", {"Cough", "Fatigue", "Headache"}}
    };

    if (loadDiseases(diseases))
        cout << "Dataset loaded successfully from diseases.txt\n";
    else
        cout << "Using default dataset.\n";

    TreeNode* root = buildBST(diseases);

    buildGraph(diseases);

    int choice;

    do {
        bool changed = false;

        cout << "\n";
        cout << "============================================\n";
        cout << "   DISEASE-SYMPTOM RELATIONSHIP ANALYZER\n";
        cout << "============================================\n";

        cout << "1. Display All Diseases\n";
        cout << "2. Search Disease\n";
        cout << "3. Find Diseases by Symptom\n";
        cout << "4. Find Common Symptoms\n";
        cout << "5. Show Symptom Frequency\n";
        cout << "6. Add new disease\n";
        cout << "7. Delete diseases\n";
        cout << "8. Update diseases\n";
        cout << "9. Sort diseases\n";
        cout << "10. Sort Symptoms\n";
        cout << "11. Top K symptoms\n";
        cout << "12. View recent search history\n";
        cout << "13. BST Inorder Traversal\n";
        cout << "14. BST Preorder Traversal\n";
        cout << "15. BST Postorder Traversal\n";
        cout << "16. Search Disease Using BST\n";
        cout << "17. Display Disease-Symptom Graph\n";
        cout << "18. Graph BFS Traversal\n";
        cout << "19. Graph DFS Traversal\n";
        cout << "20. Search Diseases Using Graph Symptom\n";
        cout << "21. View Saved History\n";
        cout << "22. Export Report\n";
        cout << "23. Save Dataset and Exit\n";

        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            continue;
        }

        switch (choice) {

            case 1:
                displayDiseases(diseases);
                break;

            case 2:
                searchDiseases(diseases);
                break;

            case 3:
                findDiseasesBySymptom(diseases);
                break;

            case 4:
                commonSymptoms(diseases);
                break;

            case 5:
                symptomFrequency(diseases);
                break;

            case 6: {
                size_t oldSize = diseases.size();

                addDisease(diseases);

                changed = diseases.size() != oldSize;
                break;
            }

            case 7: {
                size_t oldSize = diseases.size();

                deleteDisease(diseases);

                changed = diseases.size() != oldSize;
                break;
            }

            case 8: {
                string diseaseName;

                cout << "Enter the name of the disease to be updated: ";
                getline(cin >> ws, diseaseName);

                changed = updateDisease(diseases, diseaseName);

                break;
            }

            case 9:
                sortDiseases(diseases);
                changed = true;
                break;

            case 10:
                sortSymptoms(diseases);
                break;

            case 11:
                topKsymptoms(diseases);
                break;

            case 12:
                displayHistory();
                break;

            case 13:
                cout << "\nBST Inorder Traversal:\n";
                inorderTraversal(root);
                break;

            case 14:
                cout << "\nBST Preorder Traversal:\n";
                preorderTraversal(root);
                break;

            case 15:
                cout << "\nBST Postorder Traversal:\n";
                postorderTraversal(root);
                break;

            case 16: {
                string name;

                cout << "Enter disease name: ";
                getline(cin >> ws, name);

                if (searchDiseaseBST(root, name)) {
                    cout << "Disease found in BST.\n";
                    addToHistory("BST search: " + name);
                }
                else {
                    cout << "Disease not found in BST.\n";
                }

                break;
            }

            case 17:
                displayGraph();
                break;

            case 18:
                graphBFS();
                break;

            case 19:
                graphDFS();
                break;

            case 20:
                graphSymptomSearch();
                break;

            case 21:
                showSavedHistory();
                break;

            case 22:
                exportReport(diseases);
                break;

            case 23:
                saveDiseases(diseases);
                cout << "Dataset saved. Exiting...\n";
                break;

            default:
                cout << "Invalid choice. Select 1-23.\n";
        }

        // Update dataset, BST and Graph whenever data changes

        if (changed) {
            saveDiseases(diseases);

            deleteBST(root);
            root = buildBST(diseases);

            buildGraph(diseases);

            cout << "Dataset, BST, and graph updated.\n";
        }

    } while (choice != 23);

    // Free BST memory

    deleteBST(root);

    // Free Linked List memory

    while (historyHead) {
        HistoryNode* next = historyHead->next;

        delete historyHead;

        historyHead = next;
    }

    return 0;
}