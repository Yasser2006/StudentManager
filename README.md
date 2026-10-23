# StudentManager

Application console en C pour gérer un petit registre d'étudiants. Projet pédagogique de niveau L2 : structures, tableaux, fonctions, chaînes de caractères, fichiers et validation des entrées.

## Compilation et lancement

Prérequis : GCC compatible C11. Sous Windows, par exemple GCC fourni par MSYS2 UCRT64 ; sous Linux ou macOS, un compilateur C installé et disponible dans le terminal.

Linux / macOS :

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -O2 src/main.c -o studentmanager
./studentmanager
```

Windows (PowerShell) :

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -O2 src/main.c -o studentmanager.exe
.\studentmanager.exe
```

Avec GNU Make : `make` puis lancer l'exécutable. Le Makefile utilise uniquement la compilation, sans commande de nettoyage propre à un système.

Le chemin du fichier est facultatif :

```powershell
.\studentmanager.exe examples/etudiants.txt
```

Dans cet exemple, choisir **8** pour charger les données, puis **2** pour les afficher. Le chargement est volontairement explicite. Pour expérimenter sans modifier l'exemple, copier d'abord le fichier vers `etudiants.txt`.

## Fonctions

- Ajouter un étudiant avec un identifiant unique, un nom et un prénom.
- Afficher le registre ou rechercher un étudiant par son identifiant.
- Modifier ses noms et remplacer ses notes, en conservant son identifiant.
- Supprimer après confirmation.
- Saisir de 0 à 10 notes sur 20 ; calculer leur moyenne arithmétique sans coefficients.
- Trier par nom croissant ou moyenne décroissante ; les étudiants sans note viennent à la fin.
- Sauvegarder et charger le registre ; avertir avant de quitter avec des modifications.

Capacité : 500 étudiants. Identifiants : 1 à 999999. Noms : 63 octets maximum, espaces et accents UTF-8 acceptés ; point-virgule et caractères de contrôle interdits. Le tri suit l'ordre des octets (`strcmp`), donc il distingue majuscules et minuscules et n'est pas un tri linguistique français. Les notes décimales utilisent un **point** : `14.5`.

Une saisie incorrecte est redemandée. Une fin d'entrée interrompt proprement le programme, sans sauvegarde automatique. Une modification interrompue n'est pas appliquée.

## Fichiers et fiabilité

Format texte versionné, séparé par des points-virgules (ce n'est pas un parseur CSV général) :

```text
STUDENTMANAGER;1
101;Benali;Amine;3;12;15;16.5
103;Durand;Sami;0
```

Chaque ligne contient `id;nom;prenom;nombre_notes`, puis exactement le nombre de notes annoncé. Les doublons, champs manquants ou supplémentaires, notes non finies (`nan`, `inf`), lignes trop longues et dépassements de capacité sont refusés. Les fichiers LF et CRLF sont acceptés ; enregistrer en UTF-8 sans BOM. Le chargement utilise un registre temporaire : un fichier invalide laisse le registre courant intact.

La sauvegarde écrit d'abord dans `<fichier>.tmp`, vérifie les erreurs, déplace l'ancienne version dans `<fichier>.bak`, puis installe la nouvelle. En cas d'échec du dernier déplacement, le programme tente de restaurer l'ancienne version. Le `.bak` permet aussi une récupération manuelle. Ce mécanisme n'offre pas de garantie contre une panne électrique, et il faut éviter plusieurs instances écrivant sur le même fichier. Le dossier doit être accessible en écriture. Les suffixes `.tmp` et `.bak` sont réservés à l'application.

Les données sont en clair : utiliser des étudiants fictifs dans un dépôt public.

## Tests

Depuis le dossier du projet, avec PowerShell et GCC disponibles :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/test.ps1
```

Le script compile avec les avertissements traités comme erreurs, puis teste les opérations console et les fichiers invalides dans `tests/work/`. Voir [le compte rendu](docs/TESTS.md) pour les résultats réellement obtenus et leurs limites.

## Comprendre et présenter le projet

Voir [le guide pédagogique](docs/COMPRENDRE.md). Le code est regroupé dans `src/main.c` pour faciliter une première lecture. Il utilise un tableau de taille fixe plutôt qu'une allocation dynamique : c'est un choix de simplicité, avec une limite explicite.


