# Proiect APD – Compresia Datelor

## 1. Tema și cerințele proiectului

Tema proiectului este **Compresia Datelor (Data Compression)**, un proces prin care dimensiunea datelor este redusă prin eliminarea redundanței informației. Compresia este importantă pentru economisirea spațiului de stocare și pentru transmiterea mai eficientă a datelor.

În cadrul proiectului a fost studiat și implementat algoritmul **Huffman Coding**, un algoritm de compresie fără pierderi, care atribuie coduri binare mai scurte caracterelor care apar mai frecvent și coduri mai lungi caracterelor rare.

Proiectul a fost realizat în limbajul **C++**.

În prima etapă a fost implementată varianta **secvențială**, iar ulterior algoritmul a fost extins într-o variantă **paralelă** folosind **MPI (Message Passing Interface)**.

Obiectivele proiectului sunt:
- implementarea algoritmului de compresie Huffman;
- măsurarea timpilor de execuție;
- analizarea comportamentului algoritmului pe fișiere de dimensiuni diferite;
- obținerea unei baze de comparație între varianta secvențială și varianta paralelă.

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

Măsurarea timpilor de execuție s-a realizat folosind funcționalități standard din C++ (`chrono`) pentru varianta secvențială, iar pentru fiecare fișier de input s-au efectuat **5 rulări**, calculându-se apoi timpul mediu.

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

## 4. Varianta paralelă folosind MPI

După implementarea variantei secvențiale, algoritmul a fost extins într-o variantă paralelă folosind **MPI**. În această abordare, fișierul este împărțit între mai multe procese, iar fiecare proces calculează frecvențele locale pentru bucata primită. Frecvențele locale sunt apoi combinate pentru a obține frecvențele globale, pe baza cărora se construiește arborele Huffman.

După construirea arborelui, fiecare proces comprimă în paralel partea sa de date. Rezultatul final este obținut prin însumarea dimensiunilor fragmentelor comprimate.

În implementarea realizată au fost folosite **8 procese**.

### 4.1. Rezultate pentru `input_small.txt`

| Parametru | Valoare |
|---|---:|
| Dimensiune input | 1759 bytes |
| Numar procese | 8 |
| Timp paralel | 0.00007590 secunde |
| Dimensiune dupa compresie | 913 bytes |

---

### 4.2. Rezultate pentru `input_medium.txt`

| Parametru | Valoare |
|---|---:|
| Dimensiune input | 35040000 bytes |
| Numar procese | 8 |
| Timp paralel | 0.66854240 secunde |
| Dimensiune dupa compresie | 18122500 bytes |

---

### 4.3. Rezultate pentru `input_large.txt`

| Parametru | Valoare |
|---|---:|
| Dimensiune input | 350400000 bytes |
| Numar procese | 8 |
| Timp paralel | 7.21449790 secunde |
| Dimensiune dupa compresie | 181225000 bytes |

---

## 5. Observații

Rezultatele arată că timpul de execuție crește odată cu dimensiunea inputului, ceea ce este un comportament așteptat pentru un algoritm de compresie care procesează întregul conținut al fișierului.

Pe fișiere mici, timpul de rulare este foarte redus, iar variațiile dintre execuții sunt cauzate de factorii normali ai sistemului de operare și ai mediului de execuție.

Pentru fișiere mari, timpul de execuție devine semnificativ mai mare, ceea ce justifică interesul pentru o variantă paralelă a algoritmului.

În varianta MPI se observă o îmbunătățire a timpului de execuție, deoarece calculul frecvențelor și compresia sunt distribuite între mai multe procese. Totuși, există și costuri de comunicare între procese, motiv pentru care accelerarea nu este perfect proporțională cu numărul de procese.

---

## 6. Concluzie

Proiectul demonstrează implementarea unei soluții de compresie a datelor folosind algoritmul Huffman în C++. Au fost realizate atât varianta secvențială, cât și varianta paralelă folosind MPI.

Testele efectuate pe fișiere de dimensiuni diferite evidențiază creșterea timpului de execuție odată cu volumul datelor în varianta secvențială, respectiv reducerea timpului de procesare în varianta paralelă.

Rezultatele obținute oferă o bază bună pentru compararea performanțelor dintre cele două abordări și confirmă faptul că paralelizarea poate fi utilă în special pentru fișiere de dimensiuni mari.

