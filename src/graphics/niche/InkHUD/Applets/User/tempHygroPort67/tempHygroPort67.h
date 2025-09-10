#ifdef MESHTASTIC_INCLUDE_INKHUD

#pragma once

#include "configuration.h"
#include "graphics/niche/InkHUD/Applet.h"
#include "modules/Telemetry/EnvironmentTelemetry.h"
#include "mesh/SinglePortModule.h"
#include "ProtobufModule.h"
#include "Observer.h"

namespace NicheGraphics::InkHUD
{

class tempHygroPort67 : public Applet, public SinglePortModule, public Observable<const meshtastic_MeshPacket *>
{

  public:
    // The MeshModule API requires us to have a constructor, to specify that we're interested in Text Messages.
     // Constructor
     // name is for debugging output
    //SinglePortModule(const char *_name, meshtastic_PortNum _ourPortNum) : MeshModule(_name), ourPortNum(_ourPortNum) {}
    
    tempHygroPort67() : SinglePortModule("tempHygroPort67", meshtastic_PortNum_TELEMETRY_APP) {
      loopbackOk = true; // Allow locally generated messages to loop back to the client
      }
    void onActivate() override;
    void onDeactivate() override;
    // All drawing happens here
    void onRender() override;

  protected:

    // void onActivate();   // When started
    // void onDeactivate(); // When stopped
    // void onForeground(); // When shown by short-press
    // void onBackground(); // When hidden by short-press

  private:
    // Called when we receive new messages
    // Part of the MeshModule API
    ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;

    // Store info from handleReceived
    bool haveMessage = false;
    NodeNum fromWho = 0;
    unsigned int countMsg = 0;
    meshtastic_MeshPacket *lastMeasurePacket;
    std::string dateTime = 0;
    float temperature = 0;
    float hygrometrie = 0;
    float batterie = 0;

};

} // namespace NicheGraphics::InkHUD

#endif