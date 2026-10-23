#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

#define MAX_ETUDIANTS 500
#define MAX_NOTES 10
#define TAILLE_NOM 64
#define TAILLE_LIGNE 1024

typedef struct {
    int id;
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    int nombre_notes;
    double notes[MAX_NOTES];
} Etudiant;

typedef struct {
    Etudiant etudiants[MAX_ETUDIANTS];
    int taille;
} Registre;

/* Une ligne trop longue est consommee entierement, jamais tronquee en silence. */
static int lire_ligne(FILE *fichier, char *ligne, size_t capacite)
{
    size_t longueur;
    int caractere;
    if (fgets(ligne, (int)capacite, fichier) == NULL) return 0;
    longueur = strlen(ligne);
    if (longueur > 0 && ligne[longueur - 1] == '\n') {
        ligne[--longueur] = '\0';
    } else if (!feof(fichier)) {
        caractere = fgetc(fichier);
        if (caractere != EOF) {
            while (caractere != '\n' && caractere != EOF) caractere = fgetc(fichier);
            return -1;
        }
    }
    if (longueur > 0 && ligne[longueur - 1] == '\r') ligne[longueur - 1] = '\0';
    return 1;
}

static int convertir_entier(const char *texte, int minimum, int maximum, int *valeur)
{
    char *fin;
    long nombre;
    errno = 0;
    nombre = strtol(texte, &fin, 10);
    if (fin == texte || errno != 0) return 0;
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0' || nombre < minimum || nombre > maximum) return 0;
    *valeur = (int)nombre;
    return 1;
}

static int convertir_note(const char *texte, double *valeur)
{
    char *fin;
    double note;
    errno = 0;
    note = strtod(texte, &fin);
    if (fin == texte || errno != 0) return 0;
    while (isspace((unsigned char)*fin)) fin++;
    if (*fin != '\0' || !isfinite(note) || note < 0 || note > 20) return 0;
    *valeur = note;
    return 1;
}

static int demander_entier(const char *message, int minimum, int maximum, int *valeur)
{
    char ligne[TAILLE_LIGNE];
    int resultat;
    for (;;) {
        printf("%s", message);
        fflush(stdout);
        resultat = lire_ligne(stdin, ligne, sizeof ligne);
        if (resultat == 0) return 0;
        if (resultat == 1 && convertir_entier(ligne, minimum, maximum, valeur)) return 1;
        printf("Entrez un entier entre %d et %d.\n", minimum, maximum);
    }
}

static int nom_valide(const char *nom)
{
    size_t i, longueur = strlen(nom);
    int contenu = 0;
    if (longueur == 0 || longueur >= TAILLE_NOM) return 0;
    for (i = 0; i < longueur; i++) {
        unsigned char c = (unsigned char)nom[i];
        if (c == ';' || c < 32 || c == 127) return 0;
        if (!isspace(c)) contenu = 1;
    }
    return contenu;
}

static int demander_nom(const char *message, char *nom)
{
    char ligne[TAILLE_LIGNE];
    int resultat;
    for (;;) {
        printf("%s", message);
        fflush(stdout);
        resultat = lire_ligne(stdin, ligne, sizeof ligne);
        if (resultat == 0) return 0;
        if (resultat == 1 && nom_valide(ligne)) {
            strcpy(nom, ligne);
            return 1;
        }
        puts("Nom requis : 1 a 63 octets, sans point-virgule ni caractere de controle.");
    }
}

static int trouver(const Registre *registre, int id)
{
    int i;
    for (i = 0; i < registre->taille; i++)
        if (registre->etudiants[i].id == id) return i;
    return -1;
}

static double moyenne(const Etudiant *etudiant)
{
    double somme = 0;
    int i;
    for (i = 0; i < etudiant->nombre_notes; i++) somme += etudiant->notes[i];
    return etudiant->nombre_notes ? somme / etudiant->nombre_notes : -1;
}

static void afficher_etudiant(const Etudiant *etudiant)
{
    int i;
    printf("ID %d | %s %s | Notes :", etudiant->id, etudiant->nom, etudiant->prenom);
    for (i = 0; i < etudiant->nombre_notes; i++) printf(" %.2f", etudiant->notes[i]);
    if (etudiant->nombre_notes) printf(" | Moyenne : %.2f/20\n", moyenne(etudiant));
    else puts(" aucune | Moyenne : non disponible");
}

static int saisir_etudiant(Etudiant *etudiant)
{
    char ligne[TAILLE_LIGNE];
    int i, resultat;
    if (!demander_nom("Nom : ", etudiant->nom) ||
        !demander_nom("Prenom : ", etudiant->prenom) ||
        !demander_entier("Nombre de notes (0-10) : ", 0, MAX_NOTES, &etudiant->nombre_notes)) return 0;
    for (i = 0; i < etudiant->nombre_notes; i++) {
        for (;;) {
            printf("Note %d (0-20, point decimal) : ", i + 1);
            fflush(stdout);
            resultat = lire_ligne(stdin, ligne, sizeof ligne);
            if (resultat == 0) return 0;
            if (resultat == 1 && convertir_note(ligne, &etudiant->notes[i])) break;
            puts("Note invalide.");
        }
    }
    return 1;
}

/* Chaque champ doit exister, y compris lorsqu'il est vide : pas de strtok. */
static char *champ_suivant(char **curseur)
{
    char *debut = *curseur;
    char *separateur;
    if (debut == NULL) return NULL;
    separateur = strchr(debut, ';');
    if (separateur) {
        *separateur = '\0';
        *curseur = separateur + 1;
    } else *curseur = NULL;
    return debut;
}

static int decoder_etudiant(char *ligne, Etudiant *etudiant)
{
    char *curseur = ligne;
    char *champ;
    int i;
    champ = champ_suivant(&curseur);
    if (!champ || !convertir_entier(champ, 1, 999999, &etudiant->id)) return 0;
    champ = champ_suivant(&curseur);
    if (!champ || !nom_valide(champ)) return 0;
    strcpy(etudiant->nom, champ);
    champ = champ_suivant(&curseur);
    if (!champ || !nom_valide(champ)) return 0;
    strcpy(etudiant->prenom, champ);
    champ = champ_suivant(&curseur);
    if (!champ || !convertir_entier(champ, 0, MAX_NOTES, &etudiant->nombre_notes)) return 0;
    for (i = 0; i < etudiant->nombre_notes; i++) {
        champ = champ_suivant(&curseur);
        if (!champ || !convertir_note(champ, &etudiant->notes[i])) return 0;
    }
    return curseur == NULL;
}

static int charger(Registre *registre, const char *chemin)
{
    FILE *fichier = fopen(chemin, "rb");
    Registre temporaire = {0};
    Etudiant etudiant;
    char ligne[TAILLE_LIGNE];
    int resultat, numero = 1, valide = 1;
    if (!fichier) { puts("Chargement impossible : fichier absent ou inaccessible."); return 0; }
    if (lire_ligne(fichier, ligne, sizeof ligne) != 1 || strcmp(ligne, "STUDENTMANAGER;1") != 0) valide = 0;
    while (valide && (resultat = lire_ligne(fichier, ligne, sizeof ligne)) != 0) {
        numero++;
        if (resultat != 1 || temporaire.taille == MAX_ETUDIANTS ||
            !decoder_etudiant(ligne, &etudiant) || trouver(&temporaire, etudiant.id) >= 0) valide = 0;
        else temporaire.etudiants[temporaire.taille++] = etudiant;
    }
    if (ferror(fichier)) valide = 0;
    if (fclose(fichier) != 0) valide = 0;
    if (!valide) { printf("Fichier invalide ou erreur de lecture (vers la ligne %d). Donnees conservees.\n", numero); return 0; }
    *registre = temporaire;
    printf("Chargement reussi : %d etudiant(s).\n", registre->taille);
    return 1;
}

/* Le fichier precedent est garde en .bak pour permettre la restauration. */
static int sauvegarder(const Registre *registre, const char *chemin)
{
    char temporaire[1024], sauvegarde[1024];
    FILE *fichier, *ancien;
    int i, j, valide = 1, existe = 0;
    if (strlen(chemin) > 1000) { puts("Chemin trop long."); return 0; }
    snprintf(temporaire, sizeof temporaire, "%s.tmp", chemin);
    snprintf(sauvegarde, sizeof sauvegarde, "%s.bak", chemin);
    fichier = fopen(temporaire, "wb");
    if (!fichier) { puts("Impossible de creer le fichier temporaire."); return 0; }
    if (fprintf(fichier, "STUDENTMANAGER;1\n") < 0) valide = 0;
    for (i = 0; i < registre->taille; i++) {
        const Etudiant *e = &registre->etudiants[i];
        if (fprintf(fichier, "%d;%s;%s;%d", e->id, e->nom, e->prenom, e->nombre_notes) < 0) valide = 0;
        for (j = 0; j < e->nombre_notes; j++)
            if (fprintf(fichier, ";%.17g", e->notes[j]) < 0) valide = 0;
        if (fputc('\n', fichier) == EOF) valide = 0;
    }
    if (fflush(fichier) != 0) valide = 0;
    if (fclose(fichier) != 0) valide = 0;
    if (!valide) { remove(temporaire); puts("Erreur d'ecriture. Ancien fichier conserve."); return 0; }
    errno = 0;
    ancien = fopen(chemin, "rb");
    if (ancien) { existe = 1; fclose(ancien); }
    else if (errno != ENOENT) { remove(temporaire); puts("Ancien fichier inaccessible."); return 0; }
    if (existe) {
        if (remove(sauvegarde) != 0 && errno != ENOENT) {
            remove(temporaire); puts("Impossible de remplacer la sauvegarde .bak."); return 0;
        }
        if (rename(chemin, sauvegarde) != 0) {
            remove(temporaire); puts("Impossible de proteger l'ancien fichier."); return 0;
        }
    }
    if (rename(temporaire, chemin) != 0) {
        if (existe && rename(sauvegarde, chemin) != 0)
            puts("Restauration impossible : recuperer les donnees dans le fichier .bak.");
        remove(temporaire); puts("Sauvegarde impossible."); return 0;
    }
    puts("Sauvegarde reussie.");
    return 1;
}

static int comparer_noms(const void *a, const void *b)
{
    const Etudiant *x = a, *y = b;
    int resultat = strcmp(x->nom, y->nom);
    if (!resultat) resultat = strcmp(x->prenom, y->prenom);
    return resultat ? resultat : (x->id > y->id) - (x->id < y->id);
}

static int comparer_moyennes(const void *a, const void *b)
{
    double x = moyenne(a), y = moyenne(b);
    return x == y ? comparer_noms(a, b) : (x < y ? 1 : -1);
}

int main(int argc, char **argv)
{
    Registre registre = {0};
    const char *chemin = argc > 1 ? argv[1] : "etudiants.txt";
    int choix, id, position, i, ordre, modifie = 0;
    Etudiant nouveau;
    if (argc > 2) { fprintf(stderr, "Usage : %s [fichier]\n", argv[0]); return EXIT_FAILURE; }
    puts("StudentManager - gestion des etudiants");
    puts("Utilisez 8 pour charger un fichier existant.");
    for (;;) {
        puts("\n1 Ajouter | 2 Afficher | 3 Rechercher (ID) | 4 Modifier");
        puts("5 Supprimer | 6 Trier | 7 Sauvegarder | 8 Charger | 0 Quitter");
        if (!demander_entier("Choix : ", 0, 8, &choix)) break;
        if (choix == 0) {
            if (modifie) {
                if (!demander_entier("Donnees non sauvegardees. Quitter quand meme ? (1 oui / 0 non) : ", 0, 1, &choix)) break;
                if (!choix) continue;
            }
            break;
        }
        if (choix == 1) {
            if (registre.taille == MAX_ETUDIANTS) { puts("Registre plein."); continue; }
            if (!demander_entier("Identifiant (1-999999) : ", 1, 999999, &id)) break;
            if (trouver(&registre, id) >= 0) { puts("Identifiant deja utilise."); continue; }
            nouveau.id = id;
            if (!saisir_etudiant(&nouveau)) break;
            registre.etudiants[registre.taille++] = nouveau;
            modifie = 1; puts("Etudiant ajoute.");
        } else if (choix == 2) {
            if (!registre.taille) puts("Aucun etudiant.");
            for (i = 0; i < registre.taille; i++) afficher_etudiant(&registre.etudiants[i]);
        } else if (choix >= 3 && choix <= 5) {
            if (!demander_entier("Identifiant : ", 1, 999999, &id)) break;
            position = trouver(&registre, id);
            if (position < 0) { puts("Etudiant introuvable."); continue; }
            if (choix == 3) afficher_etudiant(&registre.etudiants[position]);
            else if (choix == 4) {
                nouveau = registre.etudiants[position];
                if (!saisir_etudiant(&nouveau)) break;
                registre.etudiants[position] = nouveau;
                modifie = 1; puts("Etudiant modifie.");
            } else {
                if (!demander_entier("Confirmer la suppression ? (1 oui / 0 non) : ", 0, 1, &ordre)) break;
                if (!ordre) continue;
                for (i = position; i < registre.taille - 1; i++) registre.etudiants[i] = registre.etudiants[i + 1];
                registre.taille--; modifie = 1; puts("Etudiant supprime.");
            }
        } else if (choix == 6) {
            if (!demander_entier("Tri : 1 nom croissant / 2 moyenne decroissante : ", 1, 2, &ordre)) break;
            qsort(registre.etudiants, (size_t)registre.taille, sizeof(Etudiant), ordre == 1 ? comparer_noms : comparer_moyennes);
            modifie = 1; puts("Tri termine.");
        } else if (choix == 7) {
            if (sauvegarder(&registre, chemin)) modifie = 0;
        } else if (choix == 8) {
            if (modifie) {
                if (!demander_entier("Remplacer les donnees en memoire ? (1 oui / 0 non) : ", 0, 1, &ordre)) break;
                if (!ordre) continue;
            }
            if (charger(&registre, chemin)) modifie = 0;
        }
    }
    if (modifie) puts("Attention : modifications non sauvegardees.");
    puts("Au revoir.");
    return EXIT_SUCCESS;
}
