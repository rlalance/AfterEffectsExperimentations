//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     DesignSystem data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json &j, const std::shared_ptr<T> &opt)
        {
            if (!opt)
                j = nullptr;
            else
                j = *opt;
        }

        static std::shared_ptr<T> from_json(const json &j)
        {
            if (j.is_null())
                return std::unique_ptr<T>();
            else
                return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace Youi
{
    using nlohmann::json;

    inline json get_untyped(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json &j, std::string property)
    {
        return get_untyped(j, property.data());
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, std::string property)
    {
        return get_optional<T>(j, property.data());
    }

    struct ColorRgba
    {
        std::shared_ptr<double> a;
        std::shared_ptr<double> b;
        std::shared_ptr<double> g;
        std::shared_ptr<double> r;
    };

    struct FlavorColor
    {
        std::shared_ptr<ColorRgba> color_rgba;
        std::shared_ptr<std::string> name;
    };

    struct Metadata
    {
        std::shared_ptr<std::vector<std::string>> prop;
    };

    struct ColorsFlavor
    {
        std::shared_ptr<std::vector<FlavorColor>> flavor_colors;
        std::shared_ptr<Metadata> metadata;
        std::shared_ptr<std::string> name;
    };

    struct Colors
    {
        std::shared_ptr<std::vector<ColorsFlavor>> flavors;
    };

    struct Instructions
    {
        std::shared_ptr<std::string> design_system_format;
        std::shared_ptr<std::vector<std::string>> metadata;
    };

    struct Curve
    {
        std::shared_ptr<std::string> curvetype;
        std::shared_ptr<std::vector<nlohmann::json>> points;
    };

    struct MotionFlavor
    {
        std::shared_ptr<Curve> curve;
        std::shared_ptr<int64_t> delay;
        std::shared_ptr<std::string> delayunits;
        std::shared_ptr<std::string> description;
        std::shared_ptr<double> duration;
        std::shared_ptr<std::string> durationunit;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::string> title;
        std::shared_ptr<std::string> transformation;
    };

    struct Motion
    {
        std::shared_ptr<std::vector<MotionFlavor>> flavors;
    };

    struct TypographyFlavor
    {
        std::shared_ptr<std::string> alignment;
        std::shared_ptr<std::vector<double>> color;
        std::shared_ptr<std::string> font;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::string> rem;
        std::shared_ptr<double> size;
    };

    struct Typography
    {
        std::shared_ptr<std::vector<TypographyFlavor>> flavors;
    };

    struct DesignSystem
    {
        std::shared_ptr<Colors> colors;
        std::shared_ptr<std::map<std::string, nlohmann::json>> components;
        std::shared_ptr<Instructions> instructions;
        std::shared_ptr<Motion> motion;
        std::shared_ptr<Typography> typography;
    };
}

namespace nlohmann
{
    void from_json(const json &j, Youi::ColorRgba &x);
    void to_json(json &j, const Youi::ColorRgba &x);

    void from_json(const json &j, Youi::FlavorColor &x);
    void to_json(json &j, const Youi::FlavorColor &x);

    void from_json(const json &j, Youi::Metadata &x);
    void to_json(json &j, const Youi::Metadata &x);

    void from_json(const json &j, Youi::ColorsFlavor &x);
    void to_json(json &j, const Youi::ColorsFlavor &x);

    void from_json(const json &j, Youi::Colors &x);
    void to_json(json &j, const Youi::Colors &x);

    void from_json(const json &j, Youi::Instructions &x);
    void to_json(json &j, const Youi::Instructions &x);

    void from_json(const json &j, Youi::Curve &x);
    void to_json(json &j, const Youi::Curve &x);

    void from_json(const json &j, Youi::MotionFlavor &x);
    void to_json(json &j, const Youi::MotionFlavor &x);

    void from_json(const json &j, Youi::Motion &x);
    void to_json(json &j, const Youi::Motion &x);

    void from_json(const json &j, Youi::TypographyFlavor &x);
    void to_json(json &j, const Youi::TypographyFlavor &x);

    void from_json(const json &j, Youi::Typography &x);
    void to_json(json &j, const Youi::Typography &x);

    void from_json(const json &j, Youi::DesignSystem &x);
    void to_json(json &j, const Youi::DesignSystem &x);

    inline void from_json(const json &j, Youi::ColorRgba &x)
    {
        x.a = Youi::get_optional<double>(j, "a");
        x.b = Youi::get_optional<double>(j, "b");
        x.g = Youi::get_optional<double>(j, "g");
        x.r = Youi::get_optional<double>(j, "r");
    }

    inline void to_json(json &j, const Youi::ColorRgba &x)
    {
        j = json::object();
        j["a"] = x.a;
        j["b"] = x.b;
        j["g"] = x.g;
        j["r"] = x.r;
    }

    inline void from_json(const json &j, Youi::FlavorColor &x)
    {
        x.color_rgba = Youi::get_optional<Youi::ColorRgba>(j, "colorRGBA");
        x.name = Youi::get_optional<std::string>(j, "name");
    }

    inline void to_json(json &j, const Youi::FlavorColor &x)
    {
        j = json::object();
        j["colorRGBA"] = x.color_rgba;
        j["name"] = x.name;
    }

    inline void from_json(const json &j, Youi::Metadata &x)
    {
        x.prop = Youi::get_optional<std::vector<std::string>>(j, "prop");
    }

    inline void to_json(json &j, const Youi::Metadata &x)
    {
        j = json::object();
        j["prop"] = x.prop;
    }

    inline void from_json(const json &j, Youi::ColorsFlavor &x)
    {
        x.flavor_colors = Youi::get_optional<std::vector<Youi::FlavorColor>>(j, "flavorColors");
        x.metadata = Youi::get_optional<Youi::Metadata>(j, "metadata");
        x.name = Youi::get_optional<std::string>(j, "name");
    }

    inline void to_json(json &j, const Youi::ColorsFlavor &x)
    {
        j = json::object();
        j["flavorColors"] = x.flavor_colors;
        j["metadata"] = x.metadata;
        j["name"] = x.name;
    }

    inline void from_json(const json &j, Youi::Colors &x)
    {
        x.flavors = Youi::get_optional<std::vector<Youi::ColorsFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const Youi::Colors &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, Youi::Instructions &x)
    {
        x.design_system_format = Youi::get_optional<std::string>(j, "designSystemFormat");
        x.metadata = Youi::get_optional<std::vector<std::string>>(j, "metadata");
    }

    inline void to_json(json &j, const Youi::Instructions &x)
    {
        j = json::object();
        j["designSystemFormat"] = x.design_system_format;
        j["metadata"] = x.metadata;
    }

    inline void from_json(const json &j, Youi::Curve &x)
    {
        x.curvetype = Youi::get_optional<std::string>(j, "curvetype");
        x.points = Youi::get_optional<std::vector<json>>(j, "points");
    }

    inline void to_json(json &j, const Youi::Curve &x)
    {
        j = json::object();
        j["curvetype"] = x.curvetype;
        j["points"] = x.points;
    }

    inline void from_json(const json &j, Youi::MotionFlavor &x)
    {
        x.curve = Youi::get_optional<Youi::Curve>(j, "curve");
        x.delay = Youi::get_optional<int64_t>(j, "delay");
        x.delayunits = Youi::get_optional<std::string>(j, "delayunits");
        x.description = Youi::get_optional<std::string>(j, "description");
        x.duration = Youi::get_optional<double>(j, "duration");
        x.durationunit = Youi::get_optional<std::string>(j, "durationunit");
        x.name = Youi::get_optional<std::string>(j, "name");
        x.title = Youi::get_optional<std::string>(j, "title");
        x.transformation = Youi::get_optional<std::string>(j, "transformation");
    }

    inline void to_json(json &j, const Youi::MotionFlavor &x)
    {
        j = json::object();
        j["curve"] = x.curve;
        j["delay"] = x.delay;
        j["delayunits"] = x.delayunits;
        j["description"] = x.description;
        j["duration"] = x.duration;
        j["durationunit"] = x.durationunit;
        j["name"] = x.name;
        j["title"] = x.title;
        j["transformation"] = x.transformation;
    }

    inline void from_json(const json &j, Youi::Motion &x)
    {
        x.flavors = Youi::get_optional<std::vector<Youi::MotionFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const Youi::Motion &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, Youi::TypographyFlavor &x)
    {
        x.alignment = Youi::get_optional<std::string>(j, "alignment");
        x.color = Youi::get_optional<std::vector<double>>(j, "color");
        x.font = Youi::get_optional<std::string>(j, "font");
        x.name = Youi::get_optional<std::string>(j, "name");
        x.rem = Youi::get_optional<std::string>(j, "rem");
        x.size = Youi::get_optional<double>(j, "size");
    }

    inline void to_json(json &j, const Youi::TypographyFlavor &x)
    {
        j = json::object();
        j["alignment"] = x.alignment;
        j["color"] = x.color;
        j["font"] = x.font;
        j["name"] = x.name;
        j["rem"] = x.rem;
        j["size"] = x.size;
    }

    inline void from_json(const json &j, Youi::Typography &x)
    {
        x.flavors = Youi::get_optional<std::vector<Youi::TypographyFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const Youi::Typography &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, Youi::DesignSystem &x)
    {
        x.colors = Youi::get_optional<Youi::Colors>(j, "colors");
        x.components = Youi::get_optional<std::map<std::string, json>>(j, "components");
        x.instructions = Youi::get_optional<Youi::Instructions>(j, "instructions");
        x.motion = Youi::get_optional<Youi::Motion>(j, "motion");
        x.typography = Youi::get_optional<Youi::Typography>(j, "typography");
    }

    inline void to_json(json &j, const Youi::DesignSystem &x)
    {
        j = json::object();
        j["colors"] = x.colors;
        j["components"] = x.components;
        j["instructions"] = x.instructions;
        j["motion"] = x.motion;
        j["typography"] = x.typography;
    }
}
