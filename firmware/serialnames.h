#ifdef ESP8266
#include <pgmspace.h>
#endif // ESP8266

#include "stdextra.h"

#ifndef ESP8266
#define PROGMEM
#define PGM_P const char*
#define strlen_P strlen
#define strcat_P strcat
#endif // !ESP8266

inline std::string computeNameForId(uint32_t id) {

    static constexpr auto colors_maxlen = 6;
    static const char colors[] PROGMEM =
        "Amber\0Aqua\0Azure\0Beige\0Black\0Blue\0Blush\0Bronze\0Brown\0Coffee\0"
        "Copper\0Coral\0Cyan\0Gold\0Gray\0Green\0Indigo\0Ivory\0Jade\0Lime\0"
        "Maroon\0Olive\0Orange\0Peach\0Pink\0Plum\0Purple\0Red\0Rose\0Tan\0Teal\0"
        "White";

    static constexpr auto languages_maxlen = 7;
    static const char languages[] PROGMEM =
        "Akan\0Amharic\0Arabic\0Awadhi\0Balochi\0Bengali\0Burmese\0Cebuano\0"
        "Chewa\0Czech\0Deccan\0Dutch\0English\0French\0Fula\0Gan\0German\0Greek\0"
        "Hakka\0Hausa\0Hindi\0Hmong\0Igbo\0Ilocano\0Italian\0Jin\0Kannada\0"
        "Kazakh\0Khmer\0Kirundi\0Konkani\0Korean\0Kurdish\0Magahi\0Malay\0"
        "Marathi\0Marwari\0Min\0Mossi\0Nepali\0Odia\0Oromo\0Pashto\0Persian\0"
        "Polish\0Punjabi\0Quechua\0Russian\0Shona\0Sindhi\0Somali\0Tamil\0"
        "Telugu\0Thai\0Urdu\0Uyghur\0Uzbek\0Wu\0Xhosa\0Xiang\0Yoruba\0Yue\0"
        "Zhuang\0Zulu";

    static constexpr auto adjectives_maxlen = 4;
    static const char adjectives[] PROGMEM =
        "Able\0Back\0Bad\0Bare\0Big\0Bold\0Busy\0Calm\0Cold\0Cool\0Cute\0Damp\0"
        "Dark\0Dead\0Deaf\0Dear\0Deep\0Drab\0Dry\0Dual\0Due\0Dull\0Easy\0Evil\0"
        "Fair\0Far\0Fast\0Fat\0Few\0Fine\0Firm\0Fit\0Flat\0Fond\0Free\0Full\0"
        "Fun\0Gay\0Glad\0Good\0Grim\0Hard\0Head\0High\0Holy\0Hon\0Hot\0Huge\0"
        "Hurt\0Icy\0Ill\0Inc\0Just\0Keen\0Key\0Kind\0Late\0Lazy\0Left\0Like\0"
        "Live\0Long\0Lost\0Loud\0Low\0Ltd\0Mad\0Main\0Male\0Many\0Mass\0Mean\0"
        "Mere\0Mid\0Mild\0Mute\0Near\0Neat\0Net\0New\0Nice\0Nosy\0Odd\0Ok\0Okay\0"
        "Old\0Only\0Open\0Oral\0Pale\0Past\0Poor\0Puny\0Pure\0Rare\0Raw\0Real\0"
        "Rear\0Rich\0Ripe\0Rude\0Sad\0Safe\0Shy\0Sick\0Slim\0Slow\0Soft\0Sole\0"
        "Sore\0Sour\0Sure\0Tall\0Tame\0Tart\0Then\0Thin\0Tiny\0Top\0Tory\0Ugly\0"
        "Used\0Vast\0Very\0Warm\0Weak\0Wee\0Wet";

    static constexpr auto animals_maxlen = 4;
    static const char animals[] PROGMEM =
        "Ant\0Ape\0Asp\0Bass\0Bat\0Bear\0Bee\0Bird\0Boa\0Boar\0Bug\0Carp\0Cat\0"
        "Clam\0Cod\0Cow\0Crab\0Crow\0Deer\0Dog\0Dove\0Duck\0Eel\0Elk\0Emu\0Fish\0"
        "Flea\0Fly\0Fowl\0Fox\0Frog\0Goat\0Guan\0Gull\0Hare\0Hawk\0Jay\0Kite\0"
        "Kiwi\0Koi\0Lark\0Lion\0Loon\0Lynx\0Mink\0Mite\0Mole\0Moth\0Mule\0Newt\0"
        "Orca\0Owl\0Ox\0Pig\0Pike\0Pony\0Puma\0Rat\0Rook\0Slug\0Sole\0Swan\0"
        "Tahr\0Yak";

    char result[colors_maxlen + languages_maxlen + adjectives_maxlen + animals_maxlen + 1] = { 0 };

    const auto cat = [&](PGM_P p, int offset, uint32_t mask) {
        for (auto i = (id >> offset) & mask; i; --i) {
            p += strlen_P(p) + 1;
        }
        strcat_P(result, p);
    };

    if (id > 0xffffff) {
        id = 0xffffff;
    }
    cat(colors, 0, 0x1f);
    cat(languages, 5, 0x3f);
    cat(adjectives, 11, 0x7f);
    cat(animals, 18, 0x3f);

    return result;
}
