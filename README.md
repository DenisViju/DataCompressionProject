# Proiect APD – Compresia Datelor

## 1. Tema și cerințele proiectului

Tema proiectului este **Compresia Datelor (Data Compression)**, un proces prin care dimensiunea datelor este redusă prin eliminarea redundanței informației. Compresia este importantă pentru economisirea spațiului de stocare și pentru transmiterea mai eficientă a datelor.

În cadrul proiectului a fost studiat și implementat algoritmul **Huffman Coding**, un algoritm de compresie fără pierderi, care atribuie coduri binare mai scurte caracterelor care apar mai frecvent și coduri mai lungi caracterelor rare.

Proiectul a fost realizat în limbajul **C++**.

Varianta implementată în această etapă este cea **secvențială**, iar în etapa următoare algoritmul poate fi extins într-o variantă paralelă folosind **MPI (Message Passing Interface)**.

Obiectivele proiectului sunt:
- implementarea algoritmului de compresie Huffman;
- măsurarea timpilor de execuție;
- analizarea comportamentului algoritmului pe fișiere de dimensiuni diferite;
- obținerea unei baze de comparație pentru varianta paralelă.

---

## 2. Informații despre mașina pe care a fost rulat codul

Codul a fost rulat pe următorul sistem:

| Componentă | Detalii |
|---|---|
| Procesor | 12th Gen Intel(R) Core(TM) i5-12450H (2.00 GHz) |
| Memorie RAM | 16,0 GB (viteză: 3200 MT/s) |
| Stocare | 477 GB (234 GB utilizați din 477 GB) |
| Placă grafică | Intel(R) UHD Graphics (128 MB VRAM dedicat) |
| Sistem de operare | Windows 11 |
| Mediu de dezvoltare | Visual Studio 2022 |

---

## 3. Rezultate experimentale

Măsurarea timpilor de execuție s-a realizat folosind funcționalități standard din C++ (`chrono`), iar pentru fiecare fișier de input s-au efectuat **5 rulări**, calculându-se apoi timpul mediu.

### 3.1. Rezultate pentru `input_small.txt`

| Rulare | Timp de execuție (secunde) |
|---|---:|
| 1 | 0.0007372 |
| 2 | 0.0004420 |
| 3 | 0.0004361 |
| 4 | 0.0004317 |
| 5 | 0.0006384 |

**Timp mediu:** `0.00053708 secunde`

Dimensiune input: `1759 bytes`

---

### 3.2. Rezultate pentru `input_medium.txt`

| Rulare | Timp de execuție (secunde) |
|---|---:|
| 1 | 4.93676 |
| 2 | 5.02607 |
| 3 | 4.96239 |
| 4 | 4.95712 |
| 5 | 4.97931 |

**Timp mediu:** `4.97233 secunde`

Dimensiune input: `35040000 bytes`

---

### 3.3. Rezultate pentru `input_large.txt`

| Rulare | Timp de execuție (secunde) |
|---|---:|
| 1 | 51.5190 |
| 2 | 54.2273 |
| 3 | 52.5410 |
| 4 | 51.6489 |
| 5 | 51.8894 |

**Timp mediu:** `52.3651 secunde`

Dimensiune input: `350400000 bytes`

---

## 4. Observații

Rezultatele arată că timpul de execuție crește odată cu dimensiunea inputului, ceea ce este un comportament așteptat pentru un algoritm de compresie care procesează întregul conținut al fișierului.

Pe fișiere mici, timpul de rulare este foarte redus, iar variațiile dintre execuții sunt cauzate de factorii normali ai sistemului de operare și ai mediului de execuție.

Pentru fișiere mari, timpul de execuție devine semnificativ mai mare, ceea ce justifică interesul pentru o posibilă variantă paralelă a algoritmului.

---

## 5. Concluzie

Proiectul demonstrează implementarea unei soluții secvențiale de compresie a datelor folosind algoritmul Huffman în C++. Testele efectuate pe fișiere de dimensiuni diferite evidențiază creșterea timpului de execuție odată cu volumul datelor. Aceasta oferă o bază bună pentru extinderea proiectului cu o variantă paralelă și pentru compararea performanțelor între cele două abordări.

