#include <mpi.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <iomanip> 

using namespace std;

// Structura nod pentru arborele Huffman
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator pentru priority queue (min-heap)
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Eliberare memorie arbore
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// Generare coduri Huffman prin parcurgere recursiva
void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;

    // Daca este frunza
    if (!root->left && !root->right) {
        // Caz special: un singur caracter
        huffmanCode[root->ch] = code.empty() ? "0" : code;
        return;
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

// Construire arbore Huffman din frecvente globale
Node* buildHuffmanTreeFromFreq(int freq[256]) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    // Adaugam doar caracterele care apar
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            pq.push(new Node((char)i, freq[i]));
        }
    }

    if (pq.empty()) return nullptr;

    // Caz special: un singur caracter
    if (pq.size() == 1) return pq.top();

    // Construire arbore
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* sum = new Node('\0', left->freq + right->freq);
        sum->left = left;
        sum->right = right;

        pq.push(sum);
    }

    return pq.top();
}

// Compresie text folosind codurile Huffman
string compress(const string& text, unordered_map<char, string>& huffmanCode) {
    string encoded;
    encoded.reserve(text.size() * 2);

    for (char ch : text)
        encoded += huffmanCode[ch];

    return encoded;
}

// Citire fisier in string
string readFile(const string& filename) {
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Eroare la deschiderea fisierului\n";
        exit(1);
    }

    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    string text;
    int totalSize = 0;

    // Procesul 0 citeste fisierul
    if (rank == 0) {
        text = readFile("input_large.txt");
        totalSize = text.size();
        cout << "Dimensiune input: " << totalSize << " bytes\n";
        cout << "Numar procese: " << size << "\n";
    }

    // Trimitem dimensiunea la toate procesele
    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Daca fisierul este gol
    if (totalSize == 0) {
        if (rank == 0)
            cout << "Fisier gol\n";
        MPI_Finalize();
        return 0;
    }

    // Calculam cate caractere primeste fiecare proces
    vector<int> sendCounts(size), displs(size);

    int base = totalSize / size;
    int remainder = totalSize % size;

    for (int i = 0; i < size; i++) {
        sendCounts[i] = base + (i < remainder ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + sendCounts[i - 1];
    }

    // Buffer local pentru fiecare proces
    string localText(sendCounts[rank], '\0');

    // Impartim textul intre procese
    MPI_Scatterv(
        rank == 0 ? (void*)text.data() : nullptr,
        sendCounts.data(),
        displs.data(),
        MPI_CHAR,
        localText.data(),
        sendCounts[rank],
        MPI_CHAR,
        0,
        MPI_COMM_WORLD
    );

    // Calcul frecvente locale
    int localFreq[256] = { 0 };
    for (unsigned char c : localText)
        localFreq[c]++;

    // Reducere frecvente la procesul 0
    int globalFreq[256] = { 0 };
    MPI_Reduce(localFreq, globalFreq, 256, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Trimitem frecventele globale la toate procesele
    MPI_Bcast(globalFreq, 256, MPI_INT, 0, MPI_COMM_WORLD);

    // Fiecare proces construieste acelasi arbore
    Node* root = buildHuffmanTreeFromFreq(globalFreq);

    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Sincronizare inainte de masurare
    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    // Compresie paralela
    string localEncoded = compress(localText, huffmanCode);

    double end = MPI_Wtime();
    double localTime = end - start;

    // Determinam timpul maxim dintre procese
    double maxTime = 0;
    MPI_Reduce(&localTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Afisare timp final formatat
    if (rank == 0) {
        cout << fixed << setprecision(8);
        cout << "Timp paralel: " << maxTime << " secunde\n";
    }

    // Calcul dimensiune rezultata
    int localEncodedSize = localEncoded.size();
    vector<int> allSizes;

    if (rank == 0)
        allSizes.resize(size);

    MPI_Gather(&localEncodedSize, 1, MPI_INT,
        rank == 0 ? allSizes.data() : nullptr,
        1, MPI_INT,
        0, MPI_COMM_WORLD);

    if (rank == 0) {
        long long totalBits = 0;
        for (int s : allSizes)
            totalBits += s;
        
        cout << "Dimensiune dupa compresie: " << totalBits / 8 << " bytes\n";
    }

    // Eliberare memorie
    freeTree(root);

    MPI_Finalize();
    return 0;
}