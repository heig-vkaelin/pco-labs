#include "mythread.h"

void runComputation(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        long long unsigned nbToCompute,
        long long unsigned totalToCompute,
        long long unsigned startingIndex,
        bool* finished,
        QString* result,
        ThreadManager* manager
        ) {
    // Vérification des différents pointeurs passés en paramètre
    if (finished == nullptr || result == nullptr || manager == nullptr) {
        qInfo() << "Erreur lors de l'appel de la fonction runComputation: pointeur nu§ll";
        return;
    }

    /*
     * Index servant à incrémenter le password testé dans la boucle
     */
    unsigned i;

    /*
     * Nombre de hashs testés par le thread
     */
    long long unsigned nbComputed = 0;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned nbValidChars = charset.length();

    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * On initialise le premier mot de passe à tester courant en le remplissant
     * de nbChars fois du premier caractère de charset
     */
    currentPasswordString.fill(charset.at(0), nbChars);
    currentPasswordArray.fill(0, nbChars);

    /*
     * Positionnement initial avec startingIndex.
     * On utilise la longueur du charset (nbValidChars) comme base pour calculer les différents
     * index initiaux du tableau représentant le password via des opérations
     * logarithmiques.
     */
    if(startingIndex != 0) {
        long long unsigned n = startingIndex;
        int power = log(n) / log(nbValidChars);

        for (long long int i = power; i >= 0; --i) {
            unsigned value = n / pow(nbValidChars, i);
            qInfo() << "index: " << i << " -> " << value << " ";
            currentPasswordArray[i] = value;
            n -= value * pow(nbValidChars, i);
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (unsigned i=0; i < nbChars; i++)
            currentPasswordString[i] = charset.at(currentPasswordArray.at(i));
    }

    /*
     * Tant qu'on a pas tout essayé et qu'aucun autre thread n'a trouvé le hash
     */
    while (nbComputed < nbToCompute && !*finished) {
        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if (currentHash == hash) {
            *result = currentPasswordString;
            *finished = true;
            return;
        }

        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if ((nbComputed % 1000) == 0) {
            manager->incrementPercentComputed((double)1000/totalToCompute);
        }

        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        i = 0;

        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0; i < nbChars; i++)
            currentPasswordString[i] = charset.at(currentPasswordArray.at(i));

        nbComputed++;
    }

    /*
     * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
     * été testés, et qu'aucun n'est la préimage de ce hash.
     */
    return;
}

