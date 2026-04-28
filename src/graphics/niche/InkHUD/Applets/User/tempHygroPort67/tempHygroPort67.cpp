#ifdef MESHTASTIC_INCLUDE_INKHUD

#include "./tempHygroPort67.h"
//#include "ProtobufModule.h"

using namespace NicheGraphics;

void InkHUD::tempHygroPort67::onActivate()
{
//  textMessageObserver.observe(textMessageModule);
}

void InkHUD::tempHygroPort67::onDeactivate()
{
//    textMessageObserver.unobserve(textMessageModule);
}

// We configured the Module API to call this method when we receive a new text message
ProcessMessage InkHUD::tempHygroPort67::handleReceived(const meshtastic_MeshPacket &mp) //, meshtastic_Telemetry *p
{

    // Abort if applet fully deactivated
    // Don't waste time: we wouldn't be rendered anyway
    //if (!isActive()) {return ProcessMessage::CONTINUE;}

    if (isFromUs(&mp)) {
        // Store the sender's nodenum
        // We need to keep this information, so we can re-use it anytime render() is called
        haveMessage = true;
        fromWho = mp.from;
        countMsg++;

        auto &pp = mp.decoded;
        LOG_WARN("Received port67 module from=0x%0x, id=0x%x, portnum=%d, payloadlen=%d", mp.from, mp.id, pp.portnum, pp.payload.size);

        const auto &p = mp.decoded;
        meshtastic_Telemetry scratch;
        meshtastic_Telemetry *decoded = NULL;
        memset(&scratch, 0, sizeof(scratch));
        if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, &meshtastic_Telemetry_msg, &scratch)) {
            decoded = &scratch;
            bool hasData = false;
            //if (t->which_variant == meshtastic_Telemetry_environment_metrics_tag) {
            if (decoded->which_variant!=meshtastic_Telemetry_environment_metrics_tag){
                LOG_INFO("not a telemetry message");
                return ProcessMessage::CONTINUE;
            }
            if (decoded->variant.environment_metrics.has_temperature){
                temperature = round(decoded->variant.environment_metrics.temperature*10);
                hasData = true;
            }
            if (decoded->variant.environment_metrics.has_relative_humidity){
                hygrometrie = decoded->variant.environment_metrics.relative_humidity;
                hasData = true;                
            }
            if (hasData){
                // std::string timeString = getTimeString(decoded->time);
                // if (timeString.length() > 0) {
                //     dateTime = timeString;
                // }
                
                // Tell InkHUD that we have something new to show on the screen
                requestAutoshow(); // Want to become foreground, if permitted
                requestUpdate();   // Want to update display, if applet is foreground     
            }       
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
void InkHUD::tempHygroPort67::onRender()
{
    printAt(0, 0, "tempHygroPort67", LEFT, TOP); // Print top-left corner of text at (0,0)

    int16_t centerX = X(0.5); // Same as width() / 2
    int16_t centerY = Y(0.5); // Same as height() / 2
/*
    std::string header;

    // RX Time
    // - if valid
    std::string timeString = getTimeString(latestMessage->dm.timestamp);
    if (timeString.length() > 0) {
        header += timeString;
        header += ": ";
    }

    // Sender's id
    // - shortname and long name, if available, or
    // - node id
    meshtastic_NodeInfoLite *sender = nodeDB->getMeshNode(latestMessage->dm.sender);
    if (sender && sender->has_user) {
        header += parseShortName(sender); // May be last-four of node if unprintable (emoji, etc)
        header += " (";
        header += parse(sender->user.long_name);
        header += ")";
    } else
        header += hexifyNodeNum(latestMessage->dm.sender);

    // Draw a "standard" applet header
    drawHeader(header);    
*/
    if (haveMessage) {
        // printAt(0, centerY-40, "New data", LEFT, MIDDLE);
        // printAt(centerX, centerY-40, "From: " + hexifyNodeNum(fromWho), CENTER, MIDDLE);
//        printAt(0, centerY-20, "New data", LEFT, MIDDLE);
        printAt(centerX, centerY-20, dateTime, CENTER, MIDDLE);
        printAt(0, centerY, "nb msg", LEFT, MIDDLE);
        printAt(centerX, centerY, to_string(countMsg), CENTER, MIDDLE);
        printAt(0, centerY+20, "Temperature=", LEFT, MIDDLE);
setFont(fontLarge);
        printAt(centerX, centerY+20, to_string(temperature/10), CENTER, MIDDLE);
setFont(fontSmall);
        printAt(0, centerY+40, "Hygro=", LEFT, MIDDLE);
setFont(fontLarge);
        printAt(centerX, centerY+40, to_string(hygrometrie), CENTER, MIDDLE);        
        //const char *sender = getSenderShortName(mp);
        // LOG_INFO("(Received from %s):" , sender);
    } else {
        printAt(centerX, centerY, "No data", CENTER, MIDDLE); // Place center of string at (centerX, centerY)
        printAt(centerX, centerY+20, to_string(countMsg), CENTER, TOP);
    }
}

// bool decodeTelemetryPacket67(const meshtastic_Telemetry *t){
//  return true;
// }

// bool isTelemetryUpdate67(const meshtastic::Status *status){
//  return true;
// }
    
/*
bool EnvironmentTelemetryModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Telemetry *t)//EnvironmentTelemetryModule
{
    if (t->which_variant == meshtastic_Telemetry_environment_metrics_tag) {
#ifdef DEBUG_PORT
        const char *sender = getSenderShortName(mp);

        LOG_INFO("(Received from %s): barometric_pressure=%f, current=%f, gas_resistance=%f, relative_humidity=%f, "
                 "temperature=%f",
                 sender, t->variant.environment_metrics.barometric_pressure, t->variant.environment_metrics.current,
                 t->variant.environment_metrics.gas_resistance, t->variant.environment_metrics.relative_humidity,
                 t->variant.environment_metrics.temperature);
        LOG_INFO("(Received from %s): voltage=%f, IAQ=%d, distance=%f, lux=%f, white_lux=%f", sender,
                 t->variant.environment_metrics.voltage, t->variant.environment_metrics.iaq,
                 t->variant.environment_metrics.distance, t->variant.environment_metrics.lux,
                 t->variant.environment_metrics.white_lux);

        LOG_INFO("(Received from %s): wind speed=%fm/s, direction=%d degrees, weight=%fkg", sender,
                 t->variant.environment_metrics.wind_speed, t->variant.environment_metrics.wind_direction,
                 t->variant.environment_metrics.weight);

        LOG_INFO("(Received from %s): radiation=%fµR/h", sender, t->variant.environment_metrics.radiation);

#endif
        // release previous packet before occupying a new spot
        if (lastMeasurementPacket != nullptr)
            packetPool.release(lastMeasurementPacket);

        lastMeasurementPacket = packetPool.allocCopy(mp);
    }

    return false; // Let others look at this message also if they want
}
*/
#endif