/**
 * @file ShardIo.hpp
 * @brief qpudatashard CSV read/write for EcoNet integration
 * 
 * Handles RFC-4180 compliant CSV files for:
 * - Reading canyon input data
 * - Writing windnet prototype shards
 * - ALN-Blockchain record export
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef SHARD_IO_HPP
#define SHARD_IO_HPP

#include "WindnetGeometry.hpp"
#include "WindnetCaptureModel.hpp"
#include "WindnetEcoImpact.hpp"
#include <string>
#include <vector>
#include <fstream>

namespace windminer {
namespace io {

/**
 * @brief qpudatashard record structure (matches CSV schema)
 */
struct QpuDataShard {
    std::string nodeid;                     ///< Unique node identifier
    std::string assettype;                  ///< "UrbanWindTrashNet"
    std::string region;                     ///< City/region name
    double latitude;                        ///< Geographic latitude
    double longitude;                       ///< Geographic longitude
    double avgwindspeed_ms;                 ///< Effective mean U⊥ (m/s)
    std::string dominantwind_from;          ///< Wind sector (W, E, etc.)
    std::string season;                     ///< Seasonal window
    double netheight_m;                     ///< Net height (m)
    double netwidth_m;                      ///< Net width (m)
    double netporosity;                     ///< Open area fraction
    double expected_capture_kg_per_year;    ///< Annual capture (kg)
    int maintenance_visits_per_year;        ///< Cleanout frequency
    double ecoimpactscore;                  ///< Normalized score (0-1)
    double karmaperunit;                    ///< Karma units
    std::string notes;                      ///< Free-text notes
    
    /**
     * @brief Default constructor
     */
    QpuDataShard()
        : latitude(0.0)
        , longitude(0.0)
        , avgwindspeed_ms(0.0)
        , netheight_m(0.0)
        , netwidth_m(0.0)
        , netporosity(0.6)
        , expected_capture_kg_per_year(0.0)
        , maintenance_visits_per_year(52)
        , ecoimpactscore(0.0)
        , karmaperunit(0.0)
    {}
};

/**
 * @brief CSV reader/writer for qpudatashards
 */
class ShardIo {
private:
    std::string filepath_;
    
public:
    /**
     * @brief Default constructor
     */
    ShardIo() = default;
    
    /**
     * @brief Constructor with file path
     * @param filepath Path to CSV file
     */
    explicit ShardIo(const std::string& filepath)
        : filepath_(filepath)
    {}
    
    /**
     * @brief Read canyon data from CSV
     * @param filepath Input CSV path
     * @return Vector of Canyon structs
     */
    static std::vector<geometry::Canyon> readCanyons(const std::string& filepath);
    
    /**
     * @brief Write qpudatashard records to CSV
     * @param filepath Output CSV path
     * @param records Vector of QpuDataShard records
     * @return true if successful
     */
    static bool writeShards(
        const std::string& filepath,
        const std::vector<QpuDataShard>& records
    );
    
    /**
     * @brief Read qpudatashard records from CSV
     * @param filepath Input CSV path
     * @return Vector of QpuDataShard records
     */
    static std::vector<QpuDataShard> readShards(const std::string& filepath);
    
    /**
     * @brief Convert EcoImpactResult to QpuDataShard
     * @param eco_result Eco-impact evaluation result
     * @param canyon Source canyon data
     * @param net Windnet specifications
     * @return QpuDataShard record
     */
    static QpuDataShard ecoResultToShard(
        const impact::EcoImpactResult& eco_result,
        const geometry::Canyon& canyon,
        const geometry::Windnet& net
    );
    
    /**
     * @brief Generate prototype shard filename
     * @param city City name
     * @param version Version string
     * @return Filename string
     */
    static std::string generateShardFilename(
        const std::string& city,
        const std::string& version
    );
};

/**
 * @brief CSV field delimiter (RFC-4180 compliant)
 */
namespace csv {
    constexpr char DELIMITER = ',';
    constexpr char QUOTE = '"';
    constexpr char NEWLINE = '\n';
}

} // namespace io
} // namespace windminer

#endif // SHARD_IO_HPP
