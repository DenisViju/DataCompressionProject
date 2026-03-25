#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <chrono>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;

    if (!root->left && !root->right)
        huffmanCode[root->ch] = code;

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

Node* buildHuffmanTree(const string& text) {
    unordered_map<char, int> freq;

    for (char ch : text)
        freq[ch]++;

    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : freq)
        pq.push(new Node(pair.first, pair.second));

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* sum = new Node('\0', left->freq + right->freq);
        sum->left = left;
        sum->right = right;

        pq.push(sum);
    }

    return pq.top();
}

string compress(const string& text, unordered_map<char, string>& huffmanCode) {
    string encoded = "";

    for (char ch : text)
        encoded += huffmanCode[ch];

    return encoded;
}

string readFile(const string& filename) {
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Eroare la deschiderea fisierului!\n";
        exit(1);
    }

    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main() {
    string filename = "input_large.txt";
    string text = readFile(filename);

    cout << "Dimensiune input: " << text.size() << " bytes\n";

    
    if (text.size() <= 100) {

        unordered_map<char, string> huffmanCode;

        Node* root = buildHuffmanTree(text);
        generateCodes(root, "", huffmanCode);
        string encoded = compress(text, huffmanCode);

        cout << "Text original: " << text << "\n\n";

        cout << "Coduri Huffman:\n";
        for (auto pair : huffmanCode) {
            cout << pair.first << " : " << pair.second << "\n";
        }

        cout << "\nText comprimat:\n" << encoded << "\n";
        cout << "Dimensiune dupa compresie: " << encoded.size() << " biti\n";
    }

    
    const int RUNS = 5;
    double totalTime = 0;

    for (int i = 0; i < RUNS; i++) {

        unordered_map<char, string> huffmanCode;

        auto start = chrono::high_resolution_clock::now();

        Node* root = buildHuffmanTree(text);
        generateCodes(root, "", huffmanCode);
        string encoded = compress(text, huffmanCode);

        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> duration = end - start;
        double time = duration.count();

        cout << "Rulare " << i + 1 << ": " << time << " secunde\n";

        totalTime += time;
    }

    cout << "\nTimp mediu: " << (totalTime / RUNS) << " secunde\n";

    return 0;
}