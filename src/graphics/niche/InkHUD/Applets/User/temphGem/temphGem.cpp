#include "temphGem.h"
#include "MeshService.h"
#include "pb_decode.h"
#include <stdio.h>
#include "RTC.h"

//namespace inkhud {
namespace NicheGraphics::InkHUD {

temphGem::temphGem() : lastTemperature(0.0f), lastHumidity(0.0f), hasData(false), packetObserver(this, &temphGem::onPacketReceived)     // Attachement de l'observateur à la méthode de la classe  
{
    // On s'abonne au routeur/service pour écouter les paquets entrants (RX)
    // if (service) {
    //     service->rxPackets.observe(&packetObserver);
    // }
    //loopbackOk = true; // Allow locally generated messages to loop back to the client
    packetObserver.observe(textMessageModule);
    LOG_DEBUG("TG observer started");
}

temphGem::~temphGem() {
    // if (service) {
    //     service->rxPackets.unobserve(&packetObserver);
    // }
    packetObserver.unobserve(textMessageModule);    
    LOG_DEBUG("TG observer started");
}

// const char* temphGem::getName() const {
//     return "Environnement";
// }

int temphGem::onPacketReceived(const meshtastic_MeshPacket* packet) {
    // On s'assure que le message correspond au port de l'application Télémétrie
    std::string timeStr = getTimeString(); 
    LOG_DEBUG("RTC : %s", timeStr);
    if (packet->decoded.portnum == meshtastic_PortNum_TELEMETRY_APP) {
        
        meshtastic_Telemetry telemetry = meshtastic_Telemetry_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(packet->decoded.payload.bytes, packet->decoded.payload.size);
            LOG_DEBUG("telemetry received");
        // Décodage du Protobuf
        if (pb_decode(&stream, meshtastic_Telemetry_fields, &telemetry)) {
            
            // On vérifie que la variante correspond bien à des métriques environnementales
            if (telemetry.which_variant == meshtastic_Telemetry_environment_metrics_tag) {
                lastTemperature = telemetry.variant.environment_metrics.temperature;
                lastHumidity = telemetry.variant.environment_metrics.relative_humidity;
                hasData = true;
                requestAutoshow(); // Want to become foreground, if permitted
                requestUpdate();   // Want to update display, if applet is foreground  
            }
        }
    } else {
            LOG_DEBUG("not a telemetry packet");
    }
    return 0; // Poursuivre le routage standard
}

//void temphGem::draw(Renderer& renderer, int x, int y, int width, int height) {
void temphGem::onRender(bool full) {
    // // Effacer la zone allouée à cet applet
    // renderer.fillRect(x, y, width, height, 0); // 0 pour blanc (selon l'enum de couleurs InkHUD)

    // // Configuration de la typographie (les constantes varient, ex: FONT_MEDIUM)
    // renderer.setFont(1); 
    
    if (hasData) {

        // time stuff ------------------
            // get current time:
    if (getRTCQuality() == RTCQuality::RTCQualityNone) {
        LOG_DEBUG("no RTC available. why?!?");
    }
    
    uint32_t rtc_sec = getValidTime(getRTCQuality());//         getValidTime(RTCQuality::RTCQualityDevice, true); // Display local timezone
    if (rtc_sec == 0) {  
        rtc_sec = getValidTime(RTCQuality::RTCQualityDevice, true); // Display local timezone
        LOG_DEBUG("local timezone bug. why?!?");
    } else {
        LOG_DEBUG("RTC from %s available: %s", RtcName(getRTCQuality()), getTimeString(rtc_sec));
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

        char tempStr[32];
        char humStr[32];
        snprintf(tempStr, sizeof(tempStr), "Temp: %.1f C", lastTemperature);
        snprintf(humStr, sizeof(humStr), "Hygro: %.1f %%", lastHumidity);

        LOG_INFO(tempStr);
        LOG_INFO(humStr);


        // Extra gap below the header
        int16_t textTop = 40;

        //------- display temp
        // Attempt to print with fontLarge
        uint32_t textHeight;
        setFont(fontLarge);
        textHeight = getWrappedTextHeight(0, width(), tempStr);
        if (textHeight <= (uint32_t)height()) {
            printWrapped(0, textTop, width(), tempStr);
            return;
        }
        // Fallback (too large): attempt to print with fontMedium
        setFont(fontMedium);
        textHeight = getWrappedTextHeight(0, width(), tempStr);
        if (textHeight <= (uint32_t)height()) {
            printWrapped(0, textTop, width(), tempStr);
            return;
        }
        // Fallback (too large): print with fontSmall
        setFont(fontSmall);
        printWrapped(0, textTop, width(), tempStr);
        //------------------ display hygro
        textHeight = getWrappedTextHeight(0, width(), humStr);
        if (textHeight <= (uint32_t)height()) {
            printWrapped(0, textTop+20, width(), humStr);
            return;
        }

        // Fallback (too large): attempt to print with fontMedium
        setFont(fontMedium);
        textHeight = getWrappedTextHeight(0, width(), humStr);
        if (textHeight <= (uint32_t)height()) {
            printWrapped(0, textTop+20, width(), humStr);
            return;
        }
        // Fallback (too large): print with fontSmall
        setFont(fontSmall);
        printWrapped(0, textTop+20, width(), humStr);

        // Fallback (too large): print with fontSmall
        setFont(fontSmall);
        printWrapped(0, textTop+50, width(), timeString);

        } else {
            setFont(fontMedium);
            printAt(X(0.5), Y(0.5), "pas encore de data", CENTER, MIDDLE);
        }
}

} // namespace inkhud