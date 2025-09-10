#ifdef MESHTASTIC_INCLUDE_INKHUD

#pragma once

#include "configuration.h"
#include "graphics/niche/InkHUD/Applet.h"
#include "modules/Telemetry/EnvironmentTelemetry.h"
#include "mesh/SinglePortModule.h"
// #include "NodeDB.h"
#include "ProtobufModule.h"
// #include "../mesh/generated/meshtastic/telemetry.pb.h"

namespace NicheGraphics::InkHUD
{

//class tempHygroTelem : public Applet, public EnvironmentTelemetryModule//, public ProtobufModule<meshtastic_Telemetry>
class tempHygroTelem : public Applet, public ProtobufModule<meshtastic_Telemetry>
{

  public:

    tempHygroTelem() : ProtobufModule("EnvironmentTelemetry", meshtastic_PortNum_TELEMETRY_APP, &meshtastic_Telemetry_msg) {  // EnvironmentTelemetryModule() {
       loopbackOk = true; // Allow locally generated messages to loop back to the client
       lastMeasurementPacket = nullptr;
       nodeStatusObserver.observe(&nodeStatus->onNewStatus);      
    }

    // All drawing happens here
    void onRender() override;
    void onActivate() override;
    void onDeactivate() override;
    // Your applet might also want to use some of these
    // Useful for setting up or tidying up

    // bool isTelemetryUpdate(const meshtastic::Status *status);
    // bool decodeTelemetryPacket(const meshtastic_Telemetry *t);
  protected:
//  CallbackObserver<PairingApplet,       const meshtastic::Status *>     bluetoothStatusObserver  = CallbackObserver<PairingApplet,       const meshtastic::Status *>      (this, &PairingApplet::onBluetoothStatusUpdate);  
//  CallbackObserver<NotificationApplet,  const meshtastic_MeshPacket *>  textMessageObserver      = CallbackObserver<NotificationApplet,  const meshtastic_MeshPacket *>   (this, &NotificationApplet::onReceiveTextMessage);
//  CallbackObserver<BatteryIconApplet,   const meshtastic::Status *>     powerStatusObserver      = CallbackObserver<BatteryIconApplet,   const meshtastic::Status *>      (this, &BatteryIconApplet::onPowerStatusUpdate);
//  CallbackObserver<EnvironmentTeModule, const meshtastic::Status *>     nodeStatusObserver       = CallbackObserver<EnvironmentTeModule, const meshtastic::Status *>      (this, &EnvironmentTeModule::handleStatusUpdate);
  CallbackObserver<tempHygroTelem, const meshtastic::Status *>     nodeStatusObserver       = CallbackObserver<tempHygroTelem, const meshtastic::Status *>      (this, &tempHygroTelem::handleStatusUpdate);
//CallbackObserver<tempHygroTelem, const meshtastic_Telemetry *>   telemObserver            = CallbackObserver<tempHygroTelem, const meshtastic_MeshPacket/*meshtastic_Telemetry*/ *>    (this, &tempHygroTelem::decodeTelemetryPacket);

    //virtual bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Telemetry *p) override;
    //virtual int32_t runOnce() override;
    // Called to get current Environment telemetry data    @return true if it contains valid data
    //bool getEnvironmentTelemetry(meshtastic_Telemetry *m);
    //virtual meshtastic_MeshPacket *allocReply() override;
    // Send our Telemetry into the mesh
    //bool sendTelemetry(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

    //virtual AdminMessageHandleResult handleAdminMessageForModule(const meshtastic_MeshPacket &mp, meshtastic_AdminMessage *request,meshtastic_AdminMessage *response) override;
    
    // void onActivate();   // When started
    // void onDeactivate(); // When stopped
    // void onForeground(); // When shown by short-press
    // void onBackground(); // When hidden by short-press
    virtual bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Telemetry *p) override;
  private:
    // Called when we receive new text messages
    // Part of the MeshModule API
    ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;//, meshtastic_Telemetry *p

    // Store info from handleReceived
    bool haveMessage = false;
    NodeNum fromWho = 0;
    unsigned int countMsg = 0;
    float temperature = 0;
    float hygrometrie = 0;
    float batterie = 0;

    bool firstTime = 1;
    meshtastic_MeshPacket *lastMeasurementPacket;
    uint32_t sendToPhoneIntervalMs = SECONDS_IN_MINUTE * 1000; // Send to phone every minute
    uint32_t lastSentToMesh = 0;
    uint32_t lastSentToPhone = 0;
    uint32_t sensor_read_error_count = 0;

};

} // namespace NicheGraphics::InkHUD

#endif