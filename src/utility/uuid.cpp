#include "kit/internal/pch.hpp"
#include "kit/utility/uuid.hpp"

namespace kit
{
static thread_local std::random_device s_device;
static thread_local std::mt19937_64 s_eng(s_device());
static thread_local std::uniform_int_distribution<uint64_t> s_dist;

uuid::uuid() : m_uuid(s_dist(s_eng))
{
}

uuid::uuid(const std::uint64_t uuid) : m_uuid(uuid)
{
}

uuid::operator uint64_t() const
{
    return m_uuid;
}

bool operator==(const uuid &id1, const uuid &id2)
{
    return (std::uint64_t)id1 == (std::uint64_t)id2;
}
bool operator!=(const uuid &id1, const uuid &id2)
{
    return (std::uint64_t)id1 != (std::uint64_t)id2;
}

static bool in_between(const std::uint32_t x, const std::uint32_t mm, const std::uint32_t mx)
{
    return x <= mm && x <= mx;
}

const std::string &uuid::random_name_from_id(const uuid id, const std::uint32_t min_characters,
                                             const std::uint32_t max_characters)
{
    KIT_ASSERT_ERROR(min_characters <= max_characters,
                     "Maximum characters must be greater or equal than minimum characters")

    thread_local static std::unordered_map<uuid, std::string> names;
    if (names.find(id) != names.end() && in_between((std::uint32_t)names.at(id).size(), min_characters, max_characters))
        return names.at(id);

    srand((std::uint32_t)id);
    static constexpr const char syllables[6] = "aieou";
    static constexpr const char consonants[22] = "bcdfghjklmnpqrstvwxyz";

    std::string name;
    name.reserve(max_characters);

    const std::uint8_t is_pair = (std::uint8_t)(rand() % 2);
    while (name.size() < max_characters)
    {
        const bool is_syllable = name.size() % 2 == is_pair && rand() % 12 < 11;
        const std::size_t idx = (std::size_t)(rand() % (is_syllable ? 5 : 21));
        if (name.size() >= min_characters && (is_syllable ? (idx == 4) : (idx == 20)))
            break;
        name.push_back(is_syllable ? syllables[idx] : consonants[idx]);
    }

    name[0] = (char)toupper(name[0]);
    names[id] = name;
    return names.at(id);
}
} // namespace kit

std::size_t std::hash<kit::uuid>::operator()(const kit::uuid &key) const
{
    return std::hash<uint64_t>()((std::uint64_t)key);

} // namespace std