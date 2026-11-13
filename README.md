# TP_Gestion_de_vol
## Analyse pour resoudre ce problème
Pour commencer on a:
### Passager: id, nom, prenom, telephone, email.
#### Methodes: constructeur, getters/setters, afficher()
### Vol: numero, départ, destination, date, capacite, prix, nbReservations
#### Methodes: estComplet(), placesDisponibles(), afficher()
### Reservation: id, passager, vol, statut(EN ATTENTE, VALIDEE,ANNULEE), date
#### Methodes:valider(), annuler(), afficher()
### Paiement: id, reservations, montant, date, statutRemboursement(AUCUN, A_REMBOURSER,REMBOUSE)
#### Methodes: effectuer(), rembourser()
### Agence:(classe de gestion):des "vector" de passagers, vols, réservation et paiements
#### Methodes:enregisterPassager(), ajouterVol(), reserver(), payer(), annulerReservation(), annulerReservation(),rechercherVol()...

## Les associations et multiplicités
### Passager 1---0..* Reservation(un passager peut avoir plusieurs réservations)
### Vol 1---0..* Reservation
### Réservation 1---0..1 Paiement (payée ou pas encore)
### Agence 'losange noir'---0..* de chaque classe (elle les contient tous)
