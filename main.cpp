
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
    NoAluno* raiz;
    int quantidadeElementosDeAlunos;
    int nivelMaximo;
};

Arvore a;

void inicializa() {
    a.raiz = NULL;
    a.nivelMaximo = 0;
    a.quantidadeElementosDeAlunos = 0;
}


void adicionarAluno(Aluno* novo) {
    NoAluno* no = new NoAluno;

    no->aluno = novo;
    no->pai = NULL;
    no->dir = NULL;
    no->esq = NULL;
    no->altura = 0;
    no->nivel = 0;
    no->grau = 0;

    if (a.raiz == NULL) {
        a.raiz = no;
        a.quantidadeElementosDeAlunos++;
        return;
    }

    NoAluno* atual = a.raiz;
    NoAluno* pai = NULL;

    while (atual != NULL) {
        if (strcmp(atual->aluno->nome, novo->nome) > 0) {
            pai = atual;
            atual = atual->esq;
        }
        else if (strcmp(atual->aluno->nome, novo->nome) == 0) {
            pai = atual;
            atual = atual->dir;
        }
        else {
            pai = atual;
            atual = atual->dir;
        }
    }

    no->pai = pai;

    if (strcmp(pai->aluno->nome, novo->nome) > 0) {
        pai->esq = no;
    }
    else {
        pai->dir = no;
    }

    a.quantidadeElementosDeAlunos++;

    no->nivel = pai->nivel + 1;

    if (no->nivel > a.nivelMaximo) {
        a.nivelMaximo = no->nivel;
    }

    
    pai->grau++;
}


Aluno* buscarAlunoPorNome(const char* nome) {
    NoAluno* atual = a.raiz;

    while (atual != NULL) {
        int comparacao = strcmp(atual->aluno->nome, nome);

        if (comparacao == 0) {
            return atual->aluno;
        }
        else if (comparacao > 0) {
            atual = atual->esq;
        }
        else {
            atual = atual->dir;
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
            adicionarAluno(novo);
        }
        else {
            
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


void exibirAlunos() {
    printf("\n=== ALUNOS EM ORDEM ALFABETICA ===\n");

    
    NoAluno** pilha = new NoAluno*[a.quantidadeElementosDeAlunos];

    int topo = 0;
    int contador = 0;

    NoAluno* atual = a.raiz;

    while (atual != NULL || topo > 0) {

       
        while (atual != NULL) {
            pilha[topo++] = atual;
            atual = atual->esq;
        }

        
        atual = pilha[--topo];

        contador++;
        exibirAluno(atual->aluno, contador);

        
        atual = atual->dir;
    }

    delete[] pilha;

    printf("Total: %d alunos\n\n", contador);
}


void liberarArvore() {
    if (a.raiz == NULL) {
        return;
    }

    NoAluno** pilha = new NoAluno*[a.quantidadeElementosDeAlunos];
    int topo = 0;

    pilha[topo++] = a.raiz;

    while (topo > 0) {
        NoAluno* atual = pilha[--topo];

        if (atual->esq != NULL) {
            pilha[topo++] = atual->esq;
        }

        if (atual->dir != NULL) {
            pilha[topo++] = atual->dir;
        }

        delete atual->aluno;
        delete atual;
    }

    delete[] pilha;

    inicializa();
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
        a.raiz == NULL ? 0 : a.nivelMaximo + 1;

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
