#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

#define NUM_DIGITS 15
#define MAX_NAME_LENGTH 100
#define FILE_NAME "../uaibank.txt"

// TODO: Fazer uma função para armazenar as mensagens de saída e exibi-las
// TODO: Desenvolver a função de transferência
// TODO: Analisar mais casos de erro
// TODO: Fazer o menu da aplicação

int isNameValid(const char *name)
{
    return strlen(name) <= MAX_NAME_LENGTH;
}

char *randomId()
{
    char *id = (char *)malloc((NUM_DIGITS + 1) * sizeof(char));
    if (id == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria");
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

    if (!isNameValid(name))
    {
        fprintf(stderr, "Erro: Nome excede %d caracteres.\n", MAX_NAME_LENGTH);
        return;
    }

    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Erro ao salvar dados");
        return;
    }
    fprintf(file, "\"%s\" \"%s\" \"%d\" \"%.2f\"\n", id, name, age, balance);
    fclose(file);
}

void getUserById(const char *id)
{
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char line[1024];
    char currentId[16];
    char name[101];
    int age;
    double balance;

    while (fgets(line, sizeof(line), file))

    {
        if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", currentId, name, &age, &balance) == 4)
        {

            if (strcmp(currentId, id) == 0)
            {
                printf("ID: %s\n", currentId);
                printf("Nome: %s\n", name);
                printf("Idade: %d\n", age);
                printf("Saldo: %.2f\n", balance);
                fclose(file);
                return;
            }
        }
    }

    printf("ID não encontrado.\n");
    fclose(file);
}

// void deleteUser(const char *id)
// {
//     FILE *file = fopen(FILE_NAME, "r+");
//     if (file == NULL)
//     {
//         printf("Erro ao abrir o arquivo.\n");
//         return;
//     }

//     char line[1024];
//     char currentId[16];
//     char name[101];
//     int age;
//     double balance;

//     while (fgets(line, sizeof(line), file))

//     {
//         if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", currentId, name, &age, &balance) == 4)
//         {

//             if (strcmp(currentId, id) == 0)
//             {
//                 fprintf(file, "asd");
//                 fclose(file);
//                 printf("Usuario %s removido com sucesso", currentId);
//                 return;
//             }
//         }
//     }

//     printf("ID não encontrado.\n");
//     fclose(file);
// }

void deleteUser(const char *id)
{
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        //Erro ao criar o arquivo temporario, devido a permissões por exemplo
        printf("Erro ao excluir ao usuário.\n");
        fclose(file);
        return;
    }

    char line[1024];
    char currentId[16];
    char name[101];
    int age;
    double balance;

    int found = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "\"%15[^\"]\" \"%99[^\"]\" \"%d\" \"%lf\"", currentId, name, &age, &balance) == 4)
        {
            if (strcmp(currentId, id) == 0)
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
        printf("Usuario %s removido com sucesso.\n", id);
    }
    else
    {
        remove("temp.txt");
        printf("ID %s nao encontrado.\n", id);
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
    //! Search for user
    // char searchId[NUM_DIGITS + 1];
    // printf("Digite o ID do usuario que deseja buscar: ");
    // scanf("%15s", searchId);
    // getUserById(searchId);

    //! Insert User
    // char name[MAX_NAME_LENGTH + 1];
    // int age;
    // double balance;

    // getUserDetails(name, &age, &balance);

    // char *id = randomId();
    // if (id == NULL)
    // {
    //     fprintf(stderr, "Erro ao gerar ID para o usuario\n");
    //     return 1;
    // }

    // insertUser(id, name, age, balance);
    // free(id);

    //! Insert Multiple users
    // insertMultiplesUsersMenu();

    //! Delete user
    char searchId[NUM_DIGITS + 1];
    printf("Digite o ID do usuario que deseja buscar: ");
    scanf("%15s", searchId);
    deleteUser(searchId);

    return 0;
}