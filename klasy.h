
#ifndef KLASY_H
#define KLASY_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// --- KOLORY DO KONSOLI ---
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

class Podzadanie {
public:
    string nazwa_podzadania;
    bool czy_ukonczone;

    Podzadanie(string nazwa, bool status = false);
    string eksportujCSV(string nazwaProjektu) const;
};

class User {
public:
    string imie_i_nazwisko;
    string email;
    string ranga;

    User(string imie, string mail, string r) : imie_i_nazwisko(imie), email(mail), ranga(r) {}
    virtual void wyswietlPanel() = 0;
    virtual ~User() {}
};

class Admin : public User {
private:
    string haslo; // Admin ma dodatkowo hasło
public:
    // Konstruktor
    Admin(string imie, string mail, string pass);

    // Nadpisujemy metodę wirtualną z klasy User
    void wyswietlPanel() override;

    // Getter do sprawdzania hasła przy logowaniu
    bool sprawdzHaslo(string podaneHaslo) const {
        return haslo == podaneHaslo;
    }
};

class Pracownik : public User {
protected:
    float pensja_godzinowa;
    float premia;
    string stanowisko;
public:
    bool czy_dostepny;

    Pracownik(string imie, float pensja, string stan, string mail, bool dostepny = true);
    void wyswietlDane();
    float obliczWyplate(int godziny);
    string eksportujDoCSV();
    string get_stanowisko() const { return stanowisko; }
    void wyswietlPanel() override;
};

class Projekt {
private:
    string nazwa;
    float szacowany_koszt;
    vector<Podzadanie> listaPodzadan; // To przechowuje zadania
public:
    bool czy_przydzielony;
    bool wazny;
    bool pilny;
    bool czy_zakonczony;
    string wykonawca_mail;

    Projekt(string n, float koszt, bool w = false, bool p = false, bool zakonczony = false);

    void dodajPodzadanie(string nazwa_zad);
    void wyswietlProjekt();
    int obliczProcentUkonczenia() const;

    string get_nazwa() const { return nazwa; }
    float get_koszt() const { return szacowany_koszt; }
    string eksportujProjektCSV() const;

    // --- KLUCZOWA POPRAWKA PONIŻEJ (dodano &) ---
    vector<Podzadanie>& getListaPodzadan() { return listaPodzadan; }
    const vector<Podzadanie>& getListaPodzadan() const { return listaPodzadan; }
};

// --- DEKLARACJE FUNKCJI GLOBALNYCH ---
void zapiszBaze(const vector<Pracownik*>& baza);
void wczytajBaze(vector<Pracownik*>& baza);
void zapiszProjekty(const vector<Projekt>& projekty);
void wczytajProjekty(vector<Projekt>& projekty);
void zapiszPodzadania(const vector<Projekt>& projekty);
void wczytajPodzadania(vector<Projekt>& projekty);
void sortujPoBudzecie(vector<Projekt>& projekty);
void sortujPoPriorytecie(vector<Projekt>& projekty);
void archiwizujProjekt(vector<Projekt>& projekty, vector<Pracownik*>& bazaPracownikow, int index);
void wyswietlStatystyki(const vector<Pracownik*>& pracownicy, const vector<Projekt>& projekty);
void generujWykresGantta(const vector<Projekt>& projekty);
void przydzielInzyniera(Pracownik* p, Projekt& proj);
void zwolnijPracownika(vector<Pracownik*>& baza, string email);
void gotoxy(int x, int y);
void rysujDashboard(const vector<Pracownik*>& pracownicy, const vector<Projekt>& projekty);
string pobierzDate();
int wczytajLiczbe();
bool ekranLogowania(const vector<Pracownik*>& baza);

#endif
