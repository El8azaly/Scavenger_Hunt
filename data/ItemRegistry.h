#pragma once
#include "data/ItemData.h"
#include <QHash>

class ItemRegistry {
public:
    static const QHash<QString, Item>& allItems() {
        static QHash<QString, Item> items;

        if (items.isEmpty()) {
            auto add = [&](const Item& i) { items.insert(i.id, i); };

            add({"sword", "Captain's Sword", "A trusty blade for slaying enemies.",
                 ":/assets/sprites/items/sword.json", ":/assets/sprites/items/sword.png",
                 14.0f, 38.0f, "", false});

            add({"brass_key", "Brass Key", "A shiny brass key. Wonder what it opens?",
                 ":/assets/sprites/items/key.json", ":/assets/sprites/items/key.png",
                 16.0f, 30.0f, "", true});

            add({"gold_coin", "Gold Coin", "A shiny gold coin.",
                 ":/assets/sprites/items/gold_coin.json", ":/assets/sprites/items/gold_coin.png",
                 22.0f, 22.0f, "", false});

            add({"blue_potion", "Blue Potion", "A mysterious blue liquid that restores mana.",
                 ":/assets/sprites/items/blue_potion.json", ":/assets/sprites/items/blue_potion.png",
                 14.0f, 26.0f, "", false});

            add({"red_potion", "Red Potion", "A crimson brew that restores health.",
                 ":/assets/sprites/items/red_potion.json", ":/assets/sprites/items/red_potion.png",
                 18.0f, 28.0f, "", false});

            add({"green_potion", "Green Potion", "A bubbling emerald concoction.",
                 ":/assets/sprites/items/green_potion.json", ":/assets/sprites/items/green_potion.png",
                 26.0f, 34.0f, "", false});

            add({"chest", "Small Chest", "A portable wooden chest.",
                 ":/assets/sprites/items/chest.json", ":/assets/sprites/items/chest.png",
                 128.0f, 128.0f, "", false});

            add({"diamond", "Diamond", "A brilliant, flawless gem.",
                 ":/assets/sprites/items/diamond.json", ":/assets/sprites/items/diamond.png",
                 28.0f, 24.0f, "", false});

            add({"emerald", "Emerald", "A precious green gemstone.",
                 ":/assets/sprites/items/emerald.json", ":/assets/sprites/items/emerald.png",
                 24.0f, 24.0f, "", false});

            add({"ruby", "Ruby", "A deep, blood-red gemstone.",
                 ":/assets/sprites/items/ruby.json", ":/assets/sprites/items/ruby.png",
                 26.0f, 26.0f, "", false});

            add({"map", "Treasure Map", "An old parchment detailing hidden paths.",
                 ":/assets/sprites/items/map.json", ":/assets/sprites/items/map.png",
                 30.0f, 32.0f, "", false});

            add({"silver_coin", "Silver Coin", "A polished silver coin.",
                 ":/assets/sprites/items/silver_coin.json", ":/assets/sprites/items/silver_coin.png",
                 22.0f, 22.0f, "", false});

            add({"skull", "Skull", "An eerie human skull.",
                 ":/assets/sprites/items/skull.json", ":/assets/sprites/items/skull.png",
                 32.0f, 50.0f, "", false});
        }
        return items;
    }

    static Item get(const QString& id) {
        return allItems().value(id, Item::null());
    }
};