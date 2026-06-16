#pragma once

#include "graphics/niche/InkHUD/Applet.h"
#include "NodeDB.h"
// L'inclusion exacte peut varier selon la structure de ton firmware
#include "mesh/generated/meshtastic/telemetry.pb.h"

namespace NicheGraphics::InkHUD {

class EnvTelemetryApplet : public Applet {
public:
    // Le constructeur de l'Applet
    EnvTelemetryApplet() : Applet("Environnement") {
        // Initialisation si nécessaire
    }

    // Surcharge de la méthode de dessin principale
    void draw(NicheGraphics* graphics) override {
        // Utilisation des polices standards d'InkHUD 
        // (fontLarge, fontMedium, fontSmall selon la taille de l'écran)
        graphics->setFont(fontLarge); 

        // Récupérer les informations du noeud local depuis la NodeDB
        auto myNode = nodeDB->getMeshNode(nodeDB->getNodeNum());

        // Vérification de la disponibilité des métriques environnementales
        // Note: L'accès exact aux métriques dépend de la version de nanopb/protobuf (ex: has_environment_metrics)
        if (myNode && myNode->device_metrics.has_environment_metrics) {
            
            float temp = myNode->device_metrics.environment_metrics.temperature;
            float humidity = myNode->device_metrics.environment_metrics.relative_humidity;

            char tempStr[32];
            char humStr[32];

            // Formatage des données
            snprintf(tempStr, sizeof(tempStr), "Temp: %.1f C", temp);
            snprintf(humStr, sizeof(humStr), "Hum: %.1f %%", humidity);

            // Affichage sur l'écran e-ink (Ajuster les coordonnées X,Y selon ton écran)
            graphics->drawString(10, 30, tempStr);
            graphics->drawString(10, 70, humStr);
            
        } else {
            // Affichage par défaut si le capteur (BME280/SHT40, etc.) n'a pas encore publié
            graphics->setFont(fontMedium);
            graphics->drawString(10, 40, "En attente du module...");
        }
    }

    // Optionnel : Fonction pour forcer le rafraîchissement 
    // à lier au système d'événements (Observer) de Meshtastic lors d'une mise à jour Télémétrie
    void onTelemetryUpdate() {
        // Indique au moteur InkHUD que l'affichage doit être redessiné
        setDirty(); 
    }
};

} // namespace InkHUD