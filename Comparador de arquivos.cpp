#include <iostream>
#include <Windows.h>

//#define TESTE // Teste para alocar memória e ler arquivo
#define K_ALOCAGEM 1'000'000'000  // Constante de alocagem, 1GB no momento, lembrando que vai ser alocado duas vezes, então 2GB(s)
//#define K_ALOCAGEM 5 // Pra testes



#pragma warning( disable : 6031 )

int main()
{
    char Nomes[MAX_PATH * 2 + 1] = {};  // + 1 para o '\0'

    short NomeTam = 0;
    int tempNomeTam = 0;
    byte começo = 0;
    byte começo2 = 0;

    char* ptrAloc1;
    char* ptrAloc2;

    LARGE_INTEGER struc1;
    LARGE_INTEGER struc2;

    std::cout << "Digite o caminho dos arquivos a serem comparados:\n> ";
    std::cin.getline(Nomes, MAX_PATH * 2 - 1);

    if (!*Nomes)
    {
        std::cout << "Nem um nome colocado.\n";
        return -1;
    }

    if (*Nomes == '"')
    {
        ++começo;

        while (Nomes[NomeTam + 1] != '"') ++NomeTam;

        Nomes[NomeTam + 1] = 0;
    }
    else
    {
        while (Nomes[NomeTam] != ' ') ++NomeTam;

        Nomes[NomeTam] = 0;
    }
    
    if (!Nomes[NomeTam + começo * 2 + 1])
    {
        std::cout << "Sem um segundo nome colocado.\n";
        return -2;
    }

    if (Nomes[NomeTam + começo * 2 + 1] == '"')
    {
        ++começo2;
    
        while (Nomes[NomeTam + começo * 2 + tempNomeTam + 2] != '"' && Nomes[NomeTam + começo * 2 + tempNomeTam + 1] != 0) ++tempNomeTam;
    
        Nomes[NomeTam + começo * 2 + tempNomeTam + 2] = 0;
    }
    else
    {
        // Se alguém adicionou um espaço no final não é minha culpa
        while (/*Nomes[NomeTam + começo * 2 + tempNomeTam + 1] != ' ' && */Nomes[NomeTam + começo * 2 + tempNomeTam + 1] != 0) ++tempNomeTam;
    
        //Nomes[NomeTam + começo * 2 + tempNomeTam + 1] = 0;
    }


    if (NomeTam == tempNomeTam)
    {
        for (short i = 0; i < NomeTam; i++)
        {
            if (Nomes[começo + i] != Nomes[NomeTam + começo * 2 + 1 + começo2 + i])
                goto sair;
        }

        std::cout << "Arquivos iguais.\n> ";
        std::cin.get();
        return 0;
    }
    sair:

    // TODO, NtCreateFile(), NtOpenFile()

    HANDLE hArquivo1 = CreateFileA(Nomes + começo, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hArquivo1 == INVALID_HANDLE_VALUE)
    {
        std::cout << "Erro ao abrir o primeiro arquivo.\n";
        return 1;
    }

    HANDLE hArquivo2 = CreateFileA(Nomes + NomeTam + começo * 2 + 1 + começo2, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hArquivo2 == INVALID_HANDLE_VALUE)
    {
        std::cout << "Erro ao abrir o segundo arquivo.\n";
        return 2;
    }

    GetFileSizeEx(hArquivo1, &struc1);
    GetFileSizeEx(hArquivo2, &struc2);

    if (struc1.QuadPart != struc2.QuadPart)
    {
        std::cout << "Arquivos diferentes.\n";
        std::cin.get();
        return 0;
    }

    if (struc1.QuadPart > K_ALOCAGEM)
    {
        ptrAloc1 = (char*)VirtualAlloc(0, K_ALOCAGEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        ptrAloc2 = (char*)VirtualAlloc(0, K_ALOCAGEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        // Testes
#ifdef TESTE
        if (ptrAloc1 == nullptr || ptrAloc2 == nullptr)
        {
            std::cout << "Erro ao alocar memoria.\n";
            return 3;
        }
#endif

#ifndef TESTE
        ReadFile(hArquivo1, ptrAloc1, K_ALOCAGEM, 0, 0);
        ReadFile(hArquivo2, ptrAloc2, K_ALOCAGEM, 0, 0);
#else
        //Testes
        if (!ReadFile(hArquivo1, ptrAloc1, K_ALOCAGEM, 0, 0))
        {
            std::cout << "Erro ao ler o arquivo 1.\n";
            return 4;
        }
        if (!ReadFile(hArquivo2, ptrAloc2, K_ALOCAGEM, 0, 0))
        {
            std::cout << "Erro ao ler o arquivo 2.\n";
            return 5;
        }
#endif
    }
    else
    {
        ptrAloc1 = (char*)VirtualAlloc(0, struc1.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        ptrAloc2 = (char*)VirtualAlloc(0, struc2.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        // Testes
#ifdef TESTE
        if (ptrAloc1 == nullptr || ptrAloc2 == nullptr)
        {
            std::cout << "Erro ao alocar memoria.\n";
            return 3;
        }
#endif

#ifndef TESTE
        ReadFile(hArquivo1, ptrAloc1, struc1.QuadPart, 0, 0);
        ReadFile(hArquivo2, ptrAloc2, struc2.QuadPart, 0, 0);
#else
        //Testes
        if (!ReadFile(hArquivo1, ptrAloc1, struc1.QuadPart, 0, 0))
        {
            std::cout << "Erro ao ler o arquivo 1.\n";
            return 4;
        }
        if (!ReadFile(hArquivo2, ptrAloc2, struc2.QuadPart, 0, 0))
        {
            std::cout << "Erro ao ler o arquivo 2.\n";
            return 5;
        }
#endif
    }

    int vezes = 1;

    for (int i = 0; struc1.QuadPart; struc1.QuadPart--, i++)
    {
        if (i == K_ALOCAGEM * vezes)
        {
            ++vezes;

            if (struc1.QuadPart < K_ALOCAGEM)
            {
#ifndef TESTE
                ReadFile(hArquivo1, ptrAloc1, struc1.QuadPart, 0, 0);
                ReadFile(hArquivo2, ptrAloc2, struc1.QuadPart, 0, 0);
#else
                //Testes
                if (!ReadFile(hArquivo1, ptrAloc1, struc1.QuadPart, 0, 0))
                {
                    std::cout << "Erro ao ler o arquivo 1.\n";
                    return 4;
                }
                if (!ReadFile(hArquivo2, ptrAloc2, struc1.QuadPart, 0, 0))
                {
                    std::cout << "Erro ao ler o arquivo 2.\n";
                    return 5;
                }
#endif
            }
            else
            {
#ifndef TESTE
                ReadFile(hArquivo1, ptrAloc1, K_ALOCAGEM, 0, 0);
                ReadFile(hArquivo2, ptrAloc2, K_ALOCAGEM, 0, 0);
#else
                //Testes
                if (!ReadFile(hArquivo1, ptrAloc1, K_ALOCAGEM, 0, 0))
                {
                    std::cout << "Erro ao ler o arquivo 1.\n";
                    return 4;
                }
                if (!ReadFile(hArquivo2, ptrAloc2, K_ALOCAGEM, 0, 0))
                {
                    std::cout << "Erro ao ler o arquivo 2.\n";
                    return 5;
                }
#endif
            }
        }

        if (ptrAloc1[i - K_ALOCAGEM * (vezes - 1)] != ptrAloc2[i - K_ALOCAGEM * (vezes - 1)])
        {
            // Para o program parar de travar
            VirtualFree(ptrAloc1, 0, MEM_RELEASE);
            VirtualFree(ptrAloc2, 0, MEM_RELEASE);
            
            CloseHandle(hArquivo1);
            CloseHandle(hArquivo2);
            
            std::cout << "Arquivos diferentes.\n";
            std::cin.get();
            return 0;
        }
    }

    // Para o program parar de travar
    VirtualFree(ptrAloc1, 0, MEM_RELEASE);
    VirtualFree(ptrAloc2, 0, MEM_RELEASE);

    CloseHandle(hArquivo1);
    CloseHandle(hArquivo2);

    std::cout << "Arquivos iguais.\n";
    std::cin.get();
    return 0;
}