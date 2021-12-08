//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//                                         //
// Auteurs: Valentin Kaelin & Lazar Pavicevic

#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include "locomotive.h"
#include "launchable.h"
#include "sharedsectioninterface.h"
#include "route.h"

// Nombre de tours avant d'inverser le sens de la locomotive
#define NB_TURNS 2

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
    using LocoId = SharedSectionInterface::LocoId;
    using EntryPoint = SharedSectionInterface::EntryPoint;
public:
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     * \param sharedSection la section partagée entre les différentes locomotives
     * \param route parcours de la locomotive
     * \param locoId id de la locomotive
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection, Route& route, LocoId locoId)
        : loco(loco), sharedSection(sharedSection), route(route), locoId(locoId) {
    }

protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SharedSectionInterface> sharedSection;

    /**
     * @brief inverse Arrête la locomotive, inverse son sens et la redémarre
     */
    void inverse();

    /**
     * @brief route Parcours de la locomotive
     */
    Route& route;

    /**
     * @brief locoId Id de la locomotive
     */
    LocoId locoId;
};

#endif // LOCOMOTIVEBEHAVIOR_H
