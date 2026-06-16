#ifdef MESHTASTIC_INCLUDE_INKHUD

#include "EnvironementTelemetryApplet.h"
#include "graphics/niche/InkHUD/InkHUD.h"
#include "mesh/generated/meshtastic/telemetry.pb.h"
#include "mesh/NodeDB.h"
#include "configuration.h"

using namespace NicheGraphics::InkHUD;

void EnvironementTelemetryApplet::onRender(bool full) {
    if (!hasData) {
        printAt(0.5,0.5, "Waiting for data...");
        return;
    }

    // Convertir la température (stockée en degrés Celsius)
    float tempC = lastTemperature;
    
    // Afficher la température avec son unité
    char tempStr[32];
    snprintf(tempStr, sizeof(tempStr), "Temp: %.1f°C", tempC);
    printAt(0, 0, tempStr);
    
    // Afficher l'hygrométrie
    char humStr[32];
    snprintf(humStr, sizeof(humStr), "Humidity: %.1f%%", lastHumidity);
    printAt(0, 16, humStr);
    LOG_INFO("init mesurement temp= %.2f, hygro= %.2f", tempC, lastHumidity);
    // Afficher l'âge des données
    uint32_t now = millis();
    uint32_t ageSec = (now - lastUpdateTime) / 1000;
    char ageStr[32];
    snprintf(ageStr, sizeof(ageStr), "Updated: %lu sec ago", ageSec);
    printAt(0, 48, ageStr);
}

void EnvironementTelemetryApplet::onForeground() {
    // Forcer un rafraîchissement quand l'applet devient visible
    LOG_INFO("tthforeground");
    requestUpdate();
}

ProcessMessage EnvironementTelemetryApplet::handleReceived(const meshtastic_MeshPacket &mp) {
    // Vérifier que c'est un message de télémétrie
    if (mp.decoded.portnum != meshtastic_PortNum_TELEMETRY_APP) {
        return ProcessMessage::STOP;
    }

    meshtastic_Telemetry telemetry = meshtastic_Telemetry_init_zero;
    
    // Décoder le protobuf
    if (!pb_decode_from_bytes(mp.decoded.payload.bytes, mp.decoded.payload.size,
                              &meshtastic_Telemetry_msg, &telemetry)) {
        return ProcessMessage::STOP;
    }

    // Vérifier si c'est bien une télémétrie environnementale
    if (telemetry.which_variant == meshtastic_Telemetry_environment_metrics_tag) {
        // Extraire les données
        lastTemperature = telemetry.variant.environment_metrics.temperature;
        lastHumidity = telemetry.variant.environment_metrics.relative_humidity;
        hasData = true;
        lastUpdateTime = millis();
        LOG_INFO("telem msg received");
        // Demander un rafraîchissement de l'affichage
        requestUpdate();
        requestAutoshow();
    }

    return ProcessMessage::STOP;
}

#endif