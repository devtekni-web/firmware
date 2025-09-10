#ifdef MESHTASTIC_INCLUDE_INKHUD

#include "./tempHygroTelem.h"
#include "modules/TextMessageModule.h"

using namespace NicheGraphics;

void InkHUD::tempHygroTelem::onActivate()
{
    nodeStatusObserver.observe(&nodeStatus->onNewStatus);
}

void InkHUD::tempHygroTelem::onDeactivate()
{
    nodeStatusObserver.unobserve(&nodeStatus->onNewStatus);
}

// We configured the Module API to call this method when we receive a new text message
ProcessMessage InkHUD::tempHygroTelem::handleReceived(const meshtastic_MeshPacket &mp) //, meshtastic_Telemetry *p
{

    // Abort if applet fully deactivated
    // Don't waste time: we wouldn't be rendered anyway
    if (!isActive()) {return ProcessMessage::CONTINUE;}

    // Check that this is an incoming message
    // Outgoing messages (sent by us) will also call handleReceived
        //meshtastic_Telemetry *p = mp;
    if (isFromUs(&mp)) {
        // Store the sender's nodenum
        // We need to keep this information, so we can re-use it anytime render() is called
        haveMessage = true;
        fromWho = mp.from;
        countMsg++;
        // auto &p = mp.decoded;
        // LOG_WARN("Received telem module from=0x%0x, id=0x%x, portnum=%d, payloadlen=%d", mp.from, mp.id, p.portnum, p.payload.size);
        const auto &p = mp.decoded;
        meshtastic_Telemetry scratch;
        meshtastic_Telemetry *decoded = NULL;
        memset(&scratch, 0, sizeof(scratch));
        if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, &meshtastic_Telemetry_msg, &scratch)) {
            decoded = &scratch;
            temperature = decoded->variant.environment_metrics.temperature;
            hygrometrie = decoded->variant.environment_metrics.relative_humidity;
            // Tell InkHUD that we have something new to show on the screen
            requestUpdate();
        } else {
            LOG_ERROR("Error decoding EnvironmentTelemetry module!");
        }

    }

    // Tell Module API to continue informing other firmware components about this message
    // We're not the only component which is interested in new text messages
    return ProcessMessage::CONTINUE;
}

// All drawing happens here
// We can trigger a render by calling requestUpdate()
// Render might be called by some external source
// We should always be ready to draw
void InkHUD::tempHygroTelem::onRender()
{
    printAt(0, 0, "tempHygroTelem", LEFT, TOP); // Print top-left corner of text at (0,0)

    int16_t centerX = X(0.5); // Same as width() / 2
    int16_t centerY = Y(0.5); // Same as height() / 2

    if (haveMessage) {
        printAt(0, centerY-20, "New data", LEFT, MIDDLE);
        printAt(centerX, centerY-20, "From: " + hexifyNodeNum(fromWho), CENTER, MIDDLE);
        printAt(0, centerY, "nb msg", LEFT, MIDDLE);
        printAt(centerX, centerY, to_string(countMsg), CENTER, MIDDLE);
        printAt(0, centerY+20, "Temperature=", LEFT, MIDDLE);
        printAt(centerX, centerY+20, to_string(temperature), CENTER, MIDDLE);
        printAt(0, centerY+40, "Hygro=", LEFT, MIDDLE);
        printAt(centerX, centerY+40, to_string(hygrometrie), CENTER, MIDDLE);    
    } else {
        printAt(centerX, centerY, "No telem", CENTER, MIDDLE); // Place center of string at (centerX, centerY)
        printAt(centerX, centerY+20, to_string(countMsg), CENTER, TOP);
    }
}

// bool decodeTelemetryPacket(const meshtastic_Telemetry *t){
//  return true;
// }


bool InkHUD::tempHygroTelem::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Telemetry *t)//EnvironmentTelemetryModule
{
    if (t->which_variant == meshtastic_Telemetry_environment_metrics_tag) {
#ifdef DEBUG_PORT
        const char *sender = getSenderShortName(mp);

        LOG_INFO("(=Received from %s): barometric_pressure=%f, current=%f, gas_resistance=%f, relative_humidity=%f, "
                 "temperature=%f",
                 sender, t->variant.environment_metrics.barometric_pressure, t->variant.environment_metrics.current,
                 t->variant.environment_metrics.gas_resistance, t->variant.environment_metrics.relative_humidity,
                 t->variant.environment_metrics.temperature);
        LOG_INFO("(=Received from %s): voltage=%f, IAQ=%d, distance=%f, lux=%f, white_lux=%f", sender,
                 t->variant.environment_metrics.voltage, t->variant.environment_metrics.iaq,
                 t->variant.environment_metrics.distance, t->variant.environment_metrics.lux,
                 t->variant.environment_metrics.white_lux);

        LOG_INFO("(=Received from %s): wind speed=%fm/s, direction=%d degrees, weight=%fkg", sender,
                 t->variant.environment_metrics.wind_speed, t->variant.environment_metrics.wind_direction,
                 t->variant.environment_metrics.weight);

        LOG_INFO("(=Received from %s): radiation=%fµR/h", sender, t->variant.environment_metrics.radiation);

#endif
        // release previous packet before occupying a new spot
        if (lastMeasurementPacket != nullptr)
            packetPool.release(lastMeasurementPacket);

        lastMeasurementPacket = packetPool.allocCopy(mp);
    }

    return false; // Let others look at this message also if they want
}

#endif