---Utilisation de Predictive Text Algorithm par Martin Caron et Guillaume Brizolier---

###COMPILATION###

Notre projet utilise la bibliothèque gtk

1) Vous aurez peut-être besoin de l'installer. Pour ce faire, rien de plus simple : dans votre terminal linux préféré, tapez la commande "sudo apt-get install libgtk2.0-dev" et le tour est joué

2) Pour compiler notre projet, il vous suffit de faire appel à la commande "gcc cproject.c $(pkg-config --libs --cflags gtk+-2.0) -o p"
Vous n'aurez ainsi qu'à taper "./p" pour lancer le projet


###Guide d'utilisation de la fenêtre qui pop up à l'appel de la fonction###


**Suppression d'un mot du dictionnaire : 

tapez le mot à supprimer puis cliquez sur le bouton en bas à droite avant d'appuyer sur espace.

**Ajout d'un mot au dictionnaire : 

L'ajout se fait automatiquement à chaque fois que l'utilisateur appuie sur espace (ou mise à jour si le mot avant l'espace existe déjà). Nous ne demandons pas l'avis de l'utilisateur avant de récolter ses données personnelles. C'est aussi cela, le machine Learning.

**Desactivation de la prédiction

Pour désactiver la fonction, il suffit d'appuyer sur le bouton situé en bas à gauche de la fenêtre. Pour le réactiver, il suffit d'appuyer à nouveau dessus.

**Mots proposés

Les trois boutons situés sur la partie supérieure de l'interface indiquent les mots correspondants au mot qui est en train d'être tapé, classés par nombre d'occurences de telle manière à ce que le mot qui apparaît sur le bouton en haut à gauche est le plus fréquent, et celui du bouton en haut à droite le moins fréquent.

Si l'utilisateur clique sur un de ces 3 boutons, le mot qu'il était en train de taper se verra remplacé par le texte que le bouton sur lequel il a cliqué contient.


**Eventuelles erreurs

Par manque de temps, nous n'avons pas pu traiter exhaustivement tous les cas. Il existe donc des erreurs courantes, qui peuvent être évitées de la manière suivante :

- ne pas taper sur autre chose que les touches qui correspondent à des lettres. Les majuscules et les accents ne sont pas gérés. Vous pouvez effacer un mot, mais il faut le faire en entier puis appuyer sur la barre d'espace, ce qui occasionnera l'ajout du mot "Effacez_Votre_Mot_En_Entier" dans le dictionnaire. 

- taper un mot trop long occasionne une erreur de segmentation (la mémoire allouée à un mot courrant est fixe.) veuillez donc vous limiter à 60 caractères. 

- on ne peut pas utiliser un bouton de suggestion puis appuyer sur supprimer pour supprimer ce mot. Il faut l'écrire à la main. 

