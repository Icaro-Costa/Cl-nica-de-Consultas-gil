#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PACIENTES 100
#define MAX_CONSULTAS 100
#define ARQUIVO_PACIENTES "pacientes.txt"
#define ARQUIVO_CONSULTAS "consultas.txt"

typedef struct {
    int dia;
    int hora;
    char especialidade[50];
    int paciente_index; 
} Consulta;

typedef struct {
    char nome[50];
    char telefone[15];
    int num_consultas;
} Paciente;

Paciente pacientes[MAX_PACIENTES];
Consulta consultas[MAX_CONSULTAS];
int total_pacientes = 0;
int total_consultas = 0;

int ler_inteiro(const char *prompt, int min, int max) {
    int valor;
    printf("%s", prompt);
    while (1) {
        if (scanf("%d", &valor) != 1 || valor < min || valor > max) {
            printf("Entrada invalida. Tente novamente.\n");
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            return valor;
        }
    }
}

int verificar_telefone_existente(const char *telefone) {
    for (int i = 0; i < total_pacientes; i++) {
        if (strcmp(pacientes[i].telefone, telefone) == 0) {
            return 1;
        }
    }
    return 0;
}

void salvar_pacientes_em_arquivo() {
    FILE *arquivo = fopen(ARQUIVO_PACIENTES, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", ARQUIVO_PACIENTES);
        return;
    }

    for (int i = 0; i < total_pacientes; i++) {
        fprintf(arquivo, "%s;%s;%d\n", pacientes[i].nome, pacientes[i].telefone, pacientes[i].num_consultas);
    }

    fclose(arquivo);
}

void carregar_pacientes_do_arquivo() {
    FILE *arquivo = fopen(ARQUIVO_PACIENTES, "r");
    if (arquivo == NULL) {
        printf("Arquivo %s nao encontrado. Criando um novo.\n", ARQUIVO_PACIENTES);
        return;
    }

    total_pacientes = 0;
    while (fscanf(arquivo, "%[^;];%[^;];%d\n", pacientes[total_pacientes].nome,
                  pacientes[total_pacientes].telefone, &pacientes[total_pacientes].num_consultas) == 3) {
        total_pacientes++;
        if (total_pacientes >= MAX_PACIENTES) {
            printf("Limite maximo de pacientes atingido.\n");
            break;
        }
    }

    fclose(arquivo);
}

void cadastrar_paciente() {
    if (total_pacientes == MAX_PACIENTES) {
        printf("Limite de pacientes cadastrados atingido!\n");
        return;
    }

    char nome[50];
    char telefone[15];

    printf("Digite o nome do paciente: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Digite o telefone do paciente: ");
    fgets(telefone, sizeof(telefone), stdin);
    telefone[strcspn(telefone, "\n")] = '\0'; 

    
    if (verificar_telefone_existente(telefone)) {
        printf("Paciente ja cadastrado!\n");
        return;
    }

    strcpy(pacientes[total_pacientes].nome, nome);
    strcpy(pacientes[total_pacientes].telefone, telefone);
    pacientes[total_pacientes].num_consultas = 0;
    total_pacientes++;

    printf("Paciente cadastrado com sucesso!\n");

    salvar_pacientes_em_arquivo();
}

int verificar_data_hora_valida(int dia, int hora) {
    time_t agora;
    time(&agora);
    struct tm *tempo_atual = localtime(&agora);

    int hora_atual = tempo_atual->tm_hour;

    if (dia < tempo_atual->tm_mday || (dia == tempo_atual->tm_mday && hora <= hora_atual)) {
        return 0;
    }

    return 1;
}

void salvar_consultas_em_arquivo() {
    FILE *arquivo = fopen(ARQUIVO_CONSULTAS, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", ARQUIVO_CONSULTAS);
        return;
    }

    for (int i = 0; i < total_consultas; i++) {
        fprintf(arquivo, "%d;%d;%s;%d\n", consultas[i].dia, consultas[i].hora,
                consultas[i].especialidade, consultas[i].paciente_index);
    }

    fclose(arquivo);
}

void marcar_consulta() {
    if (total_pacientes == 0) {
        printf("Nenhum paciente cadastrado!\n");
        return;
    }

    printf("Lista de Pacientes:\n");
    for (int i = 0; i < total_pacientes; i++) {
        printf("%d. %s - %s\n", i + 1, pacientes[i].nome, pacientes[i].telefone);
    }

    int escolha_paciente = ler_inteiro("Escolha o numero correspondente ao paciente: ", 1, total_pacientes) - 1;

    int dia = ler_inteiro("Digite o dia da consulta (1-31): ", 1, 31);
    int hora = ler_inteiro("Digite a hora da consulta (0-23): ", 0, 23);

    if (!verificar_data_hora_valida(dia, hora)) {
        printf("Nao e possivel agendar consultas em datas retroativas!\n");
        return;
    }

    char especialidade[50];
    printf("Digite a especialidade da consulta: ");
    fgets(especialidade, sizeof(especialidade), stdin);
    especialidade[strcspn(especialidade, "\n")] = '\0'; 

    for (int i = 0; i < total_consultas; i++) {
        if (consultas[i].dia == dia && consultas[i].hora == hora) {
            printf("Este horario ja esta ocupado para outra consulta!\n");
            return;
        }
    }

    consultas[total_consultas].dia = dia;
    consultas[total_consultas].hora = hora;
    strcpy(consultas[total_consultas].especialidade, especialidade);
    consultas[total_consultas].paciente_index = escolha_paciente;
    total_consultas++;

    pacientes[escolha_paciente].num_consultas++;

    printf("Consulta marcada com sucesso!\n");

    salvar_consultas_em_arquivo();
}

void cancelar_consulta() {
    if (total_consultas == 0) {
        printf("Nenhuma consulta agendada!\n");
        return;
    }

    printf("Lista de Consultas Agendadas:\n");
    for (int i = 0; i < total_consultas; i++) {
        int index_paciente = consultas[i].paciente_index;
        printf("%d. Paciente: %s - Dia: %d, Hora: %d, Especialidade: %s\n", i + 1,
               pacientes[index_paciente].nome, consultas[i].dia, consultas[i].hora,
               consultas[i].especialidade);
    }

    int escolha = ler_inteiro("Escolha o numero correspondente a consulta a ser cancelada: ", 1, total_consultas) - 1;

    int index_paciente = consultas[escolha].paciente_index;
    printf("Consulta selecionada para cancelamento:\n");
    printf("Paciente: %s - Dia: %d, Hora: %d, Especialidade: %s\n",
           pacientes[index_paciente].nome, consultas[escolha].dia, consultas[escolha].hora,
           consultas[escolha].especialidade);

    int confirmacao = ler_inteiro("Confirmar cancelamento (1 - Sim, 0 - Nao): ", 0, 1);
    if (confirmacao == 1) {
        for (int i = escolha; i < total_consultas - 1; i++) {
            consultas[i] = consultas[i + 1];
        }
        total_consultas--;

        pacientes[index_paciente].num_consultas--;

        printf("Consulta cancelada com sucesso!\n");

        salvar_consultas_em_arquivo();
    } else {
        printf("Operacao de cancelamento cancelada.\n");
    }
}

void menu() {
    printf("\n===== Menu =====\n");
    printf("1. Cadastrar Paciente\n");
    printf("2. Marcar Consulta\n");
    printf("3. Cancelar Consulta\n");
    printf("0. Sair\n");
}

int main() {
    carregar_pacientes_do_arquivo();

    int opcao;

    do {
        menu();
        opcao = ler_inteiro("Escolha uma opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                cadastrar_paciente();
                break;
            case 2:
                marcar_consulta();
                break;
            case 3:
                cancelar_consulta();
                break;
            case 0:
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
