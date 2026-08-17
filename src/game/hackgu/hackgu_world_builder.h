#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <map>

namespace te::hackgu {

struct DiscDescriptor {
    std::string id;          // disc1 / disc2 / disc3 / disc4
    std::string title;       // Infection / Rebirth / Reminisce / Quarantine
    std::string arc;         // infection / gu_returner
    uint32_t volume_index = 0;
    std::string data_root;   // absolute path to extracted data dir
    std::vector<std::string> ccs_files;
};

struct WorldBuildResult {
    bool success = false;
    std::string disc_id;
    std::string world_name;
    std::vector<std::string> generated_assets;
    std::string message;
};

class DiscWorldBuilder {
public:
    DiscWorldBuilder() = default;
    ~DiscWorldBuilder() = default;

    bool register_disc(const DiscDescriptor& disc);
    bool build_world_for_disc(const std::string& disc_id);
    std::vector<DiscDescriptor> available_discs() const;

    void set_seele_ai_enabled(bool enabled) { m_seele_enabled = enabled; }
    bool seele_ai_enabled() const { return m_seele_enabled; }

private:
    std::map<std::string, DiscDescriptor> m_discs;
    bool m_seele_enabled = true;
};

} // namespace te::hackgu
