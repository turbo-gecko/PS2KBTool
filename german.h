#ifndef _GERMAN_H_
#define _GERMAN_H_

/*
 * german.h
 * 
 * Textdefinitionen in englischer Sprache.
 *
 * Das Copyright für diese Software liegt bei Gary Hammond (ZL3GH) 2024-2025. Sie ist kostenlos
 * für nichtkommerzielle Zwecke.
 *
 * WARNUNG: VERWENDEN SIE diese Software NICHT in medizinischen Geräten oder für
 * andere unternehmenskritische Zwecke.
 *
 * Die Verwendung dieser Software könnte zu einem Paradoxon führen, das das Universum beendet, daher
 * die Verwendung ausschließlich auf eigenes Risiko. Es werden keine
 * ​​ausdrücklichen oder impliziten Garantien oder Gewährleistungen übernommen.
 */

#define T_HELP              "hilfe"
#define T_IS_INVALID        " ist ungültig"
#define T_OFF               "aus"
#define T_ON                "ein"

#define T_ON_OR_OFF         "Bitte verwenden Sie entweder 'ein' oder 'aus'"
#define T_PROG_MODE         "Programmiermodus..."

#define T_HELP_01           "Die folgenden Befehle sind verfügbar:"
#define T_HELP_02           "--Tastatur--"
#define T_HELP_03           "kbt <1|2|3>           - Konverterplattentyp einstellen"
#define T_HELP_04           "k101 <ein|aus>        - erweiterte 101 Tasten einstellen"
#define T_HELP_05           "kabd <µSec>           - AT bit verzögerung einstellen"
#define T_HELP_06           "kand <µSec>           - AT nächste byte verzögerung einstellen"
#define T_HELP_07           "kasd <µSec>           - AT starten bit verzögerung einstellen"
#define T_HELP_08           "kxbd <µSec>           - XT bit verzögerung einstellen"
#define T_HELP_09           "kxnd <µSec>           - XT nächste byte verzögerung einstellen"
#define T_HELP_10           "kxsd <µSec>           - XT starten bit verzögerung einstellen"
#define T_HELP_11           "--Seriell--"
#define T_HELP_12           "sbr <Baud>            - Host baudrate einstellen"
#define T_HELP_13           "scd <mSec>            - Zeichenverzögerung einstellen"
#define T_HELP_14           "sld <mSec>            - Zeilenverzögerung einstellen"
#define T_HELP_15           "sfc <ein|aus>         - XON/XOFF flusskontrolle einstellen"
#define T_HELP_16           "sen <ein|aus>         - Ausgabe zum seriellen port EIN/AUS schalten"
#define T_HELP_17           "--Debug--"
#define T_HELP_18           "reset                 - Gerät zurücksetzen"
#define T_HELP_19           "ccrc                  - EEPROM CRC berechnen"
#define T_HELP_20           "scrc                  - gespeichertes EEPROM CRC anzeigen"
#define T_HELP_21           "ep                    - Alle EEPROM Werte drucken"
#define T_HELP_22           "er <Adresse>          - Wert aus EEPROM adresse lesen"
#define T_HELP_23           "ew <Adresse> <Wert>   - Wert an EEPROM adresse schreiben"

#define T_HELP_40           "Tastatur: kbt,k101,kabd,kand,kasd,kxbd,kxnd,kxsd"
#define T_HELP_41           "Seriell : sbr,scd,sld,sfc,sen"
#define T_HELP_42           "Debug   : reset,ccrc,scrc,ep,er,ew"
#define T_HELP_43           "Geben sie 'hilfe' ein für ausführlichere hilfe"

#define T_MSG_01            "Berechnete CRC = "
#define T_MSG_02            "Gespeicherte CRC = "
#define T_MSG_03            "Befehl '" + command + "' nicht gefunden"
#define T_MSG_04            "Geben sie 'hilfe' ein für eine liste der befehle"
#define T_MSG_05            "Ungültiger platinen typ"
#define T_MSG_06            "Platinen typ = "
//#define T_MSG_07            ""
#define T_MSG_08            "101 tasten sind aktiviert"
#define T_MSG_09            "101 tasten sind deaktiviert"
#define T_MSG_10            "AT bit verzögerung = "
#define T_MSG_11            "AT nächste byte verzögerung = "
#define T_MSG_12            "AT starten bit verzögerung = "
#define T_MSG_13            "XT bit verzögerung = "
#define T_MSG_14            "XT nächste byte verzögerung = "
#define T_MSG_15            "XT starten bit verzögerung = "
#define T_MSG_16            "Host baudrate ist nicht gültig"
#define T_MSG_17            "Baudrate = "
#define T_MSG_18            "Zeichenverzögerung zu klein"
#define T_MSG_19            "Zeichenverzögerung = "
#define T_MSG_20            "Zeilenverzögerung zu klein"
#define T_MSG_21            "Zeilenverzögerung = "
#define T_MSG_22            "Xon/Xoff flusskontrolle ist aktiviert"
#define T_MSG_23            "Xon/Xoff flusskontrolle ist deaktiviert"
#define T_MSG_24            "Serielle ausgabe ist aktiviert"
#define T_MSG_25            "Serielle ausgabe ist deaktiviert"
#define T_MSG_26            "EEPROM adresse "
#define T_MSG_27            "EEPROM adresse nicht angegeben"
#define T_MSG_28            "Schreibe an EEPROM adresse "
#define T_MSG_29            "EEPROM wert nicht angegeben"

#endif // _GERMAN_H_
