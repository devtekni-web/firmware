#ifdef MESHTASTIC_INCLUDE_INKHUD

#pragma once

#include "configuration.h"
#include "graphics/niche/InkHUD/Applet.h"
//#include "modules/Telemetry/EnvironmentTelemetry.h"
#include "modules/Telemetry/Sensor/AHT10.h"
#include "mesh/SinglePortModule.h"
//#include "ProtobufModule.h"
#include "Observer.h"
#include "modules/TextMessageModule.h"
#include "RTC.h"

namespace NicheGraphics::InkHUD
{

class tempHygroPort67 : public Applet, public SinglePortModule //, public Observable<const meshtastic_MeshPacket *>
{

  public:
  AHT10Sensor sensor;

    // modif dans default.h
    
    //#define min_default_telemetry_interval_secs IF_ROUTER(ONE_DAY / 2, 1 * 60)
    //#define default_telemetry_broadcast_interval_secs IF_ROUTER(ONE_DAY / 2, 2 * 60)
    // modif dans sensors/AHT10.h
    //#define AHT10_TEMP_OFFSET 0

    #define AHT10_TEMP_OFFSET 0
    #define min_default_telemetry_interval_secs IF_ROUTER(ONE_DAY / 2, 1 * 60)
    #define default_telemetry_broadcast_interval_secs IF_ROUTER(ONE_DAY / 2, 2 * 60)
    // The MeshModule API requires us to have a constructor, to specify that we're interested in Text Messages.
     // Constructor
     // name is for debugging output
    //SinglePortModule(const char *_name, meshtastic_PortNum _ourPortNum) : MeshModule(_name), ourPortNum(_ourPortNum) {}
    
    tempHygroPort67() : SinglePortModule("tempHygroPort67", meshtastic_PortNum_TELEMETRY_APP) {
      loopbackOk = true; // Allow locally generated messages to loop back to the client
    }

    void onActivate() override;
    void onDeactivate() override;
    void onForeground() override; // When shown by short-press
    // All drawing happens here
    void onRender(bool full) override;

    int onReceiveTextMessage(const meshtastic_MeshPacket *p);

  protected:

      // Used to register our text message callback
    CallbackObserver<tempHygroPort67, const meshtastic_MeshPacket *> packetObserver =
        CallbackObserver<tempHygroPort67, const meshtastic_MeshPacket *>(this, &tempHygroPort67::onReceiveTextMessage);

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
    meshtastic_NodeInfoLite *sender;
    unsigned int countMsg = 0;
    meshtastic_MeshPacket *lastMeasurePacket;
    std::string dateTime ;
    float temperature = 0;
    float hygrometrie = 0;
    float batterie = 0;
    unsigned int myName = 0;
    std::string longName;  // handled below
    std::string shortName; // handled below
    uint32_t curTime = 0;
};

} // namespace NicheGraphics::InkHUD

#endif