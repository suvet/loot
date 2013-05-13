/*  BOSS

    A plugin load order optimiser for games that use the esp/esm plugin system.

    Copyright (C) 2012    WrinklyNinja

    This file is part of BOSS.

    BOSS is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    BOSS is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BOSS.  If not, see
    <http://www.gnu.org/licenses/>.
*/
#ifndef __BOSS_METADATA__
#define __BOSS_METADATA__

#include "game.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <list>
#include <set>

namespace boss {


    //A FormID is a 32 bit unsigned integer of the form xxYYYYYY in hex. The xx is the position in the masters list of the plugin that the FormID is from, and the YYYYYY is the rest of the FormID. Here the xx bit is stored as the corresponding filename to allow comparison between FormIDs from different plugins.
    class FormID {
    public:
        FormID();
        FormID(const std::string& pluginName, const uint32_t objectID);

        //The masters here also includes the plugin that they are masters of as the last element.
        FormID(const std::vector<std::string>& masters, const uint32_t formID);

        bool operator == (const FormID& rhs) const;
        bool operator != (const FormID& rhs) const;
        bool operator < (const FormID& rhs) const;
        bool operator > (const FormID& rhs) const;
        bool operator <= (const FormID& rhs) const;
        bool operator >= (const FormID& rhs) const;

        std::string Plugin() const;
        uint32_t Id() const;
    private:
        std::string plugin;
        uint32_t id;
    };

    class ConditionalData {
    public:
        ConditionalData();
        ConditionalData(const std::string& condition);
        ConditionalData(const std::string& condition, const std::string& data);

        bool IsConditional() const;
        bool EvalCondition(boss::Game& game) const;

        std::string Condition() const;
        std::string Data() const;

        void Data(const std::string& data);
    private:
        std::string condition;
        std::string data;
    };

    class Message : public ConditionalData {
    public:
        Message();
        Message(const std::string& type, const std::string& content);
        Message(const std::string& type, const std::string& content,
                const std::string& condition, const std::string& language);
                
        bool operator < (const Message& rhs) const;
        bool operator == (const Message& rhs) const;

        bool EvalCondition(boss::Game& game, const std::string& language) const;

        std::string Type() const;
        std::string Language() const;
        std::string Content() const;
    private:
        std::string type;
        std::string language;
    };

    class File : public ConditionalData {
    public:
        File();
        File(const std::string& name);
        File(const std::string& name, const std::string& display,
                                     const std::string& condition);

        bool operator < (const File& rhs) const;
        bool operator == (const File& rhs) const;

        std::string Name() const;
        std::string DisplayName() const;
    private:
        std::string display;
    };

    class Tag : public ConditionalData {
    public:
        Tag();
        Tag(const std::string& tag);
        Tag(const std::string& tag, const std::string& condition);

        bool operator < (const Tag& rhs) const;
        bool operator == (const Tag& rhs) const;

        bool IsAddition() const;
        std::string Name() const;
        std::string PrefixedName() const;  //Name with '-' in front if suggested for removal.
    private:
        bool addTag;
    };

    class Plugin {
    public:
        Plugin();
        Plugin(const std::string& name);
        Plugin(boss::Game& game, const std::string& name, const bool headerOnly);

        void Merge(const Plugin& plugin, bool ifdDisabled = false);
        Plugin DiffMetadata(const Plugin& plugin) const;

        std::string Name() const;
        bool Enabled() const;
        int Priority() const;
        std::set<File> LoadAfter() const;
        std::set<File> Reqs() const;
        std::set<File> Incs() const;
        std::list<Message> Messages() const;
        std::set<Tag> Tags() const;
        
        std::set<FormID> FormIDs() const;
        std::vector<std::string> Masters() const;
        bool IsMaster() const;  //Checks master bit flag.
        std::string Version() const;
        uint32_t Crc() const;
        bool IsActive() const;

        void Name(const std::string& name);
        void Enabled(const bool enabled);
        void Priority(const int priority);
        void LoadAfter(const std::set<File>& after);
        void Reqs(const std::set<File>& reqs);
        void Incs(const std::set<File>& incs);
        void Messages(const std::list<Message>& messages);
        void Tags(const std::set<Tag>& tags);

        void EvalAllConditions(boss::Game& game, const std::string& language);
        bool HasNameOnly() const;
        bool IsRegexPlugin() const;

        //Compare name strings.
        bool operator == (const Plugin& rhs) const;
        bool operator != (const Plugin& rhs) const;

        //Compare load order positions.
        bool operator < (const Plugin& rhs) const;
        bool operator > (const Plugin& rhs) const;
        bool operator <= (const Plugin& rhs) const;
        bool operator >= (const Plugin& rhs) const;

        //Load ordering functions.
        std::set<FormID> OverlapFormIDs(const Plugin& plugin) const;
        bool MustLoadAfter(const Plugin& plugin) const;  //Checks masters, reqs and loadAfter.

        //Validity checks.
        std::map<std::string,bool> CheckInstallValidity(const Game& game) const;  //Checks that reqs and masters are all present, and that no incs are present. Returns a map of filenames and whether they are missing (if bool is true, then filename is a req or master, otherwise it's an inc).
    private:
        std::string name;
        bool enabled;  //Default to true.
        int priority;  //Default to 0 : >0 is lower down in load order, <0 is higher up.
        std::set<File> loadAfter;
        std::set<File> requirements;
        std::set<File> incompatibilities;
        std::list<Message> messages;
        std::set<Tag> tags;
        
        std::vector<std::string> masters;
        std::set<FormID> formIDs;
        std::string version;  //Obtained from description field.
        bool isMaster;
        bool isActive;
        uint32_t crc;
    };
    
    bool operator == (const File& lhs, const Plugin& rhs);
}

#endif
