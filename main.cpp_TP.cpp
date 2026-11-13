// =====================================================================
// Projet : Gestion d'une agence de voyage (Cotonou) - C++ POO
// Fichier unique : toutes les classes + le menu principal
// Compilation : g++ -std=c++17 main.cpp -o agence
// =====================================================================
#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// ============================================================
// DECLARATIONS DES CLASSES
// ============================================================

// ---------- Classe Passager ----------
// Classe Passager : représente un client enregistré à l'agence.
class Passager {
private:                       // Encapsulation : attributs privés
    int id;
    std::string nom;
    std::string prenom;
    std::string telephone;
    std::string email;

public:
    // Constructeur : valide les données (lance une exception si invalides)
    Passager(int id, const std::string& nom, const std::string& prenom,
             const std::string& telephone, const std::string& email);

    // Accesseurs (getters) : const car ils ne modifient pas l'objet
    int getId() const;
    std::string getNom() const;
    std::string getPrenom() const;
    std::string getTelephone() const;
    std::string getEmail() const;

    // Mutateurs (setters)
    void setNom(const std::string& nom);
    void setPrenom(const std::string& prenom);
    void setTelephone(const std::string& telephone);
    void setEmail(const std::string& email);

    void afficher() const;
};

// ---------- Classe Vol ----------
// Classe Vol : un vol avec une capacité maximale de places.
class Vol {
private:
    std::string numero;
    std::string depart;
    std::string destination;
    std::string date;
    int capacite;
    double prix;
    int nbReservations;        // nombre de places déjà réservées

public:
    Vol(const std::string& numero, const std::string& depart,
        const std::string& destination, const std::string& date,
        int capacite, double prix);

    // Accesseurs
    std::string getNumero() const;
    std::string getDepart() const;
    std::string getDestination() const;
    std::string getDate() const;
    int getCapacite() const;
    double getPrix() const;
    int getNbReservations() const;

    // Règle 2 : un vol ne peut pas dépasser sa capacité
    int placesDisponibles() const;
    bool estComplet() const;
    void ajouterReservation();      // +1 (erreur si complet)
    void retirerReservation();      // -1 (lors d'une annulation)

    void afficher() const;
};

// ---------- Classe Reservation ----------
enum class StatutReservation { EN_ATTENTE, VALIDEE, ANNULEE };

// Une réservation est associée à UN seul passager et UN seul vol (règle 4).
class Reservation {
private:
    int id;
    std::shared_ptr<Passager> passager;   // association vers Passager
    std::shared_ptr<Vol> vol;             // association vers Vol
    StatutReservation statut;

public:
    Reservation(int id, std::shared_ptr<Passager> passager, std::shared_ptr<Vol> vol);

    int getId() const;
    std::shared_ptr<Passager> getPassager() const;
    std::shared_ptr<Vol> getVol() const;
    StatutReservation getStatut() const;

    void valider();     // EN_ATTENTE -> VALIDEE (règle 6)
    void annuler();     // -> ANNULEE
    void afficher() const;

    static std::string statutEnTexte(StatutReservation s);
};

// ---------- Classe Paiement ----------
enum class StatutRemboursement { AUCUN, A_REMBOURSER, REMBOURSE };

class Paiement {
private:
    int id;
    std::shared_ptr<Reservation> reservation;
    double montant;
    StatutRemboursement statutRemboursement;

public:
    Paiement(int id, std::shared_ptr<Reservation> reservation, double montant);

    int getId() const;
    std::shared_ptr<Reservation> getReservation() const;
    double getMontant() const;
    StatutRemboursement getStatutRemboursement() const;

    void effectuer();            // règle 5 (pas de paiement si annulée) + règle 6 (validation)
    void marquerARembourser();   // appelée quand on annule une réservation payée
    void rembourser();           // A_REMBOURSER -> REMBOURSE
    void afficher() const;

    static std::string remboursementEnTexte(StatutRemboursement s);
};

// ---------- Classe Agence ----------
// Agence : contient les collections et applique les règles de gestion.
class Agence {
private:
    std::vector<std::shared_ptr<Passager>> passagers;
    std::vector<std::shared_ptr<Vol>> vols;
    std::vector<std::shared_ptr<Reservation>> reservations;
    std::vector<std::shared_ptr<Paiement>> paiements;

    int prochainIdPassager = 1;
    int prochainIdReservation = 1;
    int prochainIdPaiement = 1;

    std::shared_ptr<Passager> trouverPassager(int id) const;
    std::shared_ptr<Vol> trouverVol(const std::string& numero) const;
    std::shared_ptr<Reservation> trouverReservation(int id) const;
    std::shared_ptr<Paiement> trouverPaiement(int idReservation) const;

public:
    int enregistrerPassager(const std::string& nom, const std::string& prenom,
                            const std::string& telephone, const std::string& email);
    void ajouterVol(const std::string& numero, const std::string& depart,
                    const std::string& destination, const std::string& date,
                    int capacite, double prix);

    void afficherVolsDisponibles() const;
    void rechercherVols(const std::string& critere) const;

    int reserver(int idPassager, const std::string& numeroVol);
    void payer(int idReservation);
    void annulerReservation(int idReservation);

    void consulterReservationsPassager(int idPassager) const;
    void afficherInfosReservation(int idReservation) const;
};

// ============================================================
// IMPLEMENTATION DES CLASSES
// ============================================================

// ---------- Passager ----------
Passager::Passager(int id, const std::string& nom, const std::string& prenom,
                   const std::string& telephone, const std::string& email)
    : id(id), telephone(telephone), email(email) {
    if (id <= 0)
        throw std::invalid_argument("L'identifiant doit être strictement positif.");
    // On passe par les setters pour réutiliser la validation (pas de code dupliqué)
    setNom(nom);
    setPrenom(prenom);
}

int Passager::getId() const { return id; }
std::string Passager::getNom() const { return nom; }
std::string Passager::getPrenom() const { return prenom; }
std::string Passager::getTelephone() const { return telephone; }
std::string Passager::getEmail() const { return email; }

void Passager::setNom(const std::string& n) {
    if (n.empty()) throw std::invalid_argument("Le nom ne peut pas être vide.");
    nom = n;
}
void Passager::setPrenom(const std::string& p) {
    if (p.empty()) throw std::invalid_argument("Le prénom ne peut pas être vide.");
    prenom = p;
}
void Passager::setTelephone(const std::string& t) { telephone = t; }
void Passager::setEmail(const std::string& e) { email = e; }

void Passager::afficher() const {
    std::cout << "Passager #" << id << " : " << prenom << " " << nom
              << " | Tel : " << telephone << " | Email : " << email << "\n";
}

// ---------- Vol ----------
Vol::Vol(const std::string& numero, const std::string& depart,
         const std::string& destination, const std::string& date,
         int capacite, double prix)
    : numero(numero), depart(depart), destination(destination), date(date),
      capacite(capacite), prix(prix), nbReservations(0) {
    if (numero.empty())
        throw std::invalid_argument("Le numéro de vol ne peut pas être vide.");
    if (capacite <= 0)
        throw std::invalid_argument("La capacité doit être strictement positive.");
    if (prix < 0)
        throw std::invalid_argument("Le prix ne peut pas être négatif.");
}

std::string Vol::getNumero() const { return numero; }
std::string Vol::getDepart() const { return depart; }
std::string Vol::getDestination() const { return destination; }
std::string Vol::getDate() const { return date; }
int Vol::getCapacite() const { return capacite; }
double Vol::getPrix() const { return prix; }
int Vol::getNbReservations() const { return nbReservations; }

int Vol::placesDisponibles() const { return capacite - nbReservations; }
bool Vol::estComplet() const { return nbReservations >= capacite; }

void Vol::ajouterReservation() {
    if (estComplet())
        throw std::runtime_error("Vol complet : plus de places disponibles.");
    ++nbReservations;
}

void Vol::retirerReservation() {
    if (nbReservations == 0)
        throw std::runtime_error("Aucune réservation à retirer sur ce vol.");
    --nbReservations;
}

void Vol::afficher() const {
    std::cout << "Vol " << numero << " : " << depart << " -> " << destination
              << " | Date : " << date << " | Prix : " << prix << " FCFA"
              << " | Places : " << placesDisponibles() << "/" << capacite << "\n";
}

// ---------- Reservation ----------
Reservation::Reservation(int id, std::shared_ptr<Passager> passager, std::shared_ptr<Vol> vol)
    : id(id), passager(passager), vol(vol), statut(StatutReservation::EN_ATTENTE) {
    if (!passager || !vol)
        throw std::invalid_argument("Une reservation doit avoir un passager et un vol.");
}

int Reservation::getId() const { return id; }
std::shared_ptr<Passager> Reservation::getPassager() const { return passager; }
std::shared_ptr<Vol> Reservation::getVol() const { return vol; }
StatutReservation Reservation::getStatut() const { return statut; }

void Reservation::valider() {
    if (statut == StatutReservation::ANNULEE)
        throw std::runtime_error("Une reservation annulee ne peut plus etre validee.");
    if (statut == StatutReservation::VALIDEE)
        throw std::runtime_error("Cette reservation est deja validee (deja payee).");
    statut = StatutReservation::VALIDEE;
}

void Reservation::annuler() {
    if (statut == StatutReservation::ANNULEE)
        throw std::runtime_error("Cette reservation est deja annulee.");
    statut = StatutReservation::ANNULEE;
}

std::string Reservation::statutEnTexte(StatutReservation s) {
    switch (s) {
        case StatutReservation::EN_ATTENTE: return "EN ATTENTE";
        case StatutReservation::VALIDEE:    return "VALIDEE";
        case StatutReservation::ANNULEE:    return "ANNULEE";
    }
    return "INCONNU";
}

void Reservation::afficher() const {
    std::cout << "Reservation #" << id
              << " | Passager : " << passager->getPrenom() << " " << passager->getNom()
              << " | Vol : " << vol->getNumero() << " (" << vol->getDepart()
              << " -> " << vol->getDestination() << ")"
              << " | Statut : " << statutEnTexte(statut) << "\n";
}

// ---------- Paiement ----------
Paiement::Paiement(int id, std::shared_ptr<Reservation> reservation, double montant)
    : id(id), reservation(reservation), montant(montant),
      statutRemboursement(StatutRemboursement::AUCUN) {
    if (!reservation)
        throw std::invalid_argument("Un paiement doit etre lie a une reservation.");
    if (montant < 0)
        throw std::invalid_argument("Le montant ne peut pas etre negatif.");
}

int Paiement::getId() const { return id; }
std::shared_ptr<Reservation> Paiement::getReservation() const { return reservation; }
double Paiement::getMontant() const { return montant; }
StatutRemboursement Paiement::getStatutRemboursement() const { return statutRemboursement; }

void Paiement::effectuer() {
    // Règle 5 : une réservation annulée ne peut plus être utilisée pour payer
    if (reservation->getStatut() == StatutReservation::ANNULEE)
        throw std::runtime_error("Paiement impossible : la reservation est annulee.");
    // Règle 6 : le paiement valide la réservation (place définitivement réservée)
    reservation->valider();
}

void Paiement::marquerARembourser() {
    if (statutRemboursement == StatutRemboursement::AUCUN)
        statutRemboursement = StatutRemboursement::A_REMBOURSER;
}

void Paiement::rembourser() {
    if (statutRemboursement != StatutRemboursement::A_REMBOURSER)
        throw std::runtime_error("Aucun remboursement en attente pour ce paiement.");
    statutRemboursement = StatutRemboursement::REMBOURSE;
}

std::string Paiement::remboursementEnTexte(StatutRemboursement s) {
    switch (s) {
        case StatutRemboursement::AUCUN:        return "AUCUN";
        case StatutRemboursement::A_REMBOURSER: return "A REMBOURSER";
        case StatutRemboursement::REMBOURSE:    return "REMBOURSE";
    }
    return "INCONNU";
}

void Paiement::afficher() const {
    std::cout << "Paiement #" << id << " | Montant : " << montant
              << " FCFA | Remboursement : " << remboursementEnTexte(statutRemboursement) << "\n";
}

// ---------- Agence ----------
namespace {
// Met une chaîne en minuscules (pour une recherche sans tenir compte de la casse)
std::string enMinuscules(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}
}

// ---------- Recherches internes ----------
std::shared_ptr<Passager> Agence::trouverPassager(int id) const {
    for (const auto& p : passagers)
        if (p->getId() == id) return p;
    return nullptr;
}

std::shared_ptr<Vol> Agence::trouverVol(const std::string& numero) const {
    for (const auto& v : vols)
        if (v->getNumero() == numero) return v;
    return nullptr;
}

std::shared_ptr<Reservation> Agence::trouverReservation(int id) const {
    for (const auto& r : reservations)
        if (r->getId() == id) return r;
    return nullptr;
}

std::shared_ptr<Paiement> Agence::trouverPaiement(int idReservation) const {
    for (const auto& p : paiements)
        if (p->getReservation()->getId() == idReservation) return p;
    return nullptr;
}

// ---------- Passagers et vols ----------
int Agence::enregistrerPassager(const std::string& nom, const std::string& prenom,
                                const std::string& telephone, const std::string& email) {
    auto p = std::make_shared<Passager>(prochainIdPassager, nom, prenom, telephone, email);
    passagers.push_back(p);
    return prochainIdPassager++;      // l'id n'augmente que si la création a réussi
}

void Agence::ajouterVol(const std::string& numero, const std::string& depart,
                        const std::string& destination, const std::string& date,
                        int capacite, double prix) {
    if (trouverVol(numero))
        throw std::runtime_error("Un vol avec ce numero existe deja.");
    vols.push_back(std::make_shared<Vol>(numero, depart, destination, date, capacite, prix));
}

void Agence::afficherVolsDisponibles() const {
    bool trouve = false;
    for (const auto& v : vols) {
        if (!v->estComplet()) { v->afficher(); trouve = true; }
    }
    if (!trouve) std::cout << "Aucun vol disponible.\n";
}

void Agence::rechercherVols(const std::string& critere) const {
    std::string c = enMinuscules(critere);
    bool trouve = false;
    for (const auto& v : vols) {
        if (enMinuscules(v->getNumero()).find(c) != std::string::npos ||
            enMinuscules(v->getDepart()).find(c) != std::string::npos ||
            enMinuscules(v->getDestination()).find(c) != std::string::npos) {
            v->afficher();
            trouve = true;
        }
    }
    if (!trouve) std::cout << "Aucun vol ne correspond a la recherche.\n";
}

// ---------- Réservations et paiements ----------
int Agence::reserver(int idPassager, const std::string& numeroVol) {
    // Règle 1 : le passager doit être enregistré
    auto passager = trouverPassager(idPassager);
    if (!passager)
        throw std::runtime_error("Passager non enregistre : enregistrez-le d'abord.");

    auto vol = trouverVol(numeroVol);
    if (!vol) throw std::runtime_error("Vol introuvable.");

    // Règle 3 : pas deux fois le même vol (une réservation annulée ne compte plus)
    for (const auto& r : reservations) {
        if (r->getPassager()->getId() == idPassager &&
            r->getVol()->getNumero() == numeroVol &&
            r->getStatut() != StatutReservation::ANNULEE)
            throw std::runtime_error("Ce passager a deja reserve ce vol.");
    }

    // Règle 2 : capacité (ajouterReservation lance une exception si le vol est complet)
    vol->ajouterReservation();

    auto r = std::make_shared<Reservation>(prochainIdReservation, passager, vol);
    reservations.push_back(r);
    return prochainIdReservation++;
}

void Agence::payer(int idReservation) {
    auto r = trouverReservation(idReservation);
    if (!r) throw std::runtime_error("Reservation introuvable.");

    auto p = std::make_shared<Paiement>(prochainIdPaiement, r, r->getVol()->getPrix());
    p->effectuer();                   // règles 5 et 6 : lance une exception si impossible
    paiements.push_back(p);
    ++prochainIdPaiement;
}

void Agence::annulerReservation(int idReservation) {
    auto r = trouverReservation(idReservation);
    if (!r) throw std::runtime_error("Reservation introuvable.");

    r->annuler();                     // erreur si déjà annulée
    r->getVol()->retirerReservation();  // la place est libérée

    // Règle 7 : réservation payée puis annulée -> statut de remboursement
    auto p = trouverPaiement(idReservation);
    if (p) p->marquerARembourser();
}

// ---------- Consultations ----------
void Agence::consulterReservationsPassager(int idPassager) const {
    if (!trouverPassager(idPassager))
        throw std::runtime_error("Passager introuvable.");
    bool trouve = false;
    for (const auto& r : reservations) {
        if (r->getPassager()->getId() == idPassager) { r->afficher(); trouve = true; }
    }
    if (!trouve) std::cout << "Ce passager n'a aucune reservation.\n";
}

void Agence::afficherInfosReservation(int idReservation) const {
    auto r = trouverReservation(idReservation);
    if (!r) throw std::runtime_error("Reservation introuvable.");
    r->afficher();
    auto p = trouverPaiement(idReservation);
    if (p) p->afficher();
    else std::cout << "Aucun paiement enregistre pour cette reservation.\n";
}

// ============================================================
// PROGRAMME PRINCIPAL (main)
// ============================================================

// ---------- main ----------
using namespace std;

// ---------- Fonctions de saisie sécurisées ----------
// On lit toujours une ligne complète (getline) puis on convertit :
// cela évite les blocages de cin quand l'utilisateur tape du texte à la place d'un nombre.
string lireTexte(const string& message) {
    cout << message;
    string ligne;
    getline(cin, ligne);
    return ligne;
}

int lireEntier(const string& message) {
    while (true) {
        string ligne = lireTexte(message);
        try {
            size_t pos = 0;
            int valeur = stoi(ligne, &pos);
            if (pos != ligne.size()) throw invalid_argument("reste");
            return valeur;
        } catch (const exception&) {
            cout << "Saisie invalide : entrez un nombre entier.\n";
        }
    }
}

double lireDecimal(const string& message) {
    while (true) {
        string ligne = lireTexte(message);
        try {
            size_t pos = 0;
            double valeur = stod(ligne, &pos);
            if (pos != ligne.size()) throw invalid_argument("reste");
            return valeur;
        } catch (const exception&) {
            cout << "Saisie invalide : entrez un nombre.\n";
        }
    }
}

void afficherMenu() {
    cout << "\n===== AGENCE DE VOYAGE - MENU PRINCIPAL =====\n"
         << " 1) Enregistrer un passager\n"
         << " 2) Ajouter un vol\n"
         << " 3) Afficher les vols disponibles\n"
         << " 4) Rechercher un vol\n"
         << " 5) Effectuer une reservation\n"
         << " 6) Enregistrer un paiement\n"
         << " 7) Annuler une reservation\n"
         << " 8) Consulter les reservations d'un passager\n"
         << " 9) Afficher les informations d'une reservation\n"
         << "10) Quitter\n";
}

int main() {
    Agence agence;
    int choix = 0;

    do {
        afficherMenu();
        choix = lireEntier("Votre choix : ");

        try {   // toutes les erreurs métier (exceptions) sont attrapées ici
            switch (choix) {
            case 1: {
                string nom = lireTexte("Nom : ");
                string prenom = lireTexte("Prenom : ");
                string tel = lireTexte("Telephone : ");
                string email = lireTexte("Email : ");
                int id = agence.enregistrerPassager(nom, prenom, tel, email);
                cout << "Passager enregistre avec l'identifiant " << id << ".\n";
                break;
            }
            case 2: {
                string numero = lireTexte("Numero du vol : ");
                string depart = lireTexte("Ville de depart : ");
                string destination = lireTexte("Destination : ");
                string date = lireTexte("Date (jj/mm/aaaa) : ");
                int capacite = lireEntier("Capacite (nombre de places) : ");
                double prix = lireDecimal("Prix du billet (FCFA) : ");
                agence.ajouterVol(numero, depart, destination, date, capacite, prix);
                cout << "Vol ajoute avec succes.\n";
                break;
            }
            case 3:
                agence.afficherVolsDisponibles();
                break;
            case 4: {
                string critere = lireTexte("Numero, depart ou destination : ");
                agence.rechercherVols(critere);
                break;
            }
            case 5: {
                int idPassager = lireEntier("Identifiant du passager : ");
                string numeroVol = lireTexte("Numero du vol : ");
                int idRes = agence.reserver(idPassager, numeroVol);
                cout << "Reservation creee (en attente de paiement) : n°" << idRes << ".\n";
                break;
            }
            case 6: {
                int idRes = lireEntier("Numero de la reservation : ");
                agence.payer(idRes);
                cout << "Paiement enregistre : reservation validee.\n";
                break;
            }
            case 7: {
                int idRes = lireEntier("Numero de la reservation : ");
                agence.annulerReservation(idRes);
                cout << "Reservation annulee.\n";
                break;
            }
            case 8: {
                int idPassager = lireEntier("Identifiant du passager : ");
                agence.consulterReservationsPassager(idPassager);
                break;
            }
            case 9: {
                int idRes = lireEntier("Numero de la reservation : ");
                agence.afficherInfosReservation(idRes);
                break;
            }
            case 10:
                cout << "Au revoir !\n";
                break;
            default:
                cout << "Choix invalide : entrez un nombre entre 1 et 10.\n";
            }
        } catch (const exception& e) {
            cout << "Erreur : " << e.what() << "\n";
        }
    } while (choix != 10);

    return 0;
}
