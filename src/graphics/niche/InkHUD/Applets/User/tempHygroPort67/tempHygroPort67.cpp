#ifdef MESHTASTIC_INCLUDE_INKHUD

#include "./tempHygroPort67.h"
//#include "ProtobufModule.h"
//#include "EnvironmentTelemetry.h"

using namespace NicheGraphics;


// We're not consuming the data passed to this method;
// we're just just using it to trigger a render
int InkHUD::tempHygroPort67::onReceiveTextMessage(const meshtastic_MeshPacket *p)
{
    // Abort if applet fully deactivated
    // Already handled by onActivate and onDeactivate, but good practice for all applets
    if (!isActive())
        return 0;

    // If DM (not broadcast)
    if (!isBroadcast(p->to)) {
        // Want to update display, if applet is foreground
        requestUpdate();

        // If this was an incoming message, suggest that our applet becomes foreground, if permitted
        if (getFrom(p) != nodeDB->getNodeNum())
            requestAutoshow();
    }

    // Return zero: no issues here, carry on notifying other observers!
    return 0;
}

void InkHUD::tempHygroPort67::onActivate()
{
    // piqué de dropzonemodule.cpp
    if (sensor.hasSensor()) {
        meshtastic_Telemetry telemetry = meshtastic_Telemetry_init_zero;
        sensor.getMetrics(&telemetry);

        auto temp = telemetry.variant.environment_metrics.temperature;
        auto hygro = telemetry.variant.environment_metrics.relative_humidity;

        LOG_INFO("init mesurement temp= %.2f, hygro= %.2f", temp, hygro);
    } else {
        LOG_ERROR("No sensor found");
        // sprintf(replyStr, "%s @ %02d:%02d:%02d\nNo sensor found", dropzoneStatus, hour, min, sec);
    }
  packetObserver.observe(textMessageModule);
}

void InkHUD::tempHygroPort67::onDeactivate()
{
    packetObserver.unobserve(textMessageModule);
}

void InkHUD::tempHygroPort67::onForeground()
{
    // piqué de dropzonemodule.cpp
    if (sensor.hasSensor()) {
        meshtastic_Telemetry telemetry = meshtastic_Telemetry_init_zero;
        sensor.getMetrics(&telemetry);

        auto temp = telemetry.variant.environment_metrics.temperature;
        auto hygro = telemetry.variant.environment_metrics.relative_humidity;
        LOG_INFO("init mesurement temp= %.2f, hygro= %.2f", temp, hygro);
    } else {
        LOG_ERROR("No sensor found");
    }
}

// We configured the Module API to call this method when we receive a new text message
ProcessMessage InkHUD::tempHygroPort67::handleReceived(const meshtastic_MeshPacket &mp) //, meshtastic_Telemetry *p
{

    // Abort if applet fully deactivated
    // Don't waste time: we wouldn't be rendered anyway
    if (!isActive()) {return ProcessMessage::CONTINUE;}
    //if (*mp == nullptr) 
    if (isFromUs(&mp)) {
        // Store the sender's nodenum
        // We need to keep this information, so we can re-use it anytime render() is called
        haveMessage = true;
        fromWho = mp.from;
        countMsg++;

        // auto &pp = mp.decoded;
        // LOG_WARN("Received port67 module from=0x%0x, id=0x%x, portnum=%d, payloadlen=%d", mp.from, mp.id, pp.portnum, pp.payload.size);

        const auto &p = mp.decoded;
        meshtastic_Telemetry scratch;
        meshtastic_Telemetry *decoded = NULL;
        memset(&scratch, 0, sizeof(scratch));

        if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, &meshtastic_Telemetry_msg, &scratch)) {
            decoded = &scratch;
            bool hasData = false;

            if (decoded->which_variant!=meshtastic_Telemetry_environment_metrics_tag){
                LOG_INFO("not a telemetry message");
                return ProcessMessage::CONTINUE;
            }
            if (decoded->variant.environment_metrics.has_temperature){
                temperature = decoded->variant.environment_metrics.temperature;
                hasData = true;
            }
            if (decoded->variant.environment_metrics.has_relative_humidity){
                hygrometrie = decoded->variant.environment_metrics.relative_humidity;
                hasData = true;                
            }
            if (hasData){
                std::string timeString = getTimeString(lastMeasurePacket->rx_time);
                // //if (timeString.length() > 0) {
                //     dateTime = timeString;
                // //}
                std::string clockString = getTimeString();

                if (clockString.length() > 0) {
                    // setFont(fontMedium);
                    // printAt(width / 2, top, clockString, CENTER, TOP);

                    // height += fontMedium.lineHeight();
                    // height += fontMedium.lineHeight() * 0.1; // Padding below clock
                    dateTime = clockString;
                }
                LOG_INFO("datetime= %s, clock = %s",timeString, clockString);

                meshtastic_NodeInfoLite *ourNode = nodeDB->getMeshNode(nodeDB->getNodeNum());
                myName = ourNode->num;
                // -- Shortname --
                // Parse special chars in the short name
                // Use "?" if unknown
                if (ourNode)
                    shortName = parseShortName(ourNode);
                else
                    shortName = "?";
                // -- Longname --
                // Parse special chars in long name
                // Use node id if unknown
                if (ourNode && ourNode->long_name!=NULL){// ->has_user)
                    longName = parse(ourNode->long_name);// ->user.long_name); // Found in nodeDB
                } else {
                    // Not found in nodeDB, show a hex nodeid instead
                    longName = hexifyNodeNum(myName);
                }
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
void InkHUD::tempHygroPort67::onRender(bool full)
{
    // get current time:
    if (getRTCQuality() == RTCQuality::RTCQualityNone) {
        LOG_DEBUG("no RTC available. why?!?");
    }
    
    uint32_t rtc_sec = getValidTime(getRTCQuality());//         getValidTime(RTCQuality::RTCQualityDevice, true); // Display local timezone
    if (rtc_sec == 0) {  
        rtc_sec = getValidTime(RTCQuality::RTCQualityDevice, true); // Display local timezone
        LOG_DEBUG("local timezone bug. why?!?");
    } else {
        LOG_DEBUG("RTC from %s available: %s", RtcName(getRTCQuality()), getTimeString());
    } 
    char timeString[16];
    int hour = 0;
    int minute = 0;
    //int second = 0;

    if (rtc_sec > 0) {
        long hms = rtc_sec % SEC_PER_DAY;
        hms = (hms + SEC_PER_DAY) % SEC_PER_DAY;

        hour = hms / SEC_PER_HOUR;
        minute = (hms % SEC_PER_HOUR) / SEC_PER_MIN;
        //second = (hms % SEC_PER_HOUR) % SEC_PER_MIN; // or hms % SEC_PER_MIN
    }

    //bool isPM = hour >= 12;
    if (config.display.use_12h_clock) {
        hour %= 12;
        if (hour == 0) {
            hour = 12;
        }
        snprintf(timeString, sizeof(timeString), "%d:%02d", hour, minute);
    } else {
        snprintf(timeString, sizeof(timeString), "%02d:%02d", hour, minute);
    }
    // end time

    int16_t centerX = X(0.5); // Same as width() / 2
    int16_t centerY = Y(0.5); // Same as height() / 2
    //int16_t droite = (int16_t)(Y(1)-5);
    // uint32_t currentTime = getValidTime(RTCQuality::RTCQualityDevice, true); // Current RTC time
    std::string ts = getTimeString(rtc_sec);
    std::string header;
    header += "*** TH67 - ";
    header += hexifyNodeNum(myName); // to_string(hex(myName));
    header += " - ";
    header += shortName;
    header += " - ";
    header += ts;
    header += " *** ";
    // Draw a "standard" applet header
    drawHeader(header);    

//    LOG_INFO("currentTime= %u, ts= %S", currentTime, timeString);
//    float tempHumide = 0.2831 * (powf(hygrometrie,0.2735)) * temperature + 0.0003018*powf(hygrometrie, 2) + 0.01289*hygrometrie - 4.0962;
    if (haveMessage) {
        char buffer[8];  // maximum expected length of the float

        setFont(fontSmall);
        printAt(5, centerY - 30, "nb msg", LEFT, MIDDLE);
        printAt(centerX + 40, centerY - 30, to_string(countMsg), CENTER, MIDDLE);
/*
        printAt(5, centerY, "TempHumide=", LEFT, MIDDLE);
        setFont(fontLarge);
        std::snprintf(buffer, 20, "%.1f", tempHumide);      
        std::string str(buffer);
        printAt(centerX + 40, centerY, buffer, CENTER, MIDDLE);
*/
        printAt(5, centerY, "heure= ", LEFT, MIDDLE);
        setFont(fontLarge);
        // std::snprintf(buffer, 20, "%.1f", tempHumide);      
        // std::string str(buffer);
        printAt(centerX + 40, centerY, timeString, CENTER, MIDDLE);

        setFont(fontSmall);
        printAt(5, centerY+20, "Temperature=", LEFT, MIDDLE);
        setFont(fontLarge);
        std::snprintf(buffer, 20, "%.1f", temperature);
        //std::string str(buffer);
        printAt(centerX + 40, centerY+20, buffer, CENTER, MIDDLE);

        setFont(fontSmall);
        printAt(5, centerY+40, "Hygro=", LEFT, MIDDLE);
        setFont(fontLarge);
        std::snprintf(buffer, 20, "%.1f", hygrometrie);
        printAt(centerX + 40, centerY + 40, buffer, CENTER, MIDDLE);        
    } else {
        printAt(centerX, centerY, "No data", CENTER, MIDDLE); // Place center of string at (centerX, centerY)
    }
}

#endif
