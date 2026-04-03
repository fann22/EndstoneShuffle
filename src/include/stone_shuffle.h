//
// Created by Loudbook on 9/29/24.
//

#pragma once

#include <chrono>
#include <filesystem>
#include <endstone/plugin/plugin.h>
#include <endstone/event/server/server_list_ping_event.h>

#include "json_loader.h"

class StoneShuffle : public endstone::Plugin {
public:
    StoneShuffle()
        : loader(JsonLoader(std::filesystem::current_path().string() + "/StoneShuffle/config.json")),
          currentIndex(0),
          lastShuffled(0) {}

    void onEnable() override {
        getLogger().info("StoneShuffle enabled!");
        registerEvent(&StoneShuffle::onServerListPingEvent, *this);
    }

    void onDisable() override {
        getLogger().info("StoneShuffle disabled.");
    }

    void onServerListPingEvent(endstone::ServerListPingEvent& event) {
        const auto& motds = loader.getStringList();
        if (motds.empty()) return;

        long now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        long refreshMs = loader.getRefreshInterval() * 1000L;

        if (lastShuffled + refreshMs < now) {
            lastShuffled = now;
            currentIndex = (currentIndex + 1) % motds.size();
        }

        event.setMotd(motds[currentIndex]);
    }

private:
    JsonLoader loader;
    size_t currentIndex;
    long lastShuffled;
};
