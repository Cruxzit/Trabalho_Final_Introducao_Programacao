// Sistema de Gerenciamento de Biblioteca em C++
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
using namespace std;

struct Livro {
    string titulo, autor, isbn, genero;
    bool emprestado;
    string idUsuario;
    string dataEmprestimo;
};

struct Usuario {
    string nome, id;
    vector<string> historico;
};

vector<Livro> livros;
vector<Usuario> usuarios;

// Funções auxiliares
string dataAtual() {
    time_t agora = time(0);
    tm *ltm = localtime(&agora);
    char data[11];
    sprintf(data, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(data);
}

// Persistência de dados
void salvarLivros() {
    ofstream file("livros.csv");
    for (auto &l : livros) {
        file << l.titulo << "," << l.autor << "," << l.isbn << "," << l.genero << "," << l.emprestado
             << "," << l.idUsuario << "," << l.dataEmprestimo << "\n";
    }
}

void carregarLivros() {
    ifstream file("livros.csv");
    string linha;
    livros.clear();
    while (getline(file, linha)) {
        stringstream ss(linha);
        Livro l;
        string emprestado;
        getline(ss, l.titulo, ',');
        getline(ss, l.autor, ',');
        getline(ss, l.isbn, ',');
        getline(ss, l.genero, ',');
        getline(ss, emprestado, ',');
        getline(ss, l.idUsuario, ',');
        getline(ss, l.dataEmprestimo, ',');
        l.emprestado = (emprestado == "1");
        livros.push_back(l);
    }
}

void salvarUsuarios() {
    ofstream file("usuarios.csv");
    for (auto &u : usuarios) {
        file << u.nome << "," << u.id;
        for (auto &h : u.historico) file << "," << h;
        file << "\n";
    }
}

void carregarUsuarios() {
    ifstream file("usuarios.csv");
    string linha;
    usuarios.clear();
    while (getline(file, linha)) {
        stringstream ss(linha);
        Usuario u;
        getline(ss, u.nome, ',');
        getline(ss, u.id, ',');
        string emprestimo;
        while (getline(ss, emprestimo, ',')) u.historico.push_back(emprestimo);
        usuarios.push_back(u);
    }
}

// Funcionalidades principais
void adicionarLivro() {
    Livro l;
    cout << "Título: "; getline(cin, l.titulo);
    cout << "Autor: "; getline(cin, l.autor);
    cout << "ISBN: "; getline(cin, l.isbn);
    cout << "Gênero: "; getline(cin, l.genero);
    l.emprestado = false;
    l.idUsuario = "";
    l.dataEmprestimo = "";
    livros.push_back(l);
    salvarLivros();
    cout << "Livro adicionado com sucesso!\n";
}

void pesquisarLivro() {
    string termo;
    int criterio;
    cout << "Pesquisar por: 1. Título 2. Autor 3. Gênero\nEscolha: ";
    while (!(cin >> criterio) || criterio < 1 || criterio > 3) {
        cout << "Entrada inválida. Escolha 1, 2 ou 3: ";
        cin.clear();
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    cout << "Digite o termo: "; getline(cin, termo);
    bool encontrado = false;
    for (auto &l : livros) {
        if ((criterio == 1 && l.titulo.find(termo) != string::npos) ||
            (criterio == 2 && l.autor.find(termo) != string::npos) ||
            (criterio == 3 && l.genero.find(termo) != string::npos)) {
            cout << l.titulo << " - " << l.autor << " (" << l.genero << ")\n";
            encontrado = true;
        }
    }
    if (!encontrado) cout << "Nenhum livro encontrado com o termo fornecido.\n";
}

void registrarUsuario() {
    Usuario u;
    cout << "Nome: "; getline(cin, u.nome);
    cout << "ID: "; getline(cin, u.id);
    usuarios.push_back(u);
    salvarUsuarios();
    cout << "Usuário registrado com sucesso!\n";
}

void emprestarLivro() {
    string isbn, id;
    cout << "ISBN do livro: "; getline(cin, isbn);
    cout << "ID do usuário: "; getline(cin, id);
    for (auto &l : livros) {
        if (l.isbn == isbn && !l.emprestado) {
            l.emprestado = true;
            l.idUsuario = id;
            l.dataEmprestimo = dataAtual();
            for (auto &u : usuarios) {
                if (u.id == id) {
                    u.historico.push_back("Emprestado: " + l.titulo + " em " + l.dataEmprestimo);
                    break;
                }
            }
            salvarLivros(); salvarUsuarios();
            cout << "Livro emprestado com sucesso!\n";
            return;
        }
    }
    cout << "Livro não disponível ou ISBN incorreto.\n";
}

void devolverLivro() {
    string isbn;
    cout << "ISBN do livro a devolver: "; getline(cin, isbn);
    for (auto &l : livros) {
        if (l.isbn == isbn && l.emprestado) {
            for (auto &u : usuarios) {
                if (u.id == l.idUsuario) {
                    u.historico.push_back("Devolvido: " + l.titulo + " em " + dataAtual());
                    break;
                }
            }
            l.emprestado = false;
            l.idUsuario = "";
            l.dataEmprestimo = "";
            salvarLivros(); salvarUsuarios();
            cout << "Livro devolvido com sucesso!\n";
            return;
        }
    }
    cout << "Livro não encontrado ou não emprestado.\n";
}

void relatorios() {
    cout << "\nRelatórios:\n";
    cout << "1. Livros Emprestados\n2. Livros Disponíveis\n3. Histórico por Usuário\nEscolha: ";
    int opcao;
    while (!(cin >> opcao) || opcao < 1 || opcao > 3) {
        cout << "Entrada inválida. Escolha entre 1 e 3: ";
        cin.clear();
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();
    if (opcao == 1) {
        cout << "\nLivros Emprestados:\n";
        for (auto &l : livros) if (l.emprestado) cout << l.titulo << " - " << l.autor << " (Emprestado por: " << l.idUsuario << ")\n";
    } else if (opcao == 2) {
        cout << "\nLivros Disponíveis:\n";
        for (auto &l : livros) if (!l.emprestado) cout << l.titulo << " - " << l.autor << "\n";
    } else if (opcao == 3) {
        string id;
        cout << "ID do usuário: "; getline(cin, id);
        for (auto &u : usuarios) {
            if (u.id == id) {
                cout << "Histórico de " << u.nome << ":\n";
                for (auto &h : u.historico) cout << "- " << h << "\n";
                return;
            }
        }
        cout << "Usuário não encontrado.\n";
    }
}

void menu() {
    int opcao;
    do {
        cout << "\n=== Sistema de Biblioteca ===\n";
        cout << "1. Adicionar Livro\n2. Pesquisar Livro\n3. Registrar Usuário\n4. Emprestar Livro\n5. Devolver Livro\n6. Relatórios\n0. Sair\n";
        cout << "Escolha: ";
        while (!(cin >> opcao) || opcao < 0 || opcao > 6) {
            cout << "Opção inválida. Escolha entre 0 e 6: ";
            cin.clear();
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        switch (opcao) {
            case 1: adicionarLivro(); break;
            case 2: pesquisarLivro(); break;
            case 3: registrarUsuario(); break;
            case 4: emprestarLivro(); break;
            case 5: devolverLivro(); break;
            case 6: relatorios(); break;
            case 0: cout << "Encerrando o sistema...\n"; break;
        }
    } while (opcao != 0);
}

int main() {
    carregarLivros();
    carregarUsuarios();
    menu();
    return 0;
}

