#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string.h>


struct Aluno {
    char matricula[9];
    char cpf[15];
    char nome[40];
    double nota;
    int idade;
    char curso[40];
    char cidade[40];
};

#define CARGA_MAXIMA 0.7
#define TAMANHO_HASH_INICIAL 1021

struct Alunos {
    Aluno** hash;
    bool* hashOcupada;
    int tamanhoAtual;
    int quantidade;
};

Alunos a;

void inicializa() {
    a.tamanhoAtual = TAMANHO_HASH_INICIAL;
    a.quantidade = 0;
    a.hash = new Aluno*[a.tamanhoAtual];
    a.hashOcupada = new bool[a.tamanhoAtual];
    for (int i = 0; i < a.tamanhoAtual; i++) {
        a.hashOcupada[i] = false;
        a.hash[i] = nullptr;
    }
}

double fatorCarga() {
    return (double)(a.quantidade + 1) / a.tamanhoAtual;
}

bool ehPrimo(int numero) {
    if (numero < 2) {
        return false;
    }
    for (int i = 2; i < numero; i++) {
        if (numero % i == 0) {
            return false;
        }
    }
    return true;
}

int proximoPrimo(int numero) {
    while (!ehPrimo(numero)) {
        numero = numero + 1;
    }
    return numero;
}


int funcaoHashNome(const char* nome) {
    int len = strlen(nome);
    int soma = 0;
    for (int i = 0; i < len; i++) {
        soma += (unsigned char)nome[i];
    }
    return (1LL * soma * soma) % a.tamanhoAtual;
}

void expandirHash() {
    Aluno** hashAntiga = a.hash;
    bool* ocupadaAntiga = a.hashOcupada;
    int tamanhoAntigo = a.tamanhoAtual;
    int tamanhoAntigo2 = tamanhoAntigo * 2;

    int novoTamanho = proximoPrimo(tamanhoAntigo2);

    Aluno** novaHash = new Aluno*[novoTamanho];
    bool* novaOcupada = new bool[novoTamanho];

    for (int i = 0; i < novoTamanho; i++) {
        novaHash[i] = nullptr;
        novaOcupada[i] = false;
    }

    a.tamanhoAtual = novoTamanho;

    for (int i = 0; i < tamanhoAntigo; i++) {
        if (ocupadaAntiga[i] == true) {
            Aluno* alunoTemp = hashAntiga[i];
            int indice = funcaoHashNome(alunoTemp->nome);

            while (novaOcupada[indice] == true) {
                indice = indice + 1;
                if (indice == a.tamanhoAtual) {
                    indice = 0;
                }
            }

            novaHash[indice] = alunoTemp;
            novaOcupada[indice] = true;
        }
    }

    a.hash = novaHash;
    a.hashOcupada = novaOcupada;

    delete[] hashAntiga;
    delete[] ocupadaAntiga;
}

void adicionarAluno(Aluno* novo) {
    if (fatorCarga() > CARGA_MAXIMA) {
        expandirHash();
    }

    int indice = funcaoHashNome(novo->nome);
    while (a.hashOcupada[indice] == true) {
        indice = indice + 1;
        if (indice == a.tamanhoAtual) {
            indice = 0;
        }
    }

    a.hash[indice] = novo;
    a.hashOcupada[indice] = true;
    a.quantidade++;
}

void lerArquivoCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char linha[300];
    printf("Iniciando leitura do arquivo CSV...\n");

    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        printf("Arquivo vazio ou erro na leitura\n");
        fclose(arquivo);
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Aluno* novo = new Aluno;

        int lidos = sscanf(
            linha,
            "%8[^,],%14[^,],%39[^,],%lf,%d,%39[^,],%39[^\n\r]",
            novo->matricula, novo->cpf, novo->nome,
            &novo->nota, &novo->idade, novo->curso, novo->cidade
        );

        if (lidos == 7) {
            adicionarAluno(novo);
        } else {
            delete novo;
        }
    }

    fclose(arquivo);
    printf("Leitura concluida. Total de alunos: %d\n", a.quantidade);
}

Aluno* buscarPorNome(const char* nome) {
    int indice = funcaoHashNome(nome);
    int inicio = indice;

    while (a.hashOcupada[indice] == true) {
        if (strcmp(a.hash[indice]->nome, nome) == 0) {
            return a.hash[indice];
        }

        indice = indice + 1;
        if (indice == a.tamanhoAtual) {
            indice = 0;
        }
        if (indice == inicio) {
            break;
        }
    }
    return nullptr;
}


void liberarMemoria() {
    for (int i = 0; i < a.tamanhoAtual; i++) {
        if (a.hashOcupada[i] == true) {
            delete a.hash[i];
        }
    }
    delete[] a.hash;
    delete[] a.hashOcupada;
    a.hash = nullptr;
    a.hashOcupada = nullptr;
    a.quantidade = 0;
}

int main() {
    inicializa();
    printf("=== SISTEMA DE LEITURA DE ALUNOS CSV ===\n\n");

    
    clock_t inicio = clock();
    lerArquivoCSV("aluno.csv");
    clock_t fim = clock();

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de leitura e insercao: %.3f segundos (tempo de CPU)\n", tempo);
    printf("Tamanho final do vetor: %d\n", a.tamanhoAtual);
    printf("Fator de carga final: %.4f\n", (double)a.quantidade / a.tamanhoAtual);

    // Teste simples de busca por nome, sem mudar a logica da funcao de busca.
    char nomeBusca[40];
    printf("\nDigite um nome para buscar (Enter para pular): ");
    if (fgets(nomeBusca, sizeof(nomeBusca), stdin) != NULL) {
        nomeBusca[strcspn(nomeBusca, "\n")] = '\0';
        if (nomeBusca[0] != '\0') {
            Aluno* encontrado = buscarPorNome(nomeBusca);
            if (encontrado != nullptr) {
                printf("Aluno encontrado: %s | CPF: %s | Matricula: %s\n",
                       encontrado->nome, encontrado->cpf, encontrado->matricula);
            } else {
                printf("Aluno nao encontrado.\n");
            }
        }
    }

    liberarMemoria();
    return 0;
}
