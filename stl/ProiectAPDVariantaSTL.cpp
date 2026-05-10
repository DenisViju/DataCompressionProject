#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <array>
#include <iomanip>
#include <algorithm>
#include <execution>   // std::execution::par
#include <numeric>     // std::iota
#include <chrono>
#include <thread>      // std::thread::hardware_concurrency

using namespace std;
using namespace chrono;

// ---------------------------------------------------------------
// Structura nod pentru arborele Huffman
// ---------------------------------------------------------------
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
void generateCodes(Node* root, const string& code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;

    // Daca este frunza
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code.empty() ? "0" : code;
        return;
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

// Construire arbore Huffman din frecvente globale
Node* buildHuffmanTreeFromFreq(const array<int, 256>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0)
            pq.push(new Node(static_cast<char>(i), freq[i]));
    }

    if (pq.empty()) return nullptr;
    if (pq.size() == 1) return pq.top();

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

// Citire fisier in string
string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Eroare la deschiderea fisierului: " << filename << "\n";
        exit(1);
    }
    return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

int main() {
    // ---------------------------------------------------------------
    // 1. Citire fisier (echivalentul rank 0 din MPI)
    // ---------------------------------------------------------------
    const string text = readFile("input_large.txt");
    const int totalSize = static_cast<int>(text.size());

    // Numarul de thread-uri disponibile (echivalentul numarului de procese MPI)
    const int numThreads = static_cast<int>(thread::hardware_concurrency());

    cout << "Dimensiune input: " << totalSize << " bytes\n";
    cout << "Numar thread-uri: " << numThreads << "\n";

    if (totalSize == 0) {
        cout << "Fisier gol\n";
        return 0;
    }

    // ---------------------------------------------------------------
    // 2. Impartire text in chunk-uri (echivalentul MPI_Scatterv)
    //    Fiecare chunk corespunde unui "proces" MPI.
    // ---------------------------------------------------------------
    const int chunkSize = (totalSize + numThreads - 1) / numThreads;

    // Indici chunk-uri: 0, 1, ..., numThreads-1
    vector<int> chunkIndices(numThreads);
    iota(chunkIndices.begin(), chunkIndices.end(), 0);

    // ---------------------------------------------------------------
    // 3. Calcul frecvente locale in paralel (echivalentul calculului
    //    local din fiecare proces MPI, inainte de MPI_Reduce)
    // ---------------------------------------------------------------
    // Fiecare chunk are propriul array de frecvente pentru a evita
    // race conditions (la fel cum fiecare proces MPI are localFreq[256])
    vector<array<int, 256>> chunkFreqs(numThreads);

    for_each(execution::par, chunkIndices.begin(), chunkIndices.end(),
        [&](int i) {
            const int start = i * chunkSize;
            const int end = min(start + chunkSize, totalSize);

            chunkFreqs[i].fill(0);
            for (int j = start; j < end; j++)
                chunkFreqs[i][static_cast<unsigned char>(text[j])]++;
        });

    // ---------------------------------------------------------------
    // 4. Reducere frecvente globale (echivalentul MPI_Reduce + MPI_Bcast)
    //    Insumam frecventele locale din toate chunk-urile.
    // ---------------------------------------------------------------
    array<int, 256> globalFreq{};
    globalFreq.fill(0);

    for (const auto& cf : chunkFreqs)
        for (int i = 0; i < 256; i++)
            globalFreq[i] += cf[i];

    // ---------------------------------------------------------------
    // 5. Construire arbore Huffman (identic cu varianta MPI;
    //    fiecare proces/thread foloseste aceleasi frecvente globale)
    // ---------------------------------------------------------------
    Node* root = buildHuffmanTreeFromFreq(globalFreq);
    if (!root) {
        cout << "Nu s-a putut construi arborele Huffman\n";
        return 0;
    }

    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // ---------------------------------------------------------------
    // 6. Compresie paralela (echivalentul compresiei locale din MPI,
    //    inainte de MPI_Gather)
    //    Fiecare chunk isi comprima independent portia de text.
    // ---------------------------------------------------------------
    vector<string> encodedChunks(numThreads);

    // Sincronizare inainte de masurare (echivalentul MPI_Barrier)
    // (nu este necesara explicit, for_each de sus garanteaza finalizarea)

    const auto startTime = high_resolution_clock::now();

    for_each(execution::par, chunkIndices.begin(), chunkIndices.end(),
        [&](int i) {
            const int start = i * chunkSize;
            const int end = min(start + chunkSize, totalSize);

            string encoded;
            encoded.reserve(static_cast<size_t>(end - start) * 8);

            for (int j = start; j < end; j++)
                encoded += huffmanCode.at(text[j]);

            encodedChunks[i] = move(encoded);
        });

    const auto endTime = high_resolution_clock::now();
    const double elapsed = duration_cast<duration<double>>(endTime - startTime).count();

    // ---------------------------------------------------------------
    // 7. Afisare rezultate (echivalentul rank 0 dupa MPI_Gather)
    // ---------------------------------------------------------------
    cout << fixed << setprecision(8);
    cout << "Timp paralel STL: " << elapsed << " secunde\n";

    // Calcul dimensiune totala dupa compresie (echivalentul MPI_Gather pe sizes)
    long long totalBits = 0;
    for (const auto& ec : encodedChunks)
        totalBits += static_cast<long long>(ec.size());

    cout << "Dimensiune dupa compresie: " << totalBits / 8 << " bytes\n";

    // ---------------------------------------------------------------
    // 8. Eliberare memorie
    // ---------------------------------------------------------------
    freeTree(root);

    return 0;
}