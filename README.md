# TP OS User - Protocole BEUIP (V1)

## Présentation du projet
Ce projet consiste en la création du protocole **BEUIP** (BEUI over IP), inspiré du fonctionnement de NetBEUI. L'objectif est de permettre une communication décentralisée entre étudiants sur un réseau local Unix/Linux via des librairies POSIX.

## Membres du groupe
* **Kevin CEPHISE**
* **Sarra SOLTAN**
* **Nadir HALIMI**

## Structure et Organisation du Code
Conformément aux exigences de qualité demandées, chaque fonction du projet respecte la limite stricte de **20 lignes**. 

* **`servbeuip.c`** : Gère l'attachement au port **9998**, le broadcast d'identification au démarrage et le maintien de la table des 255 utilisateurs (IP/Pseudo).
* **`clibeuip.c`** : Client local permettant d'interagir avec le serveur via l'interface **127.0.0.1** pour garantir la sécurité du système.
* **`Makefile`** : Assure une compilation rigoureuse avec les options `-Wall` et `-Werror`.

## Fonctionnalités Implémentées (Étape 2)
Le protocole utilise un format de message structuré : `[Code][Chaîne "BEUIP"][Payload]` .
- **Identification** : Envoi automatique d'un broadcast et gestion des accusés de réception (Codes 1 et 2).
- **Commandes Locales** : Consultation de la liste des connectés (Code 3) et envoi de messages privés ou globaux (Codes 4 et 5).
- **Sécurité** : Vérification systématique de l'adresse source pour les commandes d'administration afin d'éviter les intrusions.

## Environnement de Test
Le projet a été développé et validé sur une machine virtuelle **Debian** configurée en mode pont sur le réseau de classe C `192.168.88.0`.

## Utilisation
1. Compiler le projet : `make`
2. Lancer le serveur : `./servbeuip <VotrePseudo>`
3. Utiliser le client : `./clibeuip liste` ou `./clibeuip mp <Pseudo> "<Message>"`
