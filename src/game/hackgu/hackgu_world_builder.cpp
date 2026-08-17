#include "hackgu_world_builder.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace te::hackgu {

bool DiscWorldBuilder::register_disc(const DiscDescriptor& disc) {
    if (disc.id.empty() || disc.data_root.empty()) return false;
    m_discs[disc.id] = disc;
    return true;
}

std::vector<DiscDescriptor> DiscWorldBuilder::available_discs() const {
    std::vector<DiscDescriptor> out;
    out.reserve(m_discs.size());
    for (const auto& [id, desc] : m_discs) out.push_back(desc);
    return out;
}

bool DiscWorldBuilder::build_world_for_disc(const std::string& disc_id) {
    auto it = m_discs.find(disc_id);
    if (it == m_discs.end()) return false;

    const DiscDescriptor& disc = it->second;
    std::vector<std::string> collected;

    if (!std::filesystem::exists(disc.data_root)) {
        std::cerr << "[hackgu] missing data root: " << disc.data_root << "\n";
        return false;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(disc.data_root)) {
        if (!entry.is_regular_file()) continue;
        const auto path = entry.path();
        if (path.extension() == ".ccs") {
            collected.push_back(path.string());
        }
    }

    std::sort(collected.begin(), collected.end());

    std::ofstream manifest(disc.data_root + "/../" + disc.id + "_manifest.txt");
    manifest << "disc=" << disc.id << "\n";
    manifest << "title=" << disc.title << "\n";
    manifest << "arc=" << disc.arc << "\n";
    manifest << "ccs_count=" << collected.size() << "\n";
    for (const auto& p : collected) manifest << p << "\n";
    manifest.close();

    std::cout << "[hackgu] built world for " << disc.title
              << " with " << collected.size() << " ccs files\n";
    return true;
}

} // namespace te::hackgu
