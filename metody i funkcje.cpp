#include "klasy.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <algorithm>

// --- KONSTRUKTORY ---
Podzadanie::Podzadanie(string nazwa, bool status) : nazwa_podzadania(nazwa), czy_ukonczone(status) {}

Pracownik::Pracownik(string imie, float pensja, string stan, string mail, bool dostepny)
    : User(imie, mail, "Pracownik"), pensja_godzinowa(pensja), stanowisko(stan), czy_dostepny(dostepny), premia(0) {}

Projekt::Projekt(string n, float koszt, bool w, bool p, bool zakonczony)
    : nazwa(n), szacowany_koszt(koszt), wazny(w), pilny(p), czy_przydzielony(false),
      wykonawca_mail(""), czy_zakonczony(zakonczony) {}

// --- METODY KLASY PRACOWNIK ---
float Pracownik::obliczWyplate(int godziny) {
    float bonus = (stanowisko == "Slusarz") ? 500.0f : (stanowisko == "Technik" ? 300.0f : 0.0f);
    return (godziny * pensja_godzinowa) + bonus + premia;
}

void Pracownik::wyswietlDane() {
    cout << (czy_dostepny ? GREEN : RED) << "[" << (czy_dostepny ? "DOSTĘPNY" : "ZAJĘTY") << "] " << RESET;
    cout << imie_i_nazwisko << " | " << stanowisko << " | Email: " << email << endl;
}

void Pracownik::wyswietlPanel() {
    system("cls");
    cout << CYAN << "=== PANEL PRACOWNIKA: " << imie_i_nazwisko << " ===" << RESET << endl;
    cout << "Status: " << (czy_dostepny ? "Wolny" : "W trakcie zadania") << endl;
    _getch();
}

string Pracownik::eksportujDoCSV() {
    return imie_i_nazwisko + ";" + to_string(pensja_godzinowa) + ";" + stanowisko + ";" + email + ";" + (czy_dostepny ? "1" : "0");
}

// --- METODY KLASY PROJEKT (STYL V6/V7 MIX) ---
void Projekt::dodajPodzadanie(string nazwa_zad) { listaPodzadan.push_back(Podzadanie(nazwa_zad)); }

int Projekt::obliczProcentUkonczenia() const {
    // Jeśli projekt oznaczony jako zakończony ręcznie -> 100%
    if (czy_zakonczony) return 100;

    // Jeśli brak zadań -> 0%
    if (listaPodzadan.empty()) return 0;

    float ukonczone = 0;
    for (const auto& p : listaPodzadan) {
        if (p.czy_ukonczone) ukonczone++;
    }

    // Obliczamy procent (rzutowanie na float, żeby nie wyszło 0)
    return (int)((ukonczone / (float)listaPodzadan.size()) * 100);
}


void Projekt::wyswietlProjekt() {
    // 1. Nagłówek projektu
    cout << MAGENTA << "PROJEKT: " << nazwa << RESET << " | Budżet: " << szacowany_koszt << " PLN" << endl;

    // 2. Status priorytetu
    cout << "STATUS: ";
    if (wazny && pilny) cout << RED << "PRIORYTET: ZRÓB TERAZ!" << RESET;
    else if (wazny && !pilny) cout << BLUE << "PRIORYTET: ZAPLANUJ." << RESET;
    else if (!wazny && pilny) cout << YELLOW << "PRIORYTET: DELEGUJ!" << RESET;
    else cout << WHITE << "PRIORYTET: ELIMINUJ." << RESET;
    cout << endl;


    if (!listaPodzadan.empty()) {
        cout << "   " << CYAN << "Podzadania:" << RESET << endl;
        for(const auto &p : listaPodzadan) {
            cout << "   " << (p.czy_ukonczone ? GREEN : RED)
                 << (p.czy_ukonczone ? "[X] " : "[ ] ")
                 << RESET << p.nazwa_podzadania << endl;
        }
    }
}

string Projekt::eksportujProjektCSV() const {
    return nazwa + ";" + to_string(szacowany_koszt) + ";" + (czy_przydzielony ? "1" : "0") + ";" +
           (wazny ? "1" : "0") + ";" + (pilny ? "1" : "0") + ";" + (czy_zakonczony ? "1" : "0") + ";" + wykonawca_mail;
}

string Podzadanie::eksportujCSV(string nProj) const { return nProj + ";" + nazwa_podzadania + ";" + (czy_ukonczone ? "1" : "0"); }

// --- FUNKCJE ZAPISU / ODCZYTU ---
void zapiszProjekty(const vector<Projekt>& projekty) {
    ofstream plik("projekty.csv");
    if (plik.is_open()) {
        plik << "\xEF\xBB\xBF" << "Nazwa;Budzet;Przydzielony;Wazny;Pilny;Zakonczony;Wykonawca" << endl;
        for (const auto& p : projekty) plik << p.eksportujProjektCSV() << endl;
    }
}

void wczytajProjekty(vector<Projekt>& projekty) {
    ifstream plik("projekty.csv"); string linia; if (!plik.is_open()) return;
    getline(plik, linia);
    while (getline(plik, linia)) {
        stringstream ss(linia); string n, k, p, w, pil, zak, mail;
        getline(ss, n, ';'); getline(ss, k, ';'); getline(ss, p, ';');
        getline(ss, w, ';'); getline(ss, pil, ';'); getline(ss, zak, ';'); getline(ss, mail, ';');
        if (!n.empty()) {
            Projekt proj(n, (float)stof(k), w == "1", pil == "1", zak == "1");
            proj.czy_przydzielony = (p == "1"); proj.wykonawca_mail = mail;
            projekty.push_back(proj);
        }
    }
}

void zapiszPodzadania(const vector<Projekt>& projekty) {
    ofstream plik("podzadania.csv");
    if (plik.is_open()) {
        plik << "Projekt;Zadanie;Status" << endl;
        for (const auto& pr : projekty)
            for (const auto& z : pr.getListaPodzadan()) plik << z.eksportujCSV(pr.get_nazwa()) << endl;
    }
}

void wczytajPodzadania(vector<Projekt>& projekty) {
    ifstream plik("podzadania.csv"); string linia; if (!plik.is_open()) return;
    getline(plik, linia);
    while (getline(plik, linia)) {
        stringstream ss(linia); string nPr, nZa, st;
        getline(ss, nPr, ';'); getline(ss, nZa, ';'); getline(ss, st, ';');
        for (auto& p : projekty) if (p.get_nazwa() == nPr) p.getListaPodzadan().push_back(Podzadanie(nZa, st == "1"));
    }
}

void zapiszBaze(const vector<Pracownik*>& baza) {
    ofstream plik("pracownicy.csv");
    if (plik.is_open()) {
        plik << "\xEF\xBB\xBF" << "Imię;Stawka;Stanowisko;Email;Dostepny" << endl;
        for (auto p : baza) plik << p->eksportujDoCSV() << endl;
    }
}

void wczytajBaze(vector<Pracownik*>& baza) {
    ifstream plik("pracownicy.csv"); string l; if (!plik.is_open()) return;
    getline(plik, l);
    while (getline(plik, l)) {
        stringstream ss(l); string i, s, st, m, d;
        getline(ss, i, ';'); getline(ss, s, ';'); getline(ss, st, ';'); getline(ss, m, ';'); getline(ss, d, ';');
        if (!i.empty()) baza.push_back(new Pracownik(i, stof(s), st, m, d == "1"));
    }
}

// --- POZOSTAŁE FUNKCJE ---



void archiwizujProjekt(vector<Projekt>& proj, vector<Pracownik*>& prac, int idx) {
    ofstream plik("archiwum.csv", ios::app);
    if (plik.is_open()) {
        zwolnijPracownika(prac, proj[idx].wykonawca_mail);
        plik << proj[idx].eksportujProjektCSV() << endl;
        proj.erase(proj.begin() + idx);
        zapiszProjekty(proj); zapiszBaze(prac);
    }
}

void przydzielInzyniera(Pracownik* p, Projekt& proj) {
    p->czy_dostepny = false; proj.czy_przydzielony = true; proj.wykonawca_mail = p->email;
    // Wysylanie maila przy użyciu Pythona
    string cmd = "py wyslij_email.py " + p->email + " \"" + proj.get_nazwa() + "\" > nul 2>&1";
    system(cmd.c_str());
}

void zwolnijPracownika(vector<Pracownik*>& baza, string email) {
    for (auto& p : baza) if (p->email == email) p->czy_dostepny = true;
}

void sortujPoBudzecie(vector<Projekt>& p) {
    sort(p.begin(), p.end(), [](const Projekt& a, const Projekt& b){ return a.get_koszt() > b.get_koszt(); });
}

void sortujPoPriorytecie(vector<Projekt>& p) {
    sort(p.begin(), p.end(), [](const Projekt& a, const Projekt& b){ return (a.wazny+a.pilny) > (b.wazny+b.pilny); });
}



void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

string pobierzDate() {
    auto teraz = time(nullptr);
    tm czasStr; localtime_s(&czasStr, &teraz);
    stringstream ss; ss << put_time(&czasStr, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Dashboard V6 (Dwie kolumny)
void rysujDashboard(const vector<Pracownik*>& pracownicy, const vector<Projekt>& projekty) {
    system("cls");

    // --- NAGŁÓWEK ---
    gotoxy(0, 0);
    cout << YELLOW << "System Zarządzania Projektami | " << pobierzDate() << RESET;
    gotoxy(0, 1);
    cout << "--------------------------------------------------------------------------------";

    // --- LEWA KOLUMNA: PRACOWNICY ---
    gotoxy(0, 3);
    cout << BLUE << "1. PRACOWNICY" << RESET;
    int yPrac = 5;
    if (pracownicy.empty()) {
        gotoxy(0, yPrac); cout << "[Brak danych]";
    } else {
        for (int i = 0; i < (int)pracownicy.size() && i < 10; i++) {
            gotoxy(0, yPrac + i);
            if (pracownicy[i]->czy_dostepny) cout << GREEN << "[D] " << RESET;
            else cout << RED << "[Z] " << RESET;

            string nazwa = pracownicy[i]->imie_i_nazwisko;
            if(nazwa.length() > 25) nazwa = nazwa.substr(0, 22) + "...";
            cout << i + 1 << ". " << nazwa;
        }
    }

    // --- PIONOWA LINIA ---
    for (int i = 3; i < 18; i++) {
        gotoxy(42, i); cout << "|";
    }

    // --- PRAWA KOLUMNA: PROJEKTY ---
    gotoxy(45, 3);
    cout << MAGENTA << "2. PROJEKTY " << RESET;
    int yProj = 5;
    if (projekty.empty()) {
        gotoxy(45, yProj); cout << "[Brak projektów]";
    } else {
        for (int i = 0; i < (int)projekty.size() && i < 10; i++) {
            gotoxy(45, yProj + i);
            if (projekty[i].wazny && projekty[i].pilny) cout << RED << "[!] " << RESET;
            else if (projekty[i].wazny) cout << BLUE << "[P] " << RESET;
            else cout << WHITE << "[ ] " << RESET;

            string nazwa = projekty[i].get_nazwa();
            if(nazwa.length() > 25) nazwa = nazwa.substr(0, 22) + "...";
            cout << i + 1 << ". " << nazwa;
        }
    }

    // --- POZIOMA LINIA DOLNA ---
    gotoxy(0, 18);
    cout << "--------------------------------------------------------------------------------";

    // --- STATYSTYKI DOLNE ---
    gotoxy(0, 19);
    cout << YELLOW << "3.WSKAŹNIKI WYDAJNOŚCI" << RESET;

    float budzet = 0;
    for(const auto& p : projekty) budzet += p.get_koszt();

    gotoxy(0, 20);
    cout << "Budżet: " << GREEN << budzet << " PLN" << RESET
         << " | Pracownicy: " << pracownicy.size()
         << " | Projekty: " << projekty.size();

    gotoxy(0, 22);
    cout << CYAN << "WYBIERZ (1, 2, 3) LUB ESC-WYJŚCIE: " << RESET;
}


void wyswietlStatystyki(const vector<Pracownik*>& pracownicy, const vector<Projekt>& projekty) {
    float sumaPensji = 0;
    float sumaBudzetow = 0;
    for (auto p : pracownicy) sumaPensji += p->obliczWyplate(160);
    for (const auto& pr : projekty) sumaBudzetow += pr.get_koszt();

    cout << YELLOW << "==================== STATYSTYKI FIRMY ====================" << RESET << endl;
    cout << "Liczba pracowników: " << pracownicy.size() << endl;
    cout << "Liczba projektów:   " << projekty.size() << endl;
    cout << "Łączny budżet:      " << GREEN << sumaBudzetow << " PLN" << RESET << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "Miesięczny koszt wynagrodzeń (est.): " << RED << sumaPensji << " PLN" << RESET << endl;
    cout << "==========================================================" << endl;
}

void generujWykresGantta(const vector<Projekt>& projekty) {
    system("cls");
    cout << YELLOW << "==================== POSTĘP PROJEKTÓW (GANTT) ====================" << RESET << endl;

    if (projekty.empty()) {
        cout << "Brak projektów." << endl;
    } else {
        cout << left << setw(20) << "NAZWA PROJEKTU" << "   POSTĘP" << endl;
        cout << "------------------------------------------------------------------" << endl;

        for (const auto& p : projekty) {
            int proc = p.obliczProcentUkonczenia();

            // Przycinanie nazwy do 18 znaków, żeby tabelka była równa
            string nazwa = p.get_nazwa();
            if (nazwa.length() > 18) nazwa = nazwa.substr(0, 15) + "...";

            cout << left << setw(20) << nazwa << " [";

            // Rysowanie paska (20 kratek = 100%, jedna kratka to 5%)
            for (int i = 0; i < 20; i++) {
                if (i < proc / 5) cout << GREEN << "█" << RESET;
                else cout << ".";
            }
            cout << "] " << proc << "%" << endl;
        }
    }
    cout << "\n[DOWOLNY KLAWISZ - POWRÓT]";
    _getch();
}
// Prosta walidacja logowania
bool ekranLogowania(const vector<Pracownik*>& baza) {
    while(true) {
        system("cls");
        cout << YELLOW << "=== SYSTEM ZARZĄDZANIA V7 ===" << RESET << endl;
        cout << "1. Zaloguj jako ADMINISTRATOR" << endl;
        cout << "2. Zaloguj jako PRACOWNIK" << endl;
        cout << "ESC. Wyjdź" << endl;

        char wybor = _getch();
        if (wybor == 27) exit(0); // Wyjście z programu

        if (wybor == '1') {
            string pass;
            cout << "Podaj hasło administratora: ";
            cin >> pass;
            if (pass == "admin") return true; // Zwraca TRUE = Admin
            else {
                cout << RED << "Błędne hasło!" << RESET; Sleep(1000);
            }
        }
        else if (wybor == '2') {
            string email;
            cout << "Podaj swój email firmowy: ";
            cin >> email;

            bool znaleziono = false;
            for(auto p : baza) {
                if (p->email == email) {
                    cout << GREEN << "Witaj " << p->imie_i_nazwisko << "!" << RESET;
                    znaleziono = true;
                    Sleep(1000);
                    return false; // Zwraca FALSE = Zwykły User
                }
            }
            if (!znaleziono) {
                cout << RED << "Nie znaleziono pracownika w bazie!" << RESET; Sleep(1000);
            }
        }
    }
}
int wczytajLiczbe() {
    int liczba;
    while (!(cin >> liczba)) {
        // Obsługa wyjątku wejścia
        cout << RED << "Błąd: Wprowadzono niepoprawny znak! Podaj liczbę: " << RESET;
        cin.clear(); // Czyści flagę błędu
        cin.ignore(1000, '\n'); // Usuwa błędne znaki z bufora
    }
    return liczba;
}
Admin::Admin(string imie, string mail, string pass)
    : User(imie, mail, "Administrator"), haslo(pass) {}

// Implementacja panelu (wymagana, bo User ma tę metodę jako czysto wirtualną)
void Admin::wyswietlPanel() {
    system("cls");
    cout << RED << "========================================" << endl;
    cout << "   PANEL ADMINISTRATORA: " << imie_i_nazwisko << endl;
    cout << "========================================" << RESET << endl;
    cout << "Email: " << email << endl;
    cout << "Uprawnienia: " << RED << "PEŁNE (RWX)" << RESET << endl;
    cout << "\n[Naciśnij klawisz, aby kontynuować...]";
    _getch();
}
