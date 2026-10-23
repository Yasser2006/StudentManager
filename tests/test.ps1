$ErrorActionPreference = 'Stop'
$project = Split-Path $PSScriptRoot -Parent
$work = Join-Path $PSScriptRoot 'work'
New-Item -ItemType Directory -Force $work | Out-Null
$exe = Join-Path $work 'studentmanager.exe'
& gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Werror -O2 (Join-Path $project 'src/main.c') -o $exe
if ($LASTEXITCODE -ne 0) { throw 'Compilation echouee' }
$script:checks = 0
function Assert-True($condition, $message) {
    if (-not $condition) { throw $message }
    $script:checks++
}
function Run-App([string]$data, [string]$inputText) {
    $output = ($inputText | & $exe $data) -join "`n"
    if ($LASTEXITCODE -ne 0) { throw 'Execution echouee' }
    return $output
}
function Write-Data([string]$path, [string]$value) {
    [IO.File]::WriteAllText($path, $value, (New-Object Text.UTF8Encoding($false)))
}
$data = Join-Path $work 'valid.txt'
$out = Run-App $data "1`n101`nBenali`nAmine`n3`n12`n15`n16.5`n1`n102`nMartin`nLea`n0`n2`n3`n101`n7`n0"
Assert-True ($out.Contains('Moyenne : 14.50/20')) 'Moyenne incorrecte'
Assert-True ($out.Contains('Moyenne : non disponible')) 'Sans note incorrect'
Assert-True ((Get-Content $data).Count -eq 3) 'Sauvegarde incorrecte'
$out = Run-App $data "8`n4`n102`nDurand`nLea`n1`n19`n6`n2`n2`n7`n5`n101`n1`n7`n0"
Assert-True ($out.IndexOf('ID 102') -lt $out.IndexOf('ID 101')) 'Tri des moyennes incorrect'
Assert-True ((Get-Content $data).Count -eq 2) 'Suppression incorrecte'
Assert-True ((Get-Content "$data.bak").Count -eq 3) 'Sauvegarde de secours incorrecte'
$out = Run-App $data "8`n3`n102`n3`n101`n0"
Assert-True ($out.Contains('Durand Lea') -and $out.Contains('19.00/20')) 'Modification non persistante'
Assert-True ($out.Contains('Etudiant introuvable.')) 'Recherche absente incorrecte'
$out = Run-App $data "abc`n999`n1`n0`n1`n`nA;B`nBenali`nAmine`n11`n1`nnan`ninf`n20.1`n-1`n14,5`n14abc`n14.5`n1`n1`n2`n0`n1"
Assert-True ($out.Contains('Entrez un entier')) 'Validation des entiers absente'
Assert-True ($out.Contains('Nom requis')) 'Validation des noms absente'
Assert-True (($out.Split(@('Note invalide.'), [StringSplitOptions]::None).Count - 1) -eq 6) 'Validation des notes incorrecte'
Assert-True ($out.Contains('Identifiant deja utilise')) 'Doublon accepte'
$bad = Join-Path $work 'invalid.txt'
$cases = @(
    'MAUVAIS;1',
    "STUDENTMANAGER;1`n1;Nom;Prenom;1;nan",
    "STUDENTMANAGER;1`n1;Nom;Prenom;0`n1;Autre;Prenom;0",
    "STUDENTMANAGER;1`n1;;Prenom;0",
    "STUDENTMANAGER;1`n1;Nom;Prenom;1",
    "STUDENTMANAGER;1`n1;Nom;Prenom;0;extra",
    "STUDENTMANAGER;1`n1;Nom;Prenom;11",
    "STUDENTMANAGER;1`n1;Nom;Prenom;1;21",
    ("STUDENTMANAGER;1`n" + ('X' * 1100))
)
foreach ($case in $cases) {
    Write-Data $bad $case
    $out = Run-App $bad "1`n9`nTemoin`nTest`n0`n8`n1`n2`n0`n1"
    Assert-True ($out.Contains('Donnees conservees') -and $out.Contains('ID 9 | Temoin Test')) 'Chargement invalide non transactionnel'
}
$rows = 1..501 | ForEach-Object { "$_;Nom;Prenom;0" }
Write-Data $bad ("STUDENTMANAGER;1`n" + ($rows -join "`n"))
$out = Run-App $bad "8`n2`n0"
Assert-True ($out.Contains('Donnees conservees') -and $out.Contains('Aucun etudiant')) 'Capacite depassee acceptee'
$good = Join-Path $work 'crlf.txt'
Write-Data $good "STUDENTMANAGER;1`r`n2;Zulu;Test;0`r`n1;Alpha;Test;0"
$out = Run-App $good "8`n6`n1`n2`n0`n1"
Assert-True ($out.IndexOf('ID 1') -lt $out.IndexOf('ID 2')) 'Tri des noms ou CRLF incorrect'
$out = Run-App $good "1`n8`nIncomplet"
Assert-True (-not $out.Contains('Etudiant ajoute.')) 'Ajout incomplet applique'
$out = Run-App (Join-Path $work 'absent-dir/data.txt') "7`n0"
Assert-True ($out.Contains('Impossible de creer')) 'Erreur ecriture non signalee'
$out = Run-App (Join-Path $work 'absent-dir/data.txt') "8`n0"
Assert-True ($out.Contains('fichier absent ou inaccessible')) 'Erreur lecture non signalee'
Write-Output "SUCCES : $script:checks verifications."
