#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>


struct Aluno {
    
    char matricula[9];
    char cpf[15];
    char nome[40];
    double nota;
    int idade;
    char curso[40];
    char cidade[40];
};

struct NoAluno {
    Aluno* aluno;
    NoAluno* pai;
    NoAluno* dir;
    NoAluno* esq;
    int altura;
    int grau;
    int nivel;
};

struct Arvore {
    Aluno** vetor;
    int capacidade;
    int quantidadeElementosDeAlunos;
    int nivelMaximo;
};

Arvore a;

void inicializa() {
    a.capacidade=1048576;
    a.vetor = new Aluno*[a.capacidade]();
    

    a.nivelMaximo = 0;
    a.quantidadeElementosDeAlunos = 0;
}


bool adicionarAluno(Aluno* novo) {
    int atual = 0;
    int nivel = 0;

    while (atual < a.capacidade && a.vetor[atual] != NULL) {

        if (strcmp(a.vetor[atual]->nome, novo->nome) > 0) {

            
            if (atual > (a.capacidade - 2) / 2) {
                return false;
            }

            atual = 2 * atual + 1;

        } else {

            
            if (atual > (a.capacidade - 3) / 2) {
                return false;
            }

            atual = 2 * atual + 2;
        }
        nivel++;
    }

    

    if (atual < a.capacidade) {
        a.vetor[atual] = novo;
        a.quantidadeElementosDeAlunos++;

        if (nivel > a.nivelMaximo) {
        a.nivelMaximo = nivel;
        }
        return true;
    } else {
        return false;
    }
}

    


Aluno* buscarAlunoPorNome(const char* nome) {
    int atual = 0;

    while (atual < a.capacidade && a.vetor[atual] != NULL) {

        int comparacao = strcmp(a.vetor[atual]->nome, nome);

        if (comparacao == 0) {
            return a.vetor[atual];
        }
        else if (comparacao > 0) {
            atual = 2 * atual + 1;
        }
        else {
            atual = 2 * atual + 2;
        }
    }

    return NULL;
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
        printf("Arquivo vazio ou erro na leitura.\n");
        fclose(arquivo);
        return;
    }

    
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Aluno* novo = new Aluno;

        int campos = sscanf(
            linha,
            "%8[^,],%14[^,],%39[^,],%lf,%d,%39[^,],%39[^\r\n]",
            novo->matricula,
            novo->cpf,
            novo->nome,
            &novo->nota,
            &novo->idade,
            novo->curso,
            novo->cidade
        );

        if (campos == 7) {
            if (!adicionarAluno(novo)) {
                delete novo;

                printf("Limite do vetor atingido!\n");
                printf("Alunos inseridos: %d\n",
                    a.quantidadeElementosDeAlunos);

                break;
            }
        } else {
            delete novo;
        }
    }

    fclose(arquivo);

    
    printf(
        "Leitura concluida. Total de alunos: %d\n",
        a.quantidadeElementosDeAlunos
    );
}


void exibirAluno(Aluno* aluno, int numero) {
    printf("Aluno %d:\n", numero);
    printf("  Matricula: %s\n", aluno->matricula);
    printf("  CPF: %s\n", aluno->cpf);
    printf("  Nome: %s\n", aluno->nome);
    printf("  Nota: %.2f\n", aluno->nota);
    printf("  Idade: %d\n", aluno->idade);
    printf("  Curso: %s\n", aluno->curso);
    printf("  Cidade: %s\n", aluno->cidade);
    printf("  ---\n");
}




void liberarArvore() {
    for (int i = 0; i < a.capacidade; i++) {
        if (a.vetor[i] != NULL) {
            delete a.vetor[i];
        }
    }

    delete[] a.vetor;
    a.vetor = NULL;
}

int main() {
    inicializa();

    printf("=== SISTEMA DE ARVORE BINARIA DE ALUNOS ===\n\n");

    
    clock_t inicio = clock();

    lerArquivoCSV("aluno.csv");

    clock_t fim = clock();

    
    double tempoSegundos =
        (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\n=== RESULTADOS ===\n");
    printf("Quantidade de alunos: %d\n",
           a.quantidadeElementosDeAlunos);

    printf("Nivel maximo da arvore: %d\n",
           a.nivelMaximo);

   
    int quantidadeNiveis =
    a.quantidadeElementosDeAlunos == 0 ? 0 : a.nivelMaximo + 1;

    printf("Quantidade de niveis: %d\n",
           quantidadeNiveis);

    printf("Tempo de leitura e insercao: %.3f segundos\n",
           tempoSegundos);

    printf("Tempo de leitura e insercao: %.2f milissegundos\n",
           tempoSegundos * 1000.0);

  
    char nomeBusca[40];

    printf("\nDigite o nome completo do aluno para buscar: ");

    if (fgets(nomeBusca, sizeof(nomeBusca), stdin) != NULL) {

       
        nomeBusca[strcspn(nomeBusca, "\r\n")] = '\0';

        Aluno* encontrado = buscarAlunoPorNome(nomeBusca);

        if (encontrado != NULL) {
            printf("\nAluno encontrado!\n");
            printf("Nome: %s\n", encontrado->nome);
            printf("CPF: %s\n", encontrado->cpf);
            printf("Matricula: %s\n", encontrado->matricula);
            printf("Curso: %s\n", encontrado->curso);
        }
        else {
            printf("\nAluno nao encontrado.\n");
        }
    }


    liberarArvore();

    return 0;
}
