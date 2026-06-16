#pragma once

#include "configuration.h"
#include "graphics/niche/InkHUD/Applet.h"
#include "modules/TextMessageModule.h"

#include "graphics/niche/InkHUD/Renderer.h"
#include "mesh/generated/meshtastic/mesh.pb.h"
#include "mesh/generated/meshtastic/telemetry.pb.h"
#include "Observer.h"

namespace NicheGraphics::InkHUD {
//namespace inkhud {
class Applet;

//class AllMessageApplet : public Applet {
class temphGem : public Applet {
public:
    temphGem();
    // {
    //     loopbackOk = true; // Allow locally generated messages to loop back to the client
    //     };
    ~temphGem() override;

    // // Fonction appelée par le moteur de rendu d'InkHUD
    // void draw(Renderer& renderer, int x, int y, int width, int height) override;
    void onRender(bool full) override;
    // // Identifiant de l'applet (utile pour les menus ou le défilement)
    // const char* getName() const override;

private:
    float lastTemperature;
    float lastHumidity;
    bool hasData;

    // Fonction de rappel lors de la réception d'un paquet sur le mesh
    int onPacketReceived(const meshtastic_MeshPacket* packet);
    
    // L'observateur pour s'abonner au flux réseau
    
    //Observer<const meshtastic_MeshPacket*> packetObserver;
    //    // Used to register our text message callback
    CallbackObserver<temphGem, const meshtastic_MeshPacket *> packetObserver =
        CallbackObserver<temphGem, const meshtastic_MeshPacket *>(this, &temphGem::onPacketReceived);
};

} // namespace inkhud