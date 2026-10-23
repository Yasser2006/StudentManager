# Compte rendu de vérification

Exécution réalisée le **23 octobre 2026**, sous Windows, avec **GCC 16.2.0 (MSYS2 UCRT64)** et Windows PowerShell.

Compilation effective : C11, `-Wall -Wextra -Wpedantic -Wconversion -Werror -O2`. Aucun avertissement ni erreur. Résultat du script : **SUCCES : 26 verifications.**

## Cas vérifiés

- Ajout de deux étudiants, affichage et recherche par identifiant.
- Moyenne calculée : notes 12, 15 et 16.5 → 14.50 ; absence de moyenne sans note.
- Modification persistante après sauvegarde et rechargement.
- Suppression, recherche infructueuse et conservation de l'ancienne version en `.bak`.
- Tri par moyenne décroissante et par nom croissant.
- Refus des choix non numériques, hors limites, noms vides ou contenant un point-virgule et identifiants dupliqués.
- Refus des notes `nan`, `inf`, 20.1, -1, `14,5` et `14abc`.
- Neuf fichiers invalides : mauvais en-tête, note non finie, doublon, nom vide, note manquante, champ supplémentaire, trop de notes, note supérieure à 20, ligne trop longue. Le registre courant reste intact dans chaque cas.
- Refus d'un fichier de 501 étudiants.
- Chargement CRLF avec dernière ligne sans saut de ligne.
- Fin d'entrée au milieu d'un ajout : aucune insertion partielle.
- Erreurs de lecture et d'écriture sur un chemin inexistant.

## Reproduire

Depuis la racine du projet :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/test.ps1
```

Le script recrée ses données dans `tests/work/`, ignoré par Git. Il s'arrête dès qu'une assertion échoue et peut être relancé.

## Limites de cette vérification

Les exécutions ont été faites sous Windows uniquement. Les instructions GCC Linux/macOS et le Makefile sont fournis, mais n'ont pas été exécutés sur ces systèmes. Aucune simulation de panne électrique, de disque plein ou d'écritures concurrentes n'a été réalisée. Les tests vérifient des scénarios représentatifs, pas toutes les combinaisons possibles.
