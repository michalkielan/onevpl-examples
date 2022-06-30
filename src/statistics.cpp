// SPDX-License-Identifier: MIT

#include <fstream>

#include "statistics.hpp"

#include "nlohmann/json.hpp"

Statistics::Statistics(StatsDataFrame stats_data_frame) :
  stats_data_frame_{std::move(stats_data_frame)} {}

void Statistics::write(std::string output_filename) {
  nlohmann::json frames_info;
  for (const auto& frame_info : stats_data_frame_.frame_info) {
    nlohmann::json frame_info_json{{"frame", frame_info.counter},
                                   {"iframe", frame_info.iframe},
                                   {"size", frame_info.size},
                                   {"pts", -1},
                                   {"proctime", frame_info.stop_time - frame_info.start_time},
                                   {"starttime", frame_info.start_time},
                                   {"stoptime", frame_info.stop_time}};
    frames_info.push_back(frame_info_json);
  }
  nlohmann::json stats{{"id", stats_data_frame_.id},
                       {"description", stats_data_frame_.description},
                       {"test", stats_data_frame_.test},
                       {"testdefinition", stats_data_frame_.test_definition},
                       {"date", stats_data_frame_.date},
                       {"encapp_version", stats_data_frame_.encapp_version},
                       {"proctime", stats_data_frame_.proctime},
                       {"framecount", stats_data_frame_.framecount},
                       {"encodedfile", stats_data_frame_.encoded_file},
                       {"sourcefile", stats_data_frame_.source_file},
                       {"frames", frames_info}};
  std::ofstream out_json_file{output_filename};
  out_json_file << std::setw(4) << stats << std::endl;
}
