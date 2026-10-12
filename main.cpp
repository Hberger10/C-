// Atividade: multiplas hashes (CPF, matricula e nome)
// Este programa parte do codigo da Atividade 2 (lista duplamente encadeada,
// insercao ordenada e hash por CPF com vetor de 100 posicoes), e nao do
// codigo de exemplo enviado pelo professor, conforme combinado com ele.
// Alteracoes: o Aluno passou a ter um noAluno por lista, ha tres tabelas
// de hash e a funcao inserirAluno e unica, usando ponteiro para funcao.
//
// Medicoes: tempo de leitura/insercao e tempo de cada busca.
// Resultado observado anteriormente (399917 alunos): 203671260 us na leitura.
// Esse valor e apenas referencia de uma execucao anterior; execute novamente.
// Anote aqui os tempos obtidos nesta versao e compare com a Atividade 2.
// A medicao das buscas nao inclui digitacao nem exibicao dos dados.

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
};

struct noAluno{
    Aluno *aluno;
    noAluno *prox;
    noAluno *ante;
};

struct Alunos{
    noAluno *inicio;
    noAluno *fim;
    int quantidade;
};

Alunos tabelasCPF [100];
Alunos tabelasNome [100];
Alunos tabelasMatricula [100];

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

// CORRIGIDO: soma como unsigned char. Letras acentuadas ocupam bytes acima de 127,
// que como "char" comum contam como negativos e geravam indice negativo (ex.: "Ze" com acento dava -58).
int funcaoHashNome(const char* nome) {
    int len = strlen(nome);
    int soma = 0;
    for (int i = 0; i < len; i++) {
        soma += (unsigned char)nome[i];
    }
    return soma % 100;
}

// ALTERADO: os dois ultimos digitos da matricula (mesma ideia da hash do CPF).
// A soma dos caracteres usava so ~55 das 100 posicoes (a soma de digitos tem formato de sino),
// o que deixava algumas listas com mais de 50 mil alunos e tornava a leitura muito lenta.
int funcaoHashMatricula(const char* matricula) {
    return funcaoHash(matricula);
}

const char* chaveCpf(Aluno* al) { return al->cpf; }
const char* chaveMatricula(Aluno* al) { return al->matricula; }
const char* chaveNome(Aluno* al) { return al->nome; }

void inicializa(){
    for (int i=0;i<100;i++){
        tabelasCPF[i].inicio = NULL;
        tabelasCPF[i].fim = NULL;
        tabelasCPF[i].quantidade = 0;
        tabelasNome[i].inicio = NULL;
        tabelasNome[i].fim = NULL;
        tabelasNome[i].quantidade = 0;
        tabelasMatricula[i].inicio = NULL;
        tabelasMatricula[i].fim = NULL;
        tabelasMatricula[i].quantidade = 0;
    }
}

bool existeDuplicado( int indice ,const char* cpf) { // no indice eu não utilizo ponteiro pois quero apenas ler.´porém no cpf precisa pois vetor de char não é copiado o que viaja é o endereço
    noAluno* atual =  tabelasCPF[indice].inicio;
    while (atual != NULL){
        if (strcmp(atual->aluno->cpf,cpf)==0){
            return true;
        }
        atual= atual->prox;
    }
    return false;
}

// Recebe a tabela alvo, o aluno, e os ponteiros para a hash e para a extração da chave
void inserirAluno(Alunos tabela[], Aluno* alunoRef, int (*hash)(const char*), const char* (*chave)(Aluno*)) {
    // 1. Descobre qual é o valor (CPF, Matrícula ou Nome) usando a função chave recebida
    const char* valorChave = chave(alunoRef);
    // 2. Calcula a posição usando a função hash recebida
    int indice = hash(valorChave);
    // 3. Cria um NÓ (noAluno) novo. O Aluno em si não tem mais .prox e .ante!
    noAluno* novoNo = new noAluno;
    novoNo->aluno = alunoRef; // O nó guarda o endereço do aluno
    novoNo->prox = NULL;
    novoNo->ante = NULL;
    // Os ponteiros de caminhamento agora são do tipo noAluno*
    noAluno* atual = tabela[indice].inicio;
    noAluno* anterior = NULL;
    if (atual == NULL) {
        // Caso 1: Lista vazia (mesma lógica de antes, mas trocando 'novo' por 'novoNo')
        tabela[indice].inicio = novoNo;
        tabela[indice].fim = novoNo;
    } else {
        // 4. A comparação usa a função 'chave' dinamicamente no nó atual
        while (atual != NULL && strcmp(valorChave, chave(atual->aluno)) > 0) {
            anterior = atual;
            atual = atual->prox;
        }
        if (anterior == NULL) {
            // Inserção no início
            novoNo->prox = atual;
            novoNo->ante = NULL;
            atual->ante = novoNo;
            tabela[indice].inicio = novoNo;
        } else if (atual == NULL) {
            // Inserção no fim
            novoNo->prox = NULL;
            novoNo->ante = anterior;
            anterior->prox = novoNo;
            tabela[indice].fim = novoNo;
        } else {
            // Inserção no meio
            novoNo->prox = atual;
            novoNo->ante = anterior;
            anterior->prox = novoNo;
            atual->ante = novoNo;
        }
    }
    tabela[indice].quantidade++;
}

// Busca genérica que serve para qualquer uma das 3 tabelas
Aluno* buscarAluno(Alunos tabela[], const char* textoBusca, int (*hash)(const char*), const char* (*chave)(Aluno*)) {
    // 1. Calcula a posição usando a hash específica
    int pos = hash(textoBusca);
    // 2. Pega o início da lista naquela posição
    noAluno* atual = tabela[pos].inicio;
    // 3. Percorre a lista encadeada daquela posição
    while (atual != NULL) {
        // Compara o texto buscado com a chave do aluno atual usando strcmp
        if (strcmp(chave(atual->aluno), textoBusca) == 0) {
            return atual->aluno; // Achou! Retorna o ponteiro do aluno
        }
        atual = atual->prox;
    }
    return NULL; // Não encontrou
}

// NOVO: mostra como os alunos se espalharam pelas 100 posicoes de uma tabela.
// Serve para conferir se a hash espalha bem (muitas posicoes vazias ou uma lista enorme = hash ruim).
void estatisticas(Alunos tabela[], const char* titulo) {
    int total = 0, vazias = 0, maior = 0;
    for (int i = 0; i < 100; i++) {
        total += tabela[i].quantidade;
        if (tabela[i].quantidade == 0) vazias++;
        if (tabela[i].quantidade > maior) maior = tabela[i].quantidade;
    }
    printf("  %-10s total %d | posicoes vazias %d | maior lista %d | media %d\n",
           titulo, total, vazias, maior, total / 100);
}

void lerArquivoCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) return;
    char linha[300];
    // Pula a linha do cabeçalho do CSV
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) < 5) continue;
        Aluno* novo = new Aluno;
        novo->cidade[0] = '\0';
        int lidos = sscanf(linha, "%8[^,],%14[^,],%39[^,],%lf,%d,%39[^,],%39[^\n\r]",
                           novo->matricula, novo->cpf, novo->nome,
                           &novo->nota, &novo->idade, novo->curso, novo->cidade);
        if (lidos >= 6) {
            // 1. Calcula onde o CPF estaria na tabela de CPF
            int indiceCpf = funcaoHash(novo->cpf);
            // 2. Verifica se o CPF já existe (regra exigida pelo professor)
            if (existeDuplicado(indiceCpf, novo->cpf)) {
                delete novo; // Se for duplicado, limpa a memória e descarta
            } else {
                // 3. Se é único, insere o MESMO aluno nas 3 tabelas
                // usando a mesma função genérica, mudando apenas a hash e a chave:
                inserirAluno(tabelasCPF, novo, funcaoHash, chaveCpf);
                inserirAluno(tabelasMatricula, novo, funcaoHashMatricula, chaveMatricula);
                inserirAluno(tabelasNome, novo, funcaoHashNome, chaveNome);
            }
        } else {
            delete novo;
        }
    }
    fclose(arquivo);
}

#if 0
void exibirAlunos() {
    printf("\n=== LISTA DE ALUNOS ===\n");
    int contador = 1;
    int i=0;
    for(i=0;i<100;i++){
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
#endif

void salvarCSV(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o ficheiro %s para escrita\n", nomeArquivo);
        return;
    }
    fprintf(arquivo, "matricula,cpf,nome,nota,idade,curso,cidade\n");
    int salvos = 0;
    // Percorremos apenas a tabela de CPF para evitar duplicar os alunos gravados
    for (int i = 0; i < 100; i++) {
        noAluno* atual = tabelasCPF[i].inicio;
        while (atual != NULL) {
            // Acedemos aos dados do aluno através do ponteiro do nó (atual->aluno->campo)
            fprintf(arquivo, "%s,%s,%s,%.2f,%d,%s,%s\n",
                    atual->aluno->matricula, atual->aluno->cpf, atual->aluno->nome,
                    atual->aluno->nota, atual->aluno->idade, atual->aluno->curso, atual->aluno->cidade);
            atual = atual->prox;
            salvos++;
        }
    }
    fclose(arquivo);
    printf("Ficheiro %s atualizado. %d alunos salvos.\n", nomeArquivo, salvos);
}

void lerLinha(char* destino, int tamanho) {
    fgets(destino, tamanho, stdin);
    int len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    }
}

#if 0
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
#endif

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

#if 0
void removerAluno(Aluno* alvo) {
    int indice = funcaoHash(alvo->cpf);
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
#endif

void buscarPorCpf() {
    char cpfBusca[15];
    getchar();
    printf("Digite o CPF a buscar: ");
    lerLinha(cpfBusca, sizeof(cpfBusca));
    // Inicio da medicao da busca por CPF (sem contar digitacao).
    auto inicioBusca = std::chrono::steady_clock::now();
    Aluno* al = buscarAluno(tabelasCPF, cpfBusca, funcaoHash, chaveCpf);
    auto fimBusca = std::chrono::steady_clock::now();
    // Nanosegundos permitem exibir fracoes de microssegundo em buscas rapidas.
    auto nsBusca = std::chrono::duration_cast<std::chrono::nanoseconds>(fimBusca - inicioBusca).count();
    printf("Tempo da busca por CPF: %.3f microssegundos (%.6f ms)\n",
           nsBusca / 1000.0, nsBusca / 1000000.0);
    if (al == NULL) {
        printf("Aluno nao encontrado.\n");
    } else {
        exibirDadosAluno(al);
    }
}

void buscarPorMatricula() {
    char matriculaBusca[9];
    getchar();
    printf("Digite a matricula a buscar: ");
    lerLinha(matriculaBusca, sizeof(matriculaBusca));
    // Inicio da medicao da busca por matricula (sem contar digitacao).
    auto inicioBusca = std::chrono::steady_clock::now();
    Aluno* al = buscarAluno(tabelasMatricula, matriculaBusca, funcaoHashMatricula, chaveMatricula);
    auto fimBusca = std::chrono::steady_clock::now();
    // Nanosegundos permitem exibir fracoes de microssegundo em buscas rapidas.
    auto nsBusca = std::chrono::duration_cast<std::chrono::nanoseconds>(fimBusca - inicioBusca).count();
    printf("Tempo da busca por matricula: %.3f microssegundos (%.6f ms)\n",
           nsBusca / 1000.0, nsBusca / 1000000.0);
    if (al == NULL) {
        printf("Aluno nao encontrado.\n");
    } else {
        exibirDadosAluno(al);
    }
}

void buscarPorNome() {
    char nomeBusca[40];
    getchar();
    printf("Digite o nome a buscar: ");
    lerLinha(nomeBusca, sizeof(nomeBusca));
    // Inicio da medicao da busca por nome (sem contar digitacao).
    auto inicioBusca = std::chrono::steady_clock::now();
    Aluno* al = buscarAluno(tabelasNome, nomeBusca, funcaoHashNome, chaveNome);
    auto fimBusca = std::chrono::steady_clock::now();
    // Nanosegundos permitem exibir fracoes de microssegundo em buscas rapidas.
    auto nsBusca = std::chrono::duration_cast<std::chrono::nanoseconds>(fimBusca - inicioBusca).count();
    printf("Tempo da busca por nome: %.3f microssegundos (%.6f ms)\n",
           nsBusca / 1000.0, nsBusca / 1000000.0);
    if (al == NULL) {
        printf("Aluno nao encontrado.\n");
    } else {
        exibirDadosAluno(al);
    }
}

// Teste automatico: usa 30 alunos reais, distribuidos ao longo da tabela CPF.
// Repete as mesmas buscas 10 vezes por chave, sem contar leitura do CSV nem printf.
// Nao altera as funcoes de hash, insercao ou busca existentes.
void testarDesempenho() {
    const int MAX_AMOSTRAS = 30;
    const int REPETICOES = 10;
    Aluno* amostras[MAX_AMOSTRAS];
    int total = 0;
    for (int i = 0; i < 100; i++) total += tabelasCPF[i].quantidade;
    if (total == 0) {
        printf("\nTeste automatico: nao ha alunos carregados.\n");
        return;
    }

    // Seleciona ate 30 alunos de diferentes partes do conjunto carregado.
    int quantidade = 0;
    int passo = total / MAX_AMOSTRAS;
    if (passo == 0) passo = 1;
    int posicao = 0;
    for (int i = 0; i < 100 && quantidade < MAX_AMOSTRAS; i++) {
        for (noAluno* atual = tabelasCPF[i].inicio;
             atual != NULL && quantidade < MAX_AMOSTRAS; atual = atual->prox) {
            if (posicao % passo == 0) amostras[quantidade++] = atual->aluno;
            posicao++;
        }
    }

    Alunos* tabelas[] = {tabelasCPF, tabelasMatricula, tabelasNome};
    int (*hashes[])(const char*) = {funcaoHash, funcaoHashMatricula, funcaoHashNome};
    const char* (*chaves[])(Aluno*) = {chaveCpf, chaveMatricula, chaveNome};
    const char* titulos[] = {"CPF", "Matricula", "Nome"};

    printf("\n=== TESTE AUTOMATICO DE DESEMPENHO ===\n");
    printf("Alunos carregados: %d | Amostras: %d | Repeticoes: %d\n",
           total, quantidade, REPETICOES);
    printf("Cada media considera %d buscas reais (sem entrada/saida).\n",
           quantidade * REPETICOES);

    for (int tipo = 0; tipo < 3; tipo++) {
        volatile int encontrados = 0; // impede que o resultado seja ignorado
        auto inicio = std::chrono::steady_clock::now();
        for (int r = 0; r < REPETICOES; r++) {
            for (int j = 0; j < quantidade; j++) {
                const char* texto = chaves[tipo](amostras[j]);
                Aluno* resultado = buscarAluno(tabelas[tipo], texto, hashes[tipo], chaves[tipo]);
                if (resultado != NULL && strcmp(chaves[tipo](resultado), texto) == 0) encontrados++;
            }
        }
        auto fim = std::chrono::steady_clock::now();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(fim - inicio).count();
        int execucoes = quantidade * REPETICOES;
        printf("%-10s | %d/%d encontradas | total %.3f ms | media %.3f us\n",
               titulos[tipo], (int)encontrados, execucoes,
               nanos / 1000000.0, nanos / 1000.0 / execucoes);
    }
    printf("Obs.: os tempos variam conforme computador, compilador e otimizacoes.\n");
}

int main(){
    inicializa();
    printf("SISTEMA DE LEITURA DE ALUNOS CSV \n\n");
    auto t0 = std::chrono::steady_clock::now();
    lerArquivoCSV("aluno.csv");
    auto t1 = std::chrono::steady_clock::now();
    long long us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    printf("Tempo de leitura: %lld microssegundos (%.3f ms)\n", us, us / 1000.0);
    // NOVO: como os alunos ficaram distribuidos em cada tabela
    printf("\nDistribuicao nas tabelas:\n");
    estatisticas(tabelasCPF, "CPF");
    estatisticas(tabelasMatricula, "Matricula");
    estatisticas(tabelasNome, "Nome");
    testarDesempenho(); // executa as 3 buscas automaticamente
    int opcao;
    do {
        printf("\n=== MENU ===\n");
        printf("3 - Buscar por matricula\n");
        printf("4 - Buscar por cpf\n");
        printf("6 - Buscar por nome\n");
        printf("5 - Salvar alteracoes no arquivo\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        switch (opcao) {
            case 3:
                buscarPorMatricula();
                break;
            case 4:
                buscarPorCpf();
                break;
            case 6:
                buscarPorNome();
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
