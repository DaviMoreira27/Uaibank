#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

#define NUM_DIGITS 15
#define MAX_NAME_LENGTH 100
#define FILE_NAME "../uaibank.txt"
#define LOG_FILE "../log.txt"

typedef struct
{
    char id[NUM_DIGITS + 1];
    char name[100];
    int age;
    double balance;
} User;

int isNameValid(const char *name)
{
    return strlen(name) <= MAX_NAME_LENGTH;
}

void logMessage(const char *message)
{
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo de log.\n");
        return;
    }

    fprintf(file, "%s\n", message);

    fclose(file);

    printf("%s\n", message);
}

char *randomId()
{
    char message[255];
    char *id = (char *)malloc((NUM_DIGITS + 1) * sizeof(char));
    if (id == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Nao foi possivel gerar um ID");
        logMessage(message);
        return NULL;
    }
    srand(time(NULL));
    id[0] = '1' + rand() % 9;

    for (int i = 1; i < NUM_DIGITS; i++)
    {
        id[i] = '0' + rand() % 10;
    }

    id[NUM_DIGITS] = '\0';
    return id;
}

void insertUser(char *id, const char *name, int age, double balance)
{
    char message[255];
    if (!isNameValid(name))
    {
        fprintf(stderr, "Erro: Nome excede %d caracteres.\n", MAX_NAME_LENGTH);
        return;
    }

    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Erro ao salvar os dados");
        logMessage(message);
        return;
    }

    fprintf(file, "\"%s\" \"%s\" \"%d\" \"%.2f\"\n", id, name, age, balance);
    snprintf(message, sizeof(message), "Usuario inserido com id %s.", id);
    logMessage(message);
    fclose(file);
}

void transferMoney(const char *senderId, const char *receiverId, double amount)
{
    char message[255];
    User user;

    // Abrir o arquivo original para leitura
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Erro ao abrir o arquivo");
        logMessage(message);
        return;
    }

    // Abrir um arquivo temporário para escrita
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Erro ao criar um arquivo temporario");
        logMessage(message);
        fclose(file);
        return;
    }

    char line[1024];
    int senderFound = 0;
    int receiverFound = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", user.id, user.name, &user.age, &user.balance) == 4)
        {
            if (strcmp(user.id, senderId) == 0)
            {
                senderFound = 1;
                if (user.balance >= amount)
                {
                    user.balance -= amount;
                }
                else
                {
                    snprintf(message, sizeof(message), "Erro: O usuario id %s, nao possui saldo o suficiente", senderId);
                    logMessage(message);
                    fclose(file);
                    fclose(tempFile);
                    remove("temp.txt");
                    return;
                }
            }
            else if (strcmp(user.id, receiverId) == 0)
            {
                receiverFound = 1;
                user.balance += amount;
            }

            fprintf(tempFile, "\"%s\" \"%s\" \"%d\" \"%.2f\"\n", user.id, user.name, user.age, user.balance);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (senderFound && receiverFound)
    {
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
        snprintf(message, sizeof(message), "Transferencia de %.2f do usuario %s para o usuario %s, foi feita com sucesso", amount, senderId, receiverId);
        logMessage(message);
    }
    else
    {
        remove("temp.txt");
        if (!senderFound)
        {
            snprintf(message, sizeof(message), "Erro: ID do remetente %s, nao foi encontrado", senderId);
            logMessage(message);
        }
        if (!receiverFound)
        {
            snprintf(message, sizeof(message), "Erro: ID do destinatario %s, nao foi encontrado", receiverId);
            logMessage(message);
        }
    }
}

void getUserById(const char *id)
{
    char message[255];
    User user;
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Erro ao abrir o arquivo");
        logMessage(message);
        return;
    }

    char line[1024];

    while (fgets(line, sizeof(line), file))

    {
        if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", user.id, user.name, &user.age, &user.balance) == 4)
        {

            if (strcmp(user.id, id) == 0)
            {
                printf("ID: %s\n", user.id);
                printf("Nome: %s\n", user.name);
                printf("Idade: %d\n", user.age);
                printf("Saldo: %.2f\n", user.balance);

                snprintf(message, sizeof(message), "Usuario do ID %s tem saldo de %.2f", user.id, user.balance);
                logMessage(message);
                fclose(file);
                return;
            }
        }
    }

    snprintf(message, sizeof(message), "Erro: ID %s nao encontrado", id);
    logMessage(message);
    fclose(file);
}

void deleteUser(const char *id)
{
    char message[255];
    User user;
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        snprintf(message, sizeof(message), "Erro: Erro ao abrir o arquivo");
        logMessage(message);
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        //Erro ao criar o arquivo temporario, devido a permissões por exemplo
        snprintf(message, sizeof(message), "Erro: Nao foi possivel excluir o usuario %s", id);
        logMessage(message);
        fclose(file);
        return;
    }

    char line[1024];

    int found = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", user.id, user.name, &user.age, &user.balance) == 4)
        {
            if (strcmp(user.id, id) == 0)
            {
                found = 1;
            }else{
                fputs(line, tempFile);
            }
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found)
    {
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
        snprintf(message, sizeof(message), "Usuario %s removido com sucesso", id);
        logMessage(message);
    }
    else
    {
        remove("temp.txt");
        snprintf(message, sizeof(message), "Erro: Usuario %s nao encontrado", id);
        logMessage(message);
    }
}

void getUserDetails(char *name, int *age, double *balance)
{
    printf("Digite o nome do usuario: ");
    if (fgets(name, MAX_NAME_LENGTH + 1, stdin) == NULL)
    {
        fprintf(stderr, "Erro ao ler nome do usuario\n");
        exit(1);
    }
    name[strcspn(name, "\n")] = '\0';

    printf("Digite a idade do usuario: ");
    if (scanf("%d", age) != 1)
    {
        fprintf(stderr, "Erro ao ler idade do usuario\n");
        exit(1);
    }

    printf("Digite o saldo atual do usuario: ");
    if (scanf("%lf", balance) != 1)
    {
        fprintf(stderr, "Erro ao ler saldo do usuario\n");
        exit(1);
    }
    getchar();
}

int insertMultiplesUsersMenu()
{
    int numUsers;

    printf("Digite o numero de usuarios a serem adicionados: ");
    if (scanf("%d", &numUsers) != 1 || numUsers <= 0)
    {
        fprintf(stderr, "Erro: O numero deve ser um inteiro positivo.\n");
        exit(1);
    }
    getchar();

    for (int i = 0; i < numUsers; i++)
    {
        char name[MAX_NAME_LENGTH + 1];
        int age;
        double balance;

        getUserDetails(name, &age, &balance);

        char *id = randomId();
        if (id == NULL)
        {
            fprintf(stderr, "Erro ao gerar ID para o usuario %d\n", i + 1);
            continue;
        }

        insertUser(id, name, age, balance);
        free(id);
    }

    return 0;
}

int main()
{
    int opcao;
    char userName[MAX_NAME_LENGTH + 1];
    int age;
    double balance;

    do
    {
        printf("\n=== MENU ===\n");
        printf("1. Pesquisar usuario por ID\n");
        printf("2. Deletar usuario por ID\n");
        printf("3. Inserir um unico usuario\n");
        printf("4. Inserir varios usuarios\n");
        printf("5. Realizar transferencia\n");
        printf("6. Sair\n");

        printf("Escolha uma opcao (1-6): ");
        if (scanf("%d", &opcao) != 1)
        {
            fprintf(stderr, "Erro ao ler a opcao do menu\n");
            exit(1);
        }
        getchar();

        switch (opcao)
        {
        case 1:
        {
            char searchId[NUM_DIGITS + 1];
            printf("Digite o ID do usuario que deseja buscar: ");
            if (scanf("%15s", searchId) != 1)
            {
                fprintf(stderr, "Erro ao ler ID do usuario\n");
                exit(1);
            }
            getUserById(searchId);
            break;
        }
        case 2:
        {
            char deleteId[NUM_DIGITS + 1];
            printf("Digite o ID do usuario que deseja deletar: ");
            if (scanf("%15s", deleteId) != 1)
            {
                fprintf(stderr, "Erro ao ler ID do usuario\n");
                exit(1);
            }
            deleteUser(deleteId);
            break;
        }
        case 3:
            getUserDetails(userName, &age, &balance);

            char *id = randomId();
            if (id == NULL)
            {
                fprintf(stderr, "Erro ao gerar ID para o usuario\n");
                break;
            }

            insertUser(id, userName, age, balance);
            free(id);
            break;
        case 4:
            insertMultiplesUsersMenu();
            break;
        case 5:
        {
            char remetenteId[NUM_DIGITS + 1];
            printf("Digite o ID do usuario remetente: ");
            if (scanf("%15s", remetenteId) != 1)
            {
                fprintf(stderr, "Erro ao ler ID do usuario remetente\n");
                exit(1);
            }

            char destinoId[NUM_DIGITS + 1];
            printf("Digite o ID do destinatario: ");
            if (scanf("%15s", destinoId) != 1)
            {
                fprintf(stderr, "Erro ao ler ID do destinatario\n");
                exit(1);
            }

            float transferAmount = 0.0;
            printf("Digite a quantia a ser transferida: ");
            if (scanf("%f", &transferAmount) != 1)
            {
                fprintf(stderr, "Erro ao ler a quantia a ser transferida\n");
                exit(1);
            }

            transferMoney(remetenteId, destinoId, transferAmount);
            break;
        }
        case 6:
            printf("Encerrando o programa...\n");
            break;
        default:
            printf("Opcao invalida. Encerrando o programa...\n");
            opcao = 6;
            break;
        }

    } while (opcao != 6);

    return 0;
}