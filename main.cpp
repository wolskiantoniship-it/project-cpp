#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <iomanip>
#include "klasy.h"
using namespace std;

// --- NOWE FUNKCJE: OBSŁUGA WYJĄTKÓW I INPUTU ---

// Ta funkcja zapobiega crashowi, gdy user wpisze "abc" zamiast "1"
int main() {
    SetConsoleOutputCP(65001);

    // Blok try-catch dla globalnych błędów
    try {
        vector<Pracownik*> bazaPracownikow; //wektor wskaźników klasy Pracownik
        vector<Projekt> bazaProjektow; //wektor klasy Projekt

        // Najpierw wczytujemy dane, żeby pracownik mógł się zalogować mailem
        wczytajBaze(bazaPracownikow); // wywołuje metody wczytania odpowiednich projektów
        wczytajProjekty(bazaProjektow);
        wczytajPodzadania(bazaProjektow);

        // --- SYSTEM LOGOWANIA ---
        bool czyAdmin = ekranLogowania(bazaPracownikow);

        while(true) {
            rysujDashboard(bazaPracownikow, bazaProjektow);

            // Pasek statusu - pokazuje kim jesteś
            cout << "\nZalogowany jako: " << (czyAdmin ? RED "ADMINISTRATOR" : GREEN "PRACOWNIK") << RESET << endl;
            cout << "1. PRACOWNICY | 2. PROJEKTY | 3. RAPORTY | ESC. WYJŚCIE\n";

            char m = _getch();
            if (m == 27) break; // ESC

            // --- SEKCJA 1: PRACOWNICY ---
            if (m == '1') {
                bool wSekcji = true;
                while(wSekcji) {
                    system("cls");
                    cout << BLUE << "----------------------- SEKCJA - PRACOWNICY -----------------------" << RESET << endl;
                    if (bazaPracownikow.empty()) cout << YELLOW << "[Baza pusta]" << RESET << endl;
                    else {
                        for (int i = 0; i < (int)bazaPracownikow.size(); i++) {
                            cout << i + 1 << ". "; bazaPracownikow[i]->wyswietlDane();
                        }
                    }
                    cout << "-------------------------------------------------------------------" << endl;

                    // MENU ZALEŻNE OD ROLI
                    if (czyAdmin) {
                        cout << GREEN << "'+' DODAJ" << RESET << " | " << RED << "'-' USUŃ" << RESET << " | ";
                    }
                    cout << CYAN << "NUMER - SZCZEGÓŁY" << RESET << " | ESC - POWRÓT" << endl;

                    char podWybor1 = _getch();
                    if (podWybor1 == 27) { wSekcji = false; break; }

                    if (isdigit(podWybor1)) {
                        int idx = podWybor1 - '0' - 1;
                        if (idx >= 0 && idx < (int)bazaPracownikow.size()) {
                            system("cls");
                            bazaPracownikow[idx]->wyswietlDane();

                            // Tutaj dla uproszczenia zostawiam podgląd dla każdego
                            cout << "Szacowana wypłata (160h): " << GREEN << bazaPracownikow[idx]->obliczWyplate(160) << " PLN" << RESET << endl;
                            cout << "\n[DOWOLNY KLAWISZ - POWRÓT]"; _getch();
                        }
                    }
                    // BLOKADA DLA ZWYKŁEGO USERA
                    else if (podWybor1 == '+' || podWybor1 == '-') {
                        if (!czyAdmin) {
                            cout << RED << "\nBRAK UPRAWNIEŃ ADMINISTRATORA!" << RESET; Sleep(800);
                        } else {
                            // --- LOGIKA ADMINA (DODAWANIE/USUWANIE) ---
                            if (podWybor1 == '+') {
                                system("cls");
                                string imie, mail, stan; float stawka;
                                cout << GREEN << "--- DODAWANIE (ADMIN) ---" << RESET << endl;
                                cout << "Imię: "; getline(cin >> ws, imie);
                                cout << "Email: "; cin >> mail;
                                cout << "Stawka: "; stawka = wczytajLiczbe(); // Używamy bezpiecznego wczytywania
                                cout << "Stanowisko: "; cin >> stan;
                                bazaPracownikow.push_back(new Pracownik(imie, stawka, stan, mail));
                                zapiszBaze(bazaPracownikow); cout << "Dodano!"; _getch();
                            }
                            else if (podWybor1 == '-') {
                                system("cls"); cout << "Nr do usunięcia: ";
                                int nr = wczytajLiczbe(); // Bezpieczne wczytywanie
                                if (nr > 0 && nr <= (int)bazaPracownikow.size()) {
                                    delete bazaPracownikow[nr-1];
                                    bazaPracownikow.erase(bazaPracownikow.begin() + nr - 1);
                                    zapiszBaze(bazaPracownikow); cout << "Usunięto!";
                                }
                                _getch();
                            }
                        }
                    }
                }
            }

            // --- SEKCJA 2: PROJEKTY ---
            if (m == '2') {
                bool wSekcji = true;
                while(wSekcji) {
                    system("cls");
                    cout << MAGENTA << "----------------------- SEKCJA - PROJEKTY -----------------------" << RESET << endl;
                    for(int i = 0; i < (int)bazaProjektow.size(); i++) {
                        cout << i + 1 << ". "; bazaProjektow[i].wyswietlProjekt();
                        cout << "-----------------------------------------------------------------" << endl;
                    }

                    if (czyAdmin) cout << GREEN << "'+' NOWY" << RESET << " | ";
                    cout << CYAN << "NR - ZARZĄDZAJ" << RESET << " | " << YELLOW << "'s' SORTUJ" << RESET << " | ESC" << endl;

                    char podWybor2 = _getch();
                    if (podWybor2 == 27) { wSekcji = false; break; }

                    if (podWybor2 == 's' || podWybor2 == 'S') {
                        system("cls"); cout << "1. Budżet | 2. Priorytet: "; char s = _getch();
                        if (s == '1') sortujPoBudzecie(bazaProjektow);
                        if (s == '2') sortujPoPriorytecie(bazaProjektow);
                        zapiszProjekty(bazaProjektow);
                    }
                    else if (podWybor2 == '+') {
                        if(!czyAdmin) { cout << RED << "\nTylko ADMIN może tworzyć projekty!" << RESET; Sleep(800); }
                        else {
                            system("cls"); string n; float k; int w, p;
                            cout << "Nazwa: "; getline(cin >> ws, n);
                            cout << "Budżet: "; k = wczytajLiczbe();
                            cout << "Ważny (1/0): "; w = wczytajLiczbe();
                            cout << "Pilny (1/0): "; p = wczytajLiczbe();
                            bazaProjektow.push_back(Projekt(n, k, w==1, p==1));
                            zapiszProjekty(bazaProjektow); cout << "Utworzono!"; _getch();
                        }
                    }
                    else if (isdigit(podWybor2)) {
                        int idx = podWybor2 - '0' - 1;
                        if (idx >= 0 && idx < (int)bazaProjektow.size()) {
                            bool wPodMenu = true;
                            while(wPodMenu) {
                                system("cls");
                                bazaProjektow[idx].wyswietlProjekt();
                                cout << "\n--- AKCJE ---" << endl;
                                cout << "1. DODAJ PODZADANIE" << endl;
                                cout << "2. STATUS PODZADANIA [X]" << endl;
                                if (czyAdmin) {
                                    cout << RED << "3. PRZYDZIEL INŻYNIERA (ADMIN)" << RESET << endl;
                                    cout << RED << "4. ZAKOŃCZ PROJEKT (ADMIN)" << RESET << endl;
                                    cout << RED << "5. ARCHIWIZUJ (ADMIN)" << RESET << endl;
                                }
                                cout << "ESC. POWRÓT" << endl;

                                char akcja = _getch();
                                if (akcja == 27) wPodMenu = false;
                                // --- AKCJE DOSTĘPNE DLA KAŻDEGO (Pracownik może dodawać zadania i zmieniać status) ---
                                else if (akcja == '1') {
                                    string nPod; cout << "\nNazwa: "; getline(cin >> ws, nPod);
                                    bazaProjektow[idx].dodajPodzadanie(nPod); zapiszPodzadania(bazaProjektow);
                                }
                                else if (akcja == '2') {
                                    if(bazaProjektow[idx].getListaPodzadan().empty()) { cout << "Brak zadań!"; Sleep(500); }
                                    else {
                                        cout << "\nNumer zadania: "; int nr = wczytajLiczbe();
                                        if (nr > 0 && nr <= (int)bazaProjektow[idx].getListaPodzadan().size()) {
                                            bool st = bazaProjektow[idx].getListaPodzadan()[nr-1].czy_ukonczone;
                                            bazaProjektow[idx].getListaPodzadan()[nr-1].czy_ukonczone = !st;
                                            zapiszPodzadania(bazaProjektow);
                                        }
                                    }
                                }
                                // --- AKCJE TYLKO DLA ADMINA ---
                                else if (akcja == '3' || akcja == '4' || akcja == '5') {
                                    if (!czyAdmin) { cout << RED << "BRAK UPRAWNIEŃ!" << RESET; Sleep(800); }
                                    else {
                                        if (akcja == '3') {
                                            system("cls");
                                            for(int i=0; i<(int)bazaPracownikow.size(); i++)
                                                if(bazaPracownikow[i]->czy_dostepny) cout << i+1 << ". " << bazaPracownikow[i]->imie_i_nazwisko << endl;
                                            cout << "Wybierz: "; int nr = wczytajLiczbe();
                                            if (nr>0 && nr<=(int)bazaPracownikow.size()) przydzielInzyniera(bazaPracownikow[nr-1], bazaProjektow[idx]);
                                            zapiszBaze(bazaPracownikow); zapiszProjekty(bazaProjektow);
                                        }
                                        if (akcja == '4') {
                                            bazaProjektow[idx].czy_zakonczony = true;
                                            zwolnijPracownika(bazaPracownikow, bazaProjektow[idx].wykonawca_mail);
                                            zapiszProjekty(bazaProjektow); zapiszBaze(bazaPracownikow);
                                        }
                                        if (akcja == '5') {
                                            archiwizujProjekt(bazaProjektow, bazaPracownikow, idx); wPodMenu = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // --- SEKCJA 3: RAPORTY ---
            if (m == '3') {
                system("cls");
                wyswietlStatystyki(bazaPracownikow, bazaProjektow);
                cout << "\n1. Eksport | 2. Gantt | ESC";
                char r = _getch();
                if (r == '1') { zapiszBaze(bazaPracownikow); zapiszProjekty(bazaProjektow); cout << " Gotowe!"; Sleep(500); }
                if (r == '2') generujWykresGantta(bazaProjektow);
            }
        }

        // Sprzątanie pamięci
        for(auto p : bazaPracownikow) delete p;

    } catch (const exception& e) {
        // --- GLOBALNA OBSŁUGA WYJĄTKÓW ---
        cout << RED << "\n\nKRYTYCZNY BŁĄD PROGRAMU: " << e.what() << RESET << endl;
        cout << "Naciśnij dowolny klawisz, aby zakończyć...";
        _getch();
    } catch (...) {
        cout << RED << "\n\nNIEZNANY BŁĄD!" << RESET << endl;
        _getch();
    }

    return 0;
}
