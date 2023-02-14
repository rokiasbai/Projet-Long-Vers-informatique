* scan_ip scan plage d'@ ip passée en argument comme ceci ./scan_ip 10.0.2.1 10.0.2.255 + copie un fichier en donnant les bons chemins (j'ai testé et ca marche)
* MAIS ATTENTION : établi une connexion TCP au port 80, ca peut produire des faux positifs ou négatifs, selon la configuration du réseau et les règles du pare-feu.
