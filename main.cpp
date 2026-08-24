#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string.h>



struct Aluno{
    char matricula[9];
    char cpf[15];
    char nome[40];
    double nota;
    int idade;
    char curso[40];
    char cidade[40];
    Aluno *prox;
    Aluno *ante;
};

struct Alunos{
    Aluno *inicio;
    Aluno *fim;
    int quantidade;
};

Alunos a;

void inicializa(){
    a.inicio = NULL;
    a.fim = NULL;
    a.quantidade = 0;
}

bool existeDuplicado(const char* matricula, const char* cpf) {
    Aluno* atual = a.inicio;
    while (atual != NULL) {
        if (strcmp(atual->matricula, matricula) == 0 || 
            strcmp(atual->cpf, cpf) == 0) {
            return true;
        }
        atual = atual->prox;
    }
    return false;
}




void adicionarAluno(Aluno* novo) {

    if (existeDuplicado(novo->matricula, novo->cpf)) {
        printf("Erro: matricula ou CPF ja cadastrados\n");
               
        delete novo;
        return;
    }

    Aluno* atual = a.inicio;
    Aluno* anterior = NULL;

    if (a.inicio == NULL) {

        novo->ante = NULL;
        novo->prox = NULL;
        a.inicio = novo;
        a.fim = novo;
    } else {
        
        while (atual != NULL && strcmp(novo->nome, atual->nome) > 0) {
            anterior = atual;
            atual = atual->prox;
        }

        if (anterior == NULL) {
            
            novo->prox = a.inicio;
            novo->ante = NULL;
            a.inicio->ante = novo;
            a.inicio = novo;
        } else if (atual == NULL) {
            
            novo->prox = NULL;
            novo->ante = anterior;
            anterior->prox = novo;
            a.fim = novo;
        } else {
            
            novo->prox = atual;
            novo->ante = anterior;
            anterior->prox = novo;
            atual->ante = novo;
        }
    }

    a.quantidade++;
}


void lerArquivoCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) return;

    char linha[300];
    
    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) < 5) continue; 

        Aluno* novo = new Aluno;
        novo->cidade[0] = '\0';

        
        int lidos = sscanf(linha, "%8[^,],%14[^,],%39[^,],%lf,%d,%39[^,],%39[^\n\r]", 
                           novo->matricula, novo->cpf, novo->nome, 
                           &novo->nota, &novo->idade, novo->curso, novo->cidade);

        if (lidos >= 6) {
            novo->prox = NULL;
            novo->ante = NULL;
            adicionarAluno(novo);
        } else {
            delete novo;
        }
    }
    fclose(arquivo);
}

void exibirAlunos() {
    printf("\n=== LISTA DE ALUNOS ===\n");
    Aluno* atual = a.inicio;
    int contador = 1;
    
    while (atual != NULL) {
        printf("Aluno %d:\n", contador);
        printf("  Matricula: %s\n", atual->matricula);
        printf("  CPF: %s\n", atual->cpf);
        printf("  Nome: %s\n", atual->nome);
        printf("  Nota: %.2f\n", atual->nota);
        printf("  Idade: %d\n", atual->idade);
        printf("  Curso: %s\n", atual->curso);
        printf("  Cidade: %s\n", atual->cidade);
        printf("  ---\n");
        
        atual = atual->prox;
        contador++;
    }
    printf("Total: %d alunos\n\n", a.quantidade);
}


void salvarCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s para escrita\n", nomeArquivo);
        return;
    }

  
    fprintf(arquivo, "matricula,cpf,nome,nota,idade,curso,cidade\n");

    Aluno* atual = a.inicio;
    while (atual != NULL) {
        fprintf(arquivo, "%s,%s,%s,%.2f,%d,%s,%s\n",
                atual->matricula, atual->cpf, atual->nome,
                atual->nota, atual->idade, atual->curso, atual->cidade);
        atual = atual->prox;
    }

    fclose(arquivo);
    printf("Arquivo %s atualizado. %d alunos salvos.\n", nomeArquivo, a.quantidade);
}

void lerLinha(char* destino, int tamanho) {
    fgets(destino, tamanho, stdin);
    
    
    int len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    }
}

void cadastrarAluno(){
    Aluno* novo = new Aluno;

    getchar();

    printf("Digite o nome do aluno: ");
    lerLinha(novo->nome, sizeof(novo->nome));

    printf("Digite a matricula do aluno: ");
    lerLinha(novo->matricula, sizeof(novo->matricula));

    printf("Digite o cpf do aluno: ");
    lerLinha(novo->cpf, sizeof(novo->cpf));

    printf("Digite o curso do aluno: ");
    lerLinha(novo->curso, sizeof(novo->curso));

    printf("Digite a nota do aluno: ");
    scanf("%lf", &novo->nota);

    printf("Digite a idade do aluno: ");
    scanf("%d", &novo->idade);

    getchar();

    printf("Digite a cidade do aluno: ");
    lerLinha(novo->cidade, sizeof(novo->cidade));

    novo->prox = NULL;
    novo->ante = NULL;

    adicionarAluno(novo);

    printf("Aluno registrado\n");

    salvarCSV("aluno.csv");
}


void exibirDadosAluno(Aluno* al) {
    printf("\n=== ALUNO ENCONTRADO ===\n");
    printf("  Matricula: %s\n", al->matricula);
    printf("  CPF: %s\n", al->cpf);
    printf("  Nome: %s\n", al->nome);
    printf("  Nota: %.2f\n", al->nota);
    printf("  Idade: %d\n", al->idade);
    printf("  Curso: %s\n", al->curso);
    printf("  Cidade: %s\n", al->cidade);
}


void removerAluno(Aluno* alvo) {
    if (alvo->ante != NULL) {
        alvo->ante->prox = alvo->prox;
    } else {
        a.inicio = alvo->prox;
    }

    if (alvo->prox != NULL) {
        alvo->prox->ante = alvo->ante;
    } else {
        a.fim = alvo->ante;
    }

    delete alvo;
    a.quantidade--;
    printf("Aluno removido com sucesso!\n");

    salvarCSV("aluno.csv");
}

void buscarPorMatricula() {
    char matriculaBusca[9];

    getchar();

    printf("Digite a matricula a buscar: ");
    lerLinha(matriculaBusca, sizeof(matriculaBusca));

    Aluno* atual = a.inicio;
    while (atual != NULL && strcmp(atual->matricula, matriculaBusca) != 0) {
        atual = atual->prox;
    }

    if (atual == NULL) {
        printf("Aluno nao encontrado.\n");
        return;
    }

    exibirDadosAluno(atual);

    char resposta[10];
    printf("\nDeseja remover este aluno? (s/n): ");
    lerLinha(resposta, sizeof(resposta));

    if (resposta[0] == 's' || resposta[0] == 'S') {
        removerAluno(atual);
    }
}

void buscarPorCpf() {
    char cpfBusca[15];

    getchar();

    printf("Digite o cpf a buscar: ");
    lerLinha(cpfBusca, sizeof(cpfBusca));

    Aluno* atual = a.inicio;
    while (atual != NULL && strcmp(atual->cpf, cpfBusca) != 0) {
        atual = atual->prox;
    }

    if (atual == NULL) {
        printf("Aluno nao encontrado.\n");
        return;
    }

    exibirDadosAluno(atual);

    char resposta[10];
    printf("\nDeseja remover este aluno? (s/n): ");
    lerLinha(resposta, sizeof(resposta));

    if (resposta[0] == 's' || resposta[0] == 'S') {
        removerAluno(atual);
    }
}

int main(){
    inicializa();
    printf("SISTEMA DE LEITURA DE ALUNOS CSV \n\n");
    
    time_t inicio, fim;
    inicio = time(NULL);
    lerArquivoCSV("aluno.csv");
    fim = time(NULL);
    
    double tempo = difftime(fim, inicio);
    printf("Tempo de leitura: %.2f segundos\n", tempo);
    int opcao;
    do {
        printf("\n=== MENU ===\n");
        printf("1 - Cadastrar novo aluno\n");
        printf("2 - Exibir todos os alunos\n");
        printf("3 - Buscar por matricula\n");
        printf("4 - Buscar por cpf\n");
        printf("5 - Salvar alteracoes no arquivo\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1:
                cadastrarAluno();   
                break;
            case 2:
                exibirAlunos();
                break;
            case 3:
                buscarPorMatricula();
                break;
            case 4:
                buscarPorCpf();
                break;
            case 5:
                salvarCSV("aluno.csv");
                break;
            case 0:
                salvarCSV("aluno.csv");
                printf("Encerrando...\n");
                break;
            default:    
                printf("Opcao invalida!\n");
        }
        
    } while (opcao != 0);
    
    return 0;
}