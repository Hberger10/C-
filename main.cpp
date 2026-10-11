#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <cstring>
#include <cctype>
#include <chrono>


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

Alunos* tabelas = NULL;
int tamanho =0;
int modoHash = 0;


const char* arquivoCSV = "alunoss.csv";


char nomesBusca[10][40];
char matriculasBusca[10][9];
int totalAmostras = 0;

int funcaoHash(const char* cpf) {
    int len = strlen(cpf);
    int d1 = -1, d2 = -1;

    
    for (int i = len - 1; i >= 0; i--) {
        if (isdigit(cpf[i])) {
            if (d2 == -1) {
                d2 = cpf[i] - '0'; 
            } else if (d1 == -1) {
                d1 = cpf[i] - '0'; 
                break;
            }
        }
    } 
    if (d1 == -1 || d2 == -1) return 0;

    return (d1 * 10) + d2; 
}

int funcaoHash2(const char* cpf){
    int len = strlen(cpf);
    int resultado = 0;
    int peso = 1;
    int pegos = 0;

    for (int i = len - 1; i >= 0 && pegos < 3; i--) {
        if (isdigit(cpf[i])) {
            resultado += (cpf[i] - '0') * peso;
            peso *= 10;
            pegos++;
        }
    }
    return resultado;  
}
int funcaoHash3(const char* cpf){
    int len = strlen(cpf);
    int resultado = 0;
    int peso = 1;
    int pegos = 0;

    for (int i = len - 1; i >= 0 && pegos < 4; i--) {
        if (isdigit(cpf[i])) {
            resultado += (cpf[i] - '0') * peso;
            peso *= 10;
            pegos++;
        }
    }
    return resultado;  
}

int funcaoHash4(const char* cpf){
    int len = strlen(cpf);
    int resultado = 0;
    int peso = 1;
    int pegos = 0;

    for (int i = len - 1; i >= 0 && pegos < 5; i--) {
        if (isdigit(cpf[i])) {
            resultado += (cpf[i] - '0') * peso;
            peso *= 10;
            pegos++;
        }
    }
    return resultado;  
}

void configurarTabela(){

    
    do {
        printf("Escolha o hash:\n");
        printf("1 - 100 indices\n");
        printf("2 - 1000 indices\n");
        printf("3 - 10000 indices\n");
        printf("4 - 100000 indices\n");
        printf("Escolha: ");
        scanf("%d", &modoHash);
    } while (modoHash < 1 || modoHash > 4);

    
    switch (modoHash){
        case 1:
            tamanho = 100;
            break;
        case 2:
            tamanho = 1000;
            break;
        case 3:
            tamanho = 10000;
            break;
        case 4:
            tamanho = 100000;
            break;
    }

    
    tabelas = new Alunos[tamanho];
}



void inicializa(){
    for (int i=0;i<tamanho;i++){
        tabelas[i].inicio = NULL;
        tabelas[i].fim = NULL;
        tabelas[i].quantidade = 0;
    } 
}

bool existeDuplicado( int indice ,const char* cpf) { // no indice eu não utilizo ponteiro pois quero apenas ler.´porém no cpf precisa pois vetor de char não é copiado o que viaja é o endereço 
    Aluno* atual =  tabelas[indice].inicio;
    while (atual != NULL){
        if (strcmp(atual->cpf,cpf)==0){
            return true;
        }
        atual= atual->prox;
        
    }
    return false;
    
}




void adicionarAluno(Aluno* novo) {
    int indice =0;
    if (modoHash == 1){
        indice = funcaoHash(novo->cpf);

    } else if(modoHash == 2){
        indice = funcaoHash2(novo->cpf);
    } else if(modoHash == 3){
        indice = funcaoHash3(novo->cpf);
    } else if(modoHash == 4){
        indice = funcaoHash4(novo->cpf);
    }




    
    if(existeDuplicado(indice,novo->cpf)){
        delete novo;
        return;
    }

    

    Aluno* atual = tabelas[indice].inicio;
    Aluno* anterior = NULL;

    if (atual == NULL) {

        novo->ante = NULL;
        novo->prox = NULL;
        tabelas[indice].inicio = novo;
        tabelas[indice].fim = novo;
    } else {
        
        while (atual != NULL && strcmp(novo->nome, atual->nome) > 0) {
            anterior = atual;
            atual = atual->prox;
        }

        if (anterior == NULL) {
            
            novo->prox = atual;
            novo->ante = NULL;
            atual->ante = novo;
            tabelas[indice].inicio = novo;
        } else if (atual == NULL) {
            
            novo->prox = NULL;
            novo->ante = anterior;
            anterior->prox = novo;
            tabelas[indice].fim = novo;
        } else {
            
            novo->prox = atual;
            novo->ante = anterior;
            anterior->prox = novo;
            atual->ante = novo;
        }
    }

    tabelas[indice].quantidade++;
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
    
    int contador = 1;
    int i=0;
   

    for(i=0;i<tamanho;i++){
        Aluno* atual = tabelas[i].inicio;
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
        

    }
    
    
    printf("Total: %d alunos\n\n", contador -1);
}


void salvarCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s para escrita\n", nomeArquivo);
        return;
    }

    
    fprintf(arquivo, "matricula,cpf,nome,nota,idade,curso,cidade\n");

    int salvos = 0;   

    for (int i = 0; i < tamanho; i++) {                 
        Aluno* atual = tabelas[i].inicio;            
        while (atual != NULL) {                      
            fprintf(arquivo, "%s,%s,%s,%.2f,%d,%s,%s\n",
                    atual->matricula, atual->cpf, atual->nome,
                    atual->nota, atual->idade, atual->curso, atual->cidade);
            atual = atual->prox;                    
            salvos++;
        }
    }

    fclose(arquivo);   
    printf("Arquivo %s atualizado. %d alunos salvos.\n", nomeArquivo, salvos);
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
    int indice =0;
    if (modoHash == 1){
        indice = funcaoHash(alvo->cpf);

    } else if(modoHash == 2){
        indice = funcaoHash2(alvo->cpf);
    } else if(modoHash == 3){
        indice = funcaoHash3(alvo->cpf);
    } else if(modoHash == 4){
        indice = funcaoHash4(alvo->cpf);
    }

    

    if (alvo->ante != NULL) {
        alvo->ante->prox = alvo->prox;
    } else {
        tabelas[indice].inicio = alvo->prox;
    }

    if (alvo->prox != NULL) {
        alvo->prox->ante = alvo->ante;
    } else {
        tabelas[indice].fim = alvo->ante;
    }

    delete alvo;
    tabelas[indice].quantidade--;
    printf("Aluno removido com sucesso!\n");

    salvarCSV("aluno.csv");
}

void buscarPorMatricula() {
    char matriculaBusca[9];

    getchar();

    printf("Digite a matricula a buscar: ");
    lerLinha(matriculaBusca, sizeof(matriculaBusca));

    
    Aluno* atual = tabelas[0].inicio;
    for (int i = 0; i < tamanho; i++) {
        atual = tabelas[i].inicio;
        while (atual != NULL && strcmp(atual->matricula, matriculaBusca) != 0) {
            atual = atual->prox;
        }
        if (atual != NULL) break;  
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
    int indice =0;
    if (modoHash == 1){
        indice = funcaoHash(cpfBusca);

    } else if(modoHash == 2){
        indice = funcaoHash2(cpfBusca);
    } else if(modoHash == 3){
        indice = funcaoHash3(cpfBusca);
    } else if(modoHash == 4){
        indice = funcaoHash4(cpfBusca);
    }

     


    Aluno* atual = tabelas[indice].inicio;
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

// busca automatica
Aluno* procurarPorNome(const char* nome) {
    for (int i = 0; i < tamanho; i++) {
        Aluno* atual = tabelas[i].inicio;
        while (atual != NULL) {
            if (strcmp(atual->nome, nome) == 0) {
                return atual;
            }
            atual = atual->prox;
        }
    }
    return NULL;
}


Aluno* procurarPorMatricula(const char* matricula) {
    for (int i = 0; i < tamanho; i++) {
        Aluno* atual = tabelas[i].inicio;
        while (atual != NULL) {
            if (strcmp(atual->matricula, matricula) == 0) {
                return atual;
            }
            atual = atual->prox;
        }
    }
    return NULL;
}


void carregarAmostras() {
    totalAmostras = 0;

    FILE* arquivo = fopen(arquivoCSV, "r");
    if (arquivo == NULL) return;

    char linha[300];

    
    fgets(linha, sizeof(linha), arquivo);   
    int totalLinhas = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) >= 5) totalLinhas++;
    }

    int passo = totalLinhas / 10;           
    if (passo < 1) passo = 1;

    
    rewind(arquivo);
    fgets(linha, sizeof(linha), arquivo);   
    int numeroLinha = 0;
    while (totalAmostras < 10 && fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) < 5) continue;

        if (numeroLinha % passo == 0) {
            char mat[9], cpf[15], nome[40];
            if (sscanf(linha, "%8[^,],%14[^,],%39[^,]", mat, cpf, nome) == 3) {
                strcpy(matriculasBusca[totalAmostras], mat);
                strcpy(nomesBusca[totalAmostras], nome);
                totalAmostras++;
            }
        }
        numeroLinha++;
    }
    fclose(arquivo);
}

void buscaAutomatica() {
    carregarAmostras();     

    if (totalAmostras == 0) {
        printf("Nao foi possivel ler as amostras de %s\n", arquivoCSV);
        return;
    }

    printf("\n=== BUSCA AUTOMATICA (%d nomes e %d matriculas) ===\n", totalAmostras, totalAmostras);

    
    int achadosNome = 0;
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < totalAmostras; i++) {
        if (procurarPorNome(nomesBusca[i]) != NULL) achadosNome++;
    }
    auto t1 = std::chrono::steady_clock::now();
    long long usNome = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    printf("Nomes:      %d de %d achados | total %lld us | media %.1f us por busca\n",
           achadosNome, totalAmostras, usNome, (double)usNome / totalAmostras);

    
    int achadosMat = 0;
    auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < totalAmostras; i++) {
        if (procurarPorMatricula(matriculasBusca[i]) != NULL) achadosMat++;
    }
    auto t3 = std::chrono::steady_clock::now();
    long long usMat = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

    printf("Matriculas: %d de %d achadas  | total %lld us | media %.1f us por busca\n",
           achadosMat, totalAmostras, usMat, (double)usMat / totalAmostras);
}



int main(){
    configurarTabela();
    inicializa();
    printf("SISTEMA DE LEITURA DE ALUNOS CSV \n\n");

    

    auto t0 = std::chrono::steady_clock::now();
    lerArquivoCSV(arquivoCSV);
    auto t1 = std::chrono::steady_clock::now();

    long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    printf("Tempo de leitura: %lld microssegundos (%.3f ms)\n", us, us / 1000.0);

    int opcao;
    do {
        printf("\n=== MENU ===\n");
        printf("1 - Cadastrar novo aluno\n");
        printf("2 - Exibir todos os alunos\n");
        printf("3 - Buscar por matricula\n");
        printf("4 - Buscar por cpf\n");
        printf("5 - Salvar alteracoes no arquivo\n");
        printf("6 - Busca automatica (10 nomes e 10 matriculas)\n");
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
            case 6:
                buscaAutomatica();
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