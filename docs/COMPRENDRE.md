# Comprendre et personnaliser StudentManager

## Parcours de lecture

1. `Etudiant` regroupe l'identifiant, les noms et les notes. `Registre` contient un tableau et sa taille utile. Seules les cases avant `taille` sont des étudiants valides.
2. `lire_ligne` lit une ligne complète. `strtol` et `strtod` permettent de vérifier la conversion et le texte restant ; une saisie comme `12abc` est rejetée.
3. `trouver` parcourt le tableau : coût linéaire O(n). La valeur -1 signale l'absence de résultat.
4. `saisir_etudiant` remplit une copie locale. L'ajout ou la modification est validé uniquement quand la saisie entière réussit.
5. `moyenne` additionne les notes et divise par leur nombre. Sans note, elle renvoie -1, ce qui place ce cas après les moyennes réelles au tri.
6. Supprimer décale les cases suivantes vers la gauche puis diminue la taille. Il n'y a aucune allocation à libérer.
7. `qsort` reçoit une fonction de comparaison qui renvoie une valeur négative, nulle ou positive. Les égalités de moyenne sont départagées par les noms, puis les identifiants.
8. `decoder_etudiant` vérifie chaque champ. `charger` construit un registre temporaire avant de l'affecter au registre courant.
9. `sauvegarder` vérifie aussi la fermeture du fichier : certaines erreurs d'écriture n'apparaissent qu'au vidage du tampon.
10. `main` orchestre le menu et suit les modifications avec le drapeau `modifie`.

## Questions d'entretien

- Pourquoi une structure plutôt que plusieurs tableaux ? Pour garder ensemble les informations d'un étudiant.
- Pourquoi vérifier l'identifiant ? Pour éviter toute ambiguïté lors des recherches et modifications.
- Pourquoi ne pas employer `scanf` ici ? La lecture par ligne facilite le traitement des espaces, des erreurs et des entrées trop longues.
- Pourquoi un registre temporaire au chargement ? Pour qu'un fichier partiellement correct ne détruise pas les données déjà présentes.
- Quelle différence entre mémoire et fichier ? Le tableau disparaît en quittant ; la sauvegarde permet la persistance.
- Quelles limites ? Capacité fixe, recherche linéaire, tri non linguistique, absence de coefficients, pas d'accès concurrent ni de chiffrement.

## Démonstration de trois minutes

Compiler, ajouter deux étudiants, saisir une note invalide puis la corriger, afficher une moyenne, rechercher un identifiant, modifier un étudiant, trier, sauvegarder, quitter, relancer et charger. Montrer ensuite qu'un fichier invalide est refusé sans effacer le registre.

## Amélioration personnelle conseillée

Ajouter une option « statistiques » : nombre d'étudiants notés, moyenne des moyennes, meilleure moyenne. Décider explicitement si chaque étudiant a le même poids et exclure les étudiants sans note. Écrire un exemple calculé à la main avant de coder, puis comparer le résultat. Autres pistes : recherche par nom ou ajout d'une note sans ressaisir les précédentes.

Pour expliquer sa contribution : indiquer l'assistance reçue, puis montrer précisément les parties comprises, les tests reproduits et l'amélioration réalisée. Ne pas revendiquer un travail que l'on n'a pas effectué.
