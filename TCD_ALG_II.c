#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>  // Windows apenas
#else
    #include <time.h>     // Linux/Mac
#endif

// =======================
// ESTRUTURAS DE DADOS
// =======================
typedef struct {
    int *dados;
    int tamanho;
    int capacidade;
} Vetor;

typedef struct {
    double tempo_bubble;
    double tempo_insertion;
    double tempo_selection;
    double tempo_merge;
    double tempo_quick;
    double tempo_shell;
    double tempo_busca_linear;
    double tempo_busca_binaria;
    int total_execucoes;

    // flags para indicar quais algoritmos foram executados
    int run_bubble;
    int run_insertion;
    int run_selection;
    int run_merge;
    int run_quick;
    int run_shell;
    int run_busca_linear;
    int run_busca_binaria;
} Resultados;

typedef struct {
    char nome_arquivo[256];
    int tamanho;
} DadosPrincipais;

// =======================
// VARIÁVEIS GLOBAIS
// =======================
Vetor vetor_dados = {NULL, 0, 0};
Resultados resultados = {0};
DadosPrincipais dados_principais = {"<nenhum>", 0};
int arquivo_carregado = 0;
#define EXECUCOES_TESTE 100

// =======================
// FUNÇÕES AUXILIARES
// =======================
double tempo_agora_ms() {
#ifdef _WIN32
    LARGE_INTEGER freq, cont;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cont);
    return (cont.QuadPart * 1000.0) / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1.0e6);
#endif
}

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int esta_ordenado() {
    if (!arquivo_carregado || vetor_dados.tamanho <= 1) return 1;
    for (int i = 1; i < vetor_dados.tamanho; i++) {
        if (vetor_dados.dados[i] < vetor_dados.dados[i - 1]) {
            return 0;
        }
    }
    return 1;
}

// =======================
// CRIAÇÃO E CARREGAMENTO
// =======================
void criar_arquivo_com_conteudo(const char* nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo.\n");
        return;
    }

    printf("Digite os numeros a serem salvos no arquivo.\n");
    printf("Digite um caractere nao numerico para finalizar.\n");

    int num;
    while (1) {
        printf("Numero: ");
        if (scanf("%d", &num) != 1) {
            limpar_buffer();
            break;
        }
        fprintf(arquivo, "%d\n", num);
    }

    fclose(arquivo);
    printf("Arquivo '%s' criado com sucesso!\n", nome_arquivo);
}

int carregar_arquivo() {
    char nome_arquivo[100];
    FILE *arquivo;
    int numero;

    if (vetor_dados.dados != NULL) {
        free(vetor_dados.dados);
        vetor_dados.dados = NULL;
    }

    printf("Digite o nome do arquivo: ");
    if (scanf("%99s", nome_arquivo) != 1) {
        limpar_buffer();
        printf("Nome invalido.\n");
        return 0;
    }
    limpar_buffer();

    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Arquivo '%s' nao encontrado!\n", nome_arquivo);
        printf("Deseja criar um novo arquivo com esse nome? (s/n): ");
        char resp = getchar();
        limpar_buffer();
        if (resp == 's' || resp == 'S') {
            criar_arquivo_com_conteudo(nome_arquivo);
            arquivo = fopen(nome_arquivo, "r");
            if (arquivo == NULL) {
                printf("Erro ao abrir o arquivo apos criacao.\n");
                return 0;
            }
        } else {
            return 0;
        }
    }

    vetor_dados.capacidade = 10;
    vetor_dados.tamanho = 0;
    vetor_dados.dados = malloc(vetor_dados.capacidade * sizeof(int));
    if (!vetor_dados.dados) {
        printf("Erro de alocacao.\n");
        fclose(arquivo);
        return 0;
    }

    while (fscanf(arquivo, "%d", &numero) == 1) {
        if (vetor_dados.tamanho >= vetor_dados.capacidade) {
            vetor_dados.capacidade *= 2;
            int *tmp = realloc(vetor_dados.dados, vetor_dados.capacidade * sizeof(int));
            if (!tmp) {
                printf("Erro de realocacao.\n");
                free(vetor_dados.dados);
                fclose(arquivo);
                return 0;
            }
            vetor_dados.dados = tmp;
        }
        vetor_dados.dados[vetor_dados.tamanho] = numero;
        vetor_dados.tamanho++;
    }

    fclose(arquivo);
    arquivo_carregado = 1;

    strncpy(dados_principais.nome_arquivo, nome_arquivo, sizeof(dados_principais.nome_arquivo)-1);
    dados_principais.nome_arquivo[sizeof(dados_principais.nome_arquivo)-1] = '\0';
    dados_principais.tamanho = vetor_dados.tamanho;

    printf("Arquivo carregado com sucesso! %d elementos lidos.\n", vetor_dados.tamanho);
    return 1;
}

// =======================
// BUSCAS
// =======================
int busca_linear_uma_passada(int arr[], int n, int numero_a_encontrar) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == numero_a_encontrar) return i;
    }
    return -1;
}

int busca_binaria_uma_passada(int arr[], int n, int numero_a_encontrar) {
    int ini = 0, fim = n - 1, meio;
    while (ini <= fim) {
        meio = (ini + fim) / 2;
        if (arr[meio] == numero_a_encontrar) return meio;
        else if (arr[meio] < numero_a_encontrar) ini = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

// =======================
// ALGORITMOS DE ORDENAÇÃO
// =======================
void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void insertion_sort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void selection_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min]) min = j;
        }
        if (min != i) {
            int temp = arr[i];
            arr[i] = arr[min];
            arr[min] = temp;
        }
    }
}

void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    if (!L || !R) {
        if (L) free(L);
        if (R) free(R);
        return;
    }

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort_rec(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_rec(arr, l, m);
        merge_sort_rec(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge_sort(int arr[], int n) {
    if (n > 0) merge_sort_rec(arr, 0, n - 1);
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
        }
    }
    int t = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = t;
    return i + 1;
}

void quick_sort_rec(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_rec(arr, low, pi - 1);
        quick_sort_rec(arr, pi + 1, high);
    }
}

void quick_sort(int arr[], int n) {
    if (n > 0) quick_sort_rec(arr, 0, n - 1);
}

void shell_sort(int arr[], int n) {
    for (int gap = n/2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
}

// =======================
// MEDIDORES DE TEMPO (médias)
// =======================

double medir_tempo_sort(void (*func)(int*, int), int arr[], int n) {
    if (n <= 0) return 0.0;
    double soma = 0.0;
    for (int i = 0; i < EXECUCOES_TESTE; i++) {
        int *copia = malloc(n * sizeof(int));
        if (!copia) return 0.0;
        memcpy(copia, arr, n * sizeof(int));
        double inicio = tempo_agora_ms();
        func(copia, n);
        double fim = tempo_agora_ms();
        soma += (fim - inicio);
        free(copia);
    }
    return soma / EXECUCOES_TESTE;
}

double medir_tempo_busca_linear(int arr[], int n, int chave) {
    if (n <= 0) return 0.0;
    double soma = 0.0;
    for (int i = 0; i < EXECUCOES_TESTE; i++) {
        double inicio = tempo_agora_ms();
        busca_linear_uma_passada(arr, n, chave);
        double fim = tempo_agora_ms();
        soma += (fim - inicio);
    }
    return soma / EXECUCOES_TESTE;
}

double medir_tempo_busca_binaria(int arr[], int n, int chave) {
    if (n <= 0) return 0.0;
    // binary search precisa do vetor ordenado - criamos uma copia ordenada
    int *copia = malloc(n * sizeof(int));
    if (!copia) return 0.0;
    memcpy(copia, arr, n * sizeof(int));
    // escolhemos quick_sort para ordenar a copia antes das execucoes
    quick_sort(copia, n);

    double soma = 0.0;
    for (int i = 0; i < EXECUCOES_TESTE; i++) {
        double inicio = tempo_agora_ms();
        busca_binaria_uma_passada(copia, n, chave);
        double fim = tempo_agora_ms();
        soma += (fim - inicio);
    }
    free(copia);
    return soma / EXECUCOES_TESTE;
}

// =======================
// MENU DE ORDENAÇÃO
// =======================
void menu_de_ordenacao() {
    if (!arquivo_carregado) {
        printf("Nao tem um arquivo carregado!\n");
        return;
    }

    int opcao;
    printf("\n=== MENU DE ORDENACAO ===\n");
    printf("1. Bubble Sort\n");
    printf("2. Insertion Sort\n");
    printf("3. Selection Sort\n");
    printf("4. Merge Sort\n");
    printf("5. Quick Sort\n");
    printf("6. Shell Sort (EXTRA)\n");
    printf("7. Voltar\n");
    printf("Digite sua opcao: ");
    if (scanf("%d", &opcao) != 1) { limpar_buffer(); return; }
    limpar_buffer();

    double inicio, fim, tempo;
    int *copia = malloc(vetor_dados.tamanho * sizeof(int));
    if (!copia) { printf("Erro de alocacao.\n"); return; }
    memcpy(copia, vetor_dados.dados, vetor_dados.tamanho * sizeof(int));

    switch(opcao) {
        case 1:
            inicio = tempo_agora_ms();
            bubble_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_bubble = tempo;
            resultados.run_bubble = 1;
            printf("Bubble Sort: %.10f ms\n", resultados.tempo_bubble);
            break;
        case 2:
            inicio = tempo_agora_ms();
            insertion_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_insertion = tempo;
            resultados.run_insertion = 1;
            printf("Insertion Sort: %.10f ms\n", resultados.tempo_insertion);
            break;
        case 3:
            inicio = tempo_agora_ms();
            selection_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_selection = tempo;
            resultados.run_selection = 1;
            printf("Selection Sort: %.10f ms\n", resultados.tempo_selection);
            break;
        case 4:
            inicio = tempo_agora_ms();
            merge_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_merge = tempo;
            resultados.run_merge = 1;
            printf("Merge Sort: %.10f ms\n", resultados.tempo_merge);
            break;
        case 5:
            inicio = tempo_agora_ms();
            quick_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_quick = tempo;
            resultados.run_quick = 1;
            printf("Quick Sort: %.10f ms\n", resultados.tempo_quick);
            break;
        case 6:
            inicio = tempo_agora_ms();
            shell_sort(copia, vetor_dados.tamanho);
            fim = tempo_agora_ms();
            tempo = fim - inicio;
            resultados.tempo_shell = tempo;
            resultados.run_shell = 1;
            printf("Shell Sort (EXTRA): %.10f ms\n", resultados.tempo_shell);
            break;
        case 7:
            free(copia);
            return;
        default:
            printf("Opcao invalida.\n");
            free(copia);
            return;
    }
    resultados.total_execucoes++;
    free(copia);
}

// =======================
// MENU DE BUSCA
// =======================
void menu_de_busca() {
    if (!arquivo_carregado) {
        printf("Nao tem um arquivo carregado!\n");
        return;
    }

    int opcao, numero;
    printf("\n=== MENU DE BUSCA ===\n");
    printf("1. Busca Linear\n");
    printf("2. Busca Binaria\n");
    printf("3. Voltar\n");
    printf("Digite: ");
    if (scanf("%d", &opcao) != 1) { limpar_buffer(); return; }
    limpar_buffer();

    if (opcao == 1 || opcao == 2) {
        printf("Numero a buscar: ");
        if (scanf("%d", &numero) != 1) { limpar_buffer(); printf("Numero invalido.\n"); return; }
        limpar_buffer();
        if (opcao == 1) {
            double inicio = tempo_agora_ms();
            int pos = busca_linear_uma_passada(vetor_dados.dados, vetor_dados.tamanho, numero);
            double fim = tempo_agora_ms();
            resultados.tempo_busca_linear = fim - inicio;
            resultados.run_busca_linear = 1;
            if (pos >= 0) printf("Numero %d encontrado na posicao %d\n", numero, pos);
            else printf("Numero %d nao encontrado.\n", numero);
            printf("Tempo decorrido (uma execucao): %.10f ms\n", resultados.tempo_busca_linear);
            resultados.total_execucoes++;
        } else {
            if (!esta_ordenado()) {
                printf("Vetor nao esta ordenado. A busca binaria precisa que o vetor esteja ordenado.\n");
                printf("Deseja ordenar o vetor com Quick Sort temporariamente para realizar a busca? (s/n): ");
                char r = getchar(); limpar_buffer();
                if (r != 's' && r != 'S') return;
            }
            // cria copia ordenada para buscar
            int *copia = malloc(vetor_dados.tamanho * sizeof(int));
            if (!copia) { printf("Erro de alocacao.\n"); return; }
            memcpy(copia, vetor_dados.dados, vetor_dados.tamanho * sizeof(int));
            quick_sort(copia, vetor_dados.tamanho);

            double inicio = tempo_agora_ms();
            int pos = busca_binaria_uma_passada(copia, vetor_dados.tamanho, numero);
            double fim = tempo_agora_ms();
            resultados.tempo_busca_binaria = fim - inicio;
            resultados.run_busca_binaria = 1;
            if (pos >= 0) printf("Numero %d encontrado na posicao %d (na copia ordenada)\n", numero, pos);
            else printf("Numero %d nao encontrado.\n", numero);
            printf("Tempo decorrido (uma execucao): %.10f ms\n", resultados.tempo_busca_binaria);
            resultados.total_execucoes++;
            free(copia);
        }
    }
}

// =======================
// EXECUTAR TODOS TESTES
// =======================
void executar_todos_testes() {
    if (!arquivo_carregado) {
        printf("Nao tem um arquivo carregado!\n");
        return;
    }
    printf("Executando testes (cada algoritmo sera executado %d vezes para calcular a media)...\n", EXECUCOES_TESTE);

    // sorts
    resultados.tempo_bubble = medir_tempo_sort(bubble_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_bubble = 1;

    resultados.tempo_insertion = medir_tempo_sort(insertion_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_insertion = 1;

    resultados.tempo_selection = medir_tempo_sort(selection_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_selection = 1;

    resultados.tempo_merge = medir_tempo_sort(merge_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_merge = 1;

    resultados.tempo_quick = medir_tempo_sort(quick_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_quick = 1;

    resultados.tempo_shell = medir_tempo_sort(shell_sort, vetor_dados.dados, vetor_dados.tamanho);
    resultados.run_shell = 1;

    // buscas: pedimos um numero para testar
    int chave;
    printf("Digite um numero para testar as buscas (sera usado nas %d execucoes): ", EXECUCOES_TESTE);
    if (scanf("%d", &chave) != 1) { limpar_buffer(); printf("Numero invalido. Pulando testes de busca.\n"); }
    else {
        limpar_buffer();
        resultados.tempo_busca_linear = medir_tempo_busca_linear(vetor_dados.dados, vetor_dados.tamanho, chave);
        resultados.run_busca_linear = 1;
        resultados.tempo_busca_binaria = medir_tempo_busca_binaria(vetor_dados.dados, vetor_dados.tamanho, chave);
        resultados.run_busca_binaria = 1;
    }

    resultados.total_execucoes += 1; // Indica que foi rodado o conjunto de testes (para relatorio)

    printf("Testes concluidos. Use a opcao 'Gerar relatorio' para salvar os resultados em relatorio.txt\n");
}

// =======================
// RELATÓRIO
// =======================
void gerar_relatorio() {
    if (resultados.total_execucoes == 0 &&
        !resultados.run_bubble &&
        !resultados.run_busca_linear &&
        !resultados.run_busca_binaria) {
        printf("Erro: Nenhum algoritmo executado! Execute testes ou rode algum algoritmo antes de gerar relatorio.\n");
        return;
    }

    FILE *f = fopen("relatorio.txt", "w");
    if (f == NULL) {
        printf("Erro ao criar relatório!\n");
        return;
    }

    // timestamp
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char buffer_data[64];
    strftime(buffer_data, sizeof(buffer_data), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(f, "=== RELATORIO DE DESEMPENHO ===\n");
    fprintf(f, "Data/Hora: %s\n", buffer_data);
    fprintf(f, "Arquivo: %s\n", dados_principais.nome_arquivo);
    fprintf(f, "Elementos: %d\n", dados_principais.tamanho);
    fprintf(f, "Execucoes para media por algoritmo: %d\n\n", EXECUCOES_TESTE);

    fprintf(f, "--- RESUMO DOS ALGORITMOS DE ORDENACAO EXECUTADOS ---\n");
    if (resultados.run_bubble) fprintf(f, "Bubble Sort: media = %.10f ms\n", resultados.tempo_bubble);
    if (resultados.run_insertion) fprintf(f, "Insertion Sort: media = %.10f ms\n", resultados.tempo_insertion);
    if (resultados.run_selection) fprintf(f, "Selection Sort: media = %.10f ms\n", resultados.tempo_selection);
    if (resultados.run_merge) fprintf(f, "Merge Sort: media = %.10f ms\n", resultados.tempo_merge);
    if (resultados.run_quick) fprintf(f, "Quick Sort: media = %.10f ms\n", resultados.tempo_quick);
    if (resultados.run_shell) fprintf(f, "Shell Sort (EXTRA): media = %.10f ms\n", resultados.tempo_shell);

    fprintf(f, "\n--- RESUMO DAS BUSCAS EXECUTADAS ---\n");
    if (resultados.run_busca_linear) fprintf(f, "Busca Linear: media = %.10f ms\n", resultados.tempo_busca_linear);
    if (resultados.run_busca_binaria) fprintf(f, "Busca Binaria: media = %.10f ms\n", resultados.tempo_busca_binaria);
    if (resultados.run_busca_linear && resultados.run_busca_binaria) {
        double ratio = (resultados.tempo_busca_binaria > 0.0) ?
                       (resultados.tempo_busca_linear / resultados.tempo_busca_binaria) : 0.0;
        fprintf(f, "Busca Linear foi %.2fx mais lenta que Busca Binaria (media).\n", ratio);
    }

    fprintf(f, "\n--- DETALHES E METADADOS ---\n");
    fprintf(f, "Total de conjuntos de testes executados: %d\n", resultados.total_execucoes);
    fprintf(f, "Vetor inicialmente carregado: %s\n", dados_principais.nome_arquivo);
    fprintf(f, "Vetor estava inicialmente ordenado? %s\n", esta_ordenado() ? "SIM" : "NAO");

    fprintf(f, "\nArquivos gerados durante a execucao:\n");
    fprintf(f, " - relatorio.txt (este arquivo)\n");

        fprintf(f, "\nPorque o algoritmo extra (Shell Sort) foi incluido:\n");
    fprintf(f, " - O Shell Sort foi escolhido como algoritmo extra por ser uma evolucao do Insertion Sort.\n");
    fprintf(f, " - Ele utiliza o conceito de 'gaps' (intervalos), permitindo que elementos distantes sejam comparados e trocados,\n");
    fprintf(f, "   diminuindo rapidamente grandes desalinhamentos.\n");
    fprintf(f, " - Isso reduz o numero de movimentacoes em comparacao com Bubble, Selection e Insertion Sort.\n");
    fprintf(f, " - Embora a complexidade dependa da escolha da sequencia de gaps, na pratica o Shell Sort costuma ser muito mais rapido\n");
    fprintf(f, "   que os algoritmos simples e pode se aproximar do desempenho de algoritmos como Merge e Quick Sort.\n");
    fprintf(f, " - Por esse motivo, ele e util como 'algoritmo extra' para mostrar uma solucao intermediaria: mais eficiente que os algoritmos basicos,\n");
    fprintf(f, "   mas ainda mais simples de implementar que os algoritmos mais sofisticados.\n");


    fclose(f);

    printf("Relatorio gerado em 'relatorio.txt'!\n");

    #ifdef _WIN32
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "notepad.exe \"%s\"", "relatorio.txt");
        system(cmd);
    #endif
}


// =======================
// MENUS E MAIN
// =======================
void exibir_menu_principal() {
    printf("\n==========================================\n");
    printf("           MENU PRINCIPAL\n");
    printf("==========================================\n");
    printf("1. Carregar arquivo de dados\n");
    printf("2. Buscar elementos\n");
    printf("3. Ordenar dados\n");
    printf("4. Gerar relatorio\n");
    printf("6. Executar todos os testes e armazenar resultados\n", EXECUCOES_TESTE);
    printf("5. Sair\n");
    printf("==========================================\n");
}

int main() {
    int opcao;
    printf("=== TCD - ALGORITMOS E PROGRAMACAO II (VERSAO CORRIGIDA) ===\n");

    do {
        limpar_tela();
        exibir_menu_principal();
        printf("Digite sua opcao: ");
        if (scanf("%d", &opcao) != 1) { limpar_buffer(); continue; }
        limpar_buffer();

        switch(opcao) {
            case 1:
                carregar_arquivo();
                break;
            case 2:
                menu_de_busca();
                break;
            case 3:
                menu_de_ordenacao();
                break;
            case 4:
                gerar_relatorio();
                break;
            case 5:
                printf("Encerrando...\n");
                break;
            case 6:
                executar_todos_testes();
                break;
            default:
                printf("Opcao invalida!\n");
        }

        if (opcao != 5) {
            printf("Pressione ENTER para continuar...");
            getchar();
        }
    } while (opcao != 5);

    if (vetor_dados.dados) {
        free(vetor_dados.dados);
    }

    return 0;
}
