#ifdef MESHTASTIC_INCLUDE_INKHUD

#pragma once

#include "configuration.h"
#include "graphics/niche/InkHUD/Applet.h"
#include "mesh/SinglePortModule.h"
#include "mesh/generated/meshtastic/telemetry.pb.h"

namespace NicheGraphics::InkHUD {

class EnvironementTelemetryApplet : public Applet, public SinglePortModule {
  public:
    EnvironementTelemetryApplet() 
        : SinglePortModule("EnvironementTelemetryApplet", meshtastic_PortNum_TELEMETRY_APP) {
          loopbackOk = true; // Allow locally generated messages to loop back to the client
        }

    void onRender(bool full) override;
    void onForeground() override;

  private:
    ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;

    // Stockage des dernières valeurs reçues
    bool hasData = false;
    float lastTemperature = 0.0f;
    float lastHumidity = 0.0f;
    uint32_t lastUpdateTime = 0;
};

} // namespace NicheGraphics::InkHUD

#endif