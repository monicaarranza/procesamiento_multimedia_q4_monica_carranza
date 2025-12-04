#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <cmath>

namespace fs = std::filesystem;


struct Node {
    char ch;
    int frequency;
    Node *left;
    Node *right;

    Node(char c, int freq, Node* l = nullptr, Node* r = nullptr)
        : ch(c), frequency(freq), left(l), right(r) {}

    ~Node() {
        delete left;
        delete right;
    }

    bool isLeaf() const {
        return !left && !right;
    }
};

struct CompareNodes {
    bool operator()(const Node* l, const Node* r) const {
        return l->frequency > r->frequency; 
    }
};


void generateCodes(Node* root, const std::string& str, std::map<char, std::string>& codes) {
    if (!root) return;
    if (root->isLeaf()) {
        codes[root->ch] = str.empty() ? "1" : str;
        return;
    }
    generateCodes(root->left, str + "0", codes);
    generateCodes(root->right, str + "1", codes);
}

Node* buildHuffmanTree(const std::map<char, int>& frequency) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> minHeap;

    for (const auto& pair : frequency) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    if (minHeap.empty()) return nullptr;

    if (minHeap.size() == 1) {
        Node *root = minHeap.top();
        minHeap.pop();
        return new Node('$', root->frequency, root, nullptr); 
    }

    while (minHeap.size() > 1) {
        Node *left = minHeap.top(); minHeap.pop();
        Node *right = minHeap.top(); minHeap.pop();

        int sum = left->frequency + right->frequency;
        Node *top = new Node('$', sum, left, right); 

        minHeap.push(top);
    }

    return minHeap.top();
}

void deleteTree(Node* root) {
    delete root; 
}


void comprimir(const std::string& archivoEntrada, const std::string& archivoTabla, const std::string& archivoBinario) {
    std::ifstream in(archivoEntrada);
    if (!in.is_open()) {
        std::cerr << "Error abriendo archivo entrada: " << archivoEntrada << std::endl;
        return;
    }

    std::map<char, int> frequencies;
    char c;
    long long totalCharacters = 0;
    
    while (in.get(c)) {
        frequencies[c]++;
        totalCharacters++;
    }
    in.close();

    if (totalCharacters == 0) return;

    Node *root = buildHuffmanTree(frequencies);
    std::map<char, std::string> codes;
    generateCodes(root, "", codes);

    std::ofstream outTable(archivoTabla);
    outTable << totalCharacters << "\n";
    outTable << frequencies.size() << "\n";
    for (const auto& pair : frequencies) {
        outTable << static_cast<int>(pair.first) << " " << pair.second << "\n";
    }
    outTable.close();

    std::ifstream inTexto(archivoEntrada);
    std::ofstream outBin(archivoBinario, std::ios::binary);

    char byteBuffer = 0;
    int bitCount = 0;

    while (inTexto.get(c)) {
        const std::string& codigo = codes[c];
        for (char bit : codigo) {
            byteBuffer <<= 1; 
            if (bit == '1') {
                byteBuffer |= 1; 
            }
            bitCount++;

            if (bitCount == 8) {
                outBin.put(byteBuffer);
                byteBuffer = 0;
                bitCount = 0;
            }
        }
    }

    if (bitCount > 0) {
        byteBuffer <<= (8 - bitCount);
        outBin.put(byteBuffer);
    }

    inTexto.close();
    outBin.close();
    deleteTree(root);
}


void descomprimir(const std::string& archivoTabla, const std::string& archivoBinario, const std::string& archivoSalida) {
    std::ifstream inTable(archivoTabla);
    if (!inTable.is_open()) {
        std::cerr << "Error abriendo archivo tabla: " << archivoTabla << std::endl;
        return;
    }

    long long totalCaracteresReales;
    int tamanoMapa;

    if (!(inTable >> totalCaracteresReales >> tamanoMapa)) return;

    std::map<char, int> frecuencias;
    int asciiCode;
    int freq;

    for (int i = 0; i < tamanoMapa; i++) {
        if (!(inTable >> asciiCode >> freq)) return;
        frecuencias[static_cast<char>(asciiCode)] = freq;
    }
    inTable.close();

    Node* root = buildHuffmanTree(frecuencias);
    if (!root) return;

    std::ifstream inBin(archivoBinario, std::ios::binary);
    std::ofstream outSalida(archivoSalida);

    Node *actual = root;
    long long caracteresEncontrados = 0;
    char byteLeido;

    while (inBin.get(byteLeido)) {
        for (int i = 7; i >= 0; i--) {
            if (caracteresEncontrados >= totalCaracteresReales) break;

            int bit = (byteLeido >> i) & 1;

            if (bit == 0) {
                actual = actual->left;
            } else {
                actual = actual->right;
            }

            if (actual && actual->isLeaf()) {
                outSalida.put(actual->ch);
                caracteresEncontrados++;
                actual = root;
            }
        }
    }

    inBin.close();
    outSalida.close();
    deleteTree(root);
}


bool verificarArchivos(const std::string& original, const std::string& descomprimido) {
    std::ifstream file1(original, std::ios::binary);
    std::ifstream file2(descomprimido, std::ios::binary);

    if (!file1.is_open() || !file2.is_open()) {
        std::cerr << "Error: No se pudo abrir uno de los archivos para la verificación." << std::endl;
        return false;
    }

    try {
        if (fs::file_size(original) != fs::file_size(descomprimido)) {
            std::cout << "Verificación fallida: Los archivos tienen tamaños diferentes." << std::endl;
            return false;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error al obtener el tamaño de archivo: " << e.what() << std::endl;
    }


    char c1, c2;
    long long byteCount = 0;
    while (file1.get(c1) && file2.get(c2)) {
        if (c1 != c2) {
            std::cout << "Verificación fallida: Diferencia en el byte #" << byteCount << std::endl;
            return false;
        }
        byteCount++;
    }

    if (file1.eof() && file2.eof()) {
        std::cout << "Verificación exitosa: El archivo original y el descomprimido son idénticos." << std::endl;
        return true;
    } else {
        std::cout << "Verificación fallida: Longitud de archivo diferente." << std::endl;
        return false;
    }
}


void mostrarDiferenciaTamanos(const std::string& original, const std::string& comprimido, const std::string& tabla) {
    try {
        long long sizeOriginal = fs::file_size(original);
        long long sizeComprimido = fs::file_size(comprimido);
        long long sizeTabla = fs::file_size(tabla);
        long long sizeTotal = sizeComprimido + sizeTabla;

        std::cout << "\n--- Reporte de Compresión ---\n";
        std::cout << "Archivo Original:            " << sizeOriginal << " bytes\n";
        std::cout << "Archivo Comprimido:          " << sizeComprimido << " bytes\n";
        std::cout << "Archivo Tabla (Metadata):    " << sizeTabla << " bytes\n";
        
        std::cout << "TOTAL (Comprimido + Tabla):  " << sizeTotal << " bytes\n";

        if (sizeOriginal > 0) {
            double ratio = static_cast<double>(sizeTotal) / sizeOriginal;
            double porcentaje = (1.0 - ratio) * 100.0;
            std::cout << "Factor de Compresión:        " << (sizeOriginal / static_cast<double>(sizeTotal)) << "x\n";
            std::cout << "Ahorro de espacio:           " << (porcentaje > 0 ? "" : "-") << std::abs(porcentaje) << "%\n";
        } else {
            std::cout << "No se puede calcular el factor de compresión (archivo original vacío).\n";
        }
       

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error al acceder al sistema de archivos para el reporte de tamaño: " << e.what() << std::endl;
    }
}


int main() {
    const std::string originalFile = "input_original.txt";
    const std::string tableFile = "huffman_tabla.txt";
    const std::string compressedFile = "output_comprimido.bin";
    const std::string decompressedFile = "output_resultado.txt";

    std::ofstream f(originalFile);
    f << "Este es un ejemplo de texto para la compresion de Huffman. "
      << "Huffman es un algoritmo de codificacion sin perdida que utiliza "
      << "frecuencias para crear codigos de longitud variable, siendo "
      << "mas corto para caracteres mas frecuentes. La 'e' y la 'a' son "
      << "muy comunes en el espanol, por lo que tendran codigos cortos.";
    f.close();
    std::cout << "Archivo de prueba '" << originalFile << "' creado.\n\n";

    int opcion;
    do {
        std::cout << "\n=== Menu Compresor Huffman ===\n";
        std::cout << "1. Comprimir Archivo (Generar binario y tabla)\n";
        std::cout << "2. Descomprimir Archivo (Generar resultado.txt)\n";
        std::cout << "3. Comprimir, Descomprimir y Verificar\n";
        std::cout << "4. Salir\n";
        std::cout << "Seleccione una opción: ";
        
        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcion = 0; 
        }

        switch (opcion) {
            case 1:
                std::cout << "\n[1. Compresión]\n";
                comprimir(originalFile, tableFile, compressedFile);
                mostrarDiferenciaTamanos(originalFile, compressedFile, tableFile);
                break;
            case 2:
                std::cout << "\n[2. Descompresión]\n";
                descomprimir(tableFile, compressedFile, decompressedFile);
                break;
            case 3:
                std::cout << "\n[3. Compresión, Descompresión y Verificación]\n";
                
                std::cout << "\n--- Paso 1: Comprimiendo... ---\n";
                comprimir(originalFile, tableFile, compressedFile);
                mostrarDiferenciaTamanos(originalFile, compressedFile, tableFile);
                
                std::cout << "\n--- Paso 2: Descomprimiendo... ---\n";
                descomprimir(tableFile, compressedFile, decompressedFile);

                std::cout << "\n--- Paso 3: Verificando el resultado... ---\n";
                verificarArchivos(originalFile, decompressedFile);
                break;
            case 4:
                std::cout << "Saliendo.\n";
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo.\n";
                break;
        }
    } while (opcion != 4);

    return 0;
}